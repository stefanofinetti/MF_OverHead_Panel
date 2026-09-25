# SF OVHD — firmware

MobiFlight firmware for the overhead panel mainboard. It is standard
MobiFlight for the ATmega2560, plus one custom device, `SF_OVHD`, that
drives the three OLED displays: the BATT 1 and BATT 2 voltmeters and the
ADIRS display.

The 49 switches and the annunciator lights do not go through the custom
device. They are plain MobiFlight buttons and output shifters, configured in
the Connector like on any other board.

## Installing

1. Build the Community package `SF_OVHD_<version>.zip` as described
   below.
2. Extract it into the `Community` folder of your MobiFlight installation,
   so you end up with `Community\SF_OVHD\`. Restart the Connector.
3. Flash the board from the Connector. It shows up as **SF OVHD Mega**.
4. Load the module config `SF OVHD Mainboard.mfmc`, which comes with the
   package. It has every switch, both annunciator chains (`ANN_UPPER` and
   `ANN_LOWER`) and the custom device at 0x71, on the pins the board uses.
5. In your MobiFlight project, bind the switches and annunciators to your
   aircraft, and send the display values with the messages below.

## Messages

In the Connector, an output of type *Custom Device* on `SF OVHD` can send:

| ID | Label | Value |
|---|---|---|
| 0 | Battery 1 Voltage | shown as sent, e.g. `28.63` |
| 1 | Battery 2 Voltage | shown as sent |
| 2 | Adirs message | shown as text, e.g. `On Batt` |
| 3 | Light test | `1` shows the test picture on all three displays, `0` goes back to the values |
| 4 | Brightness | `0`–`100`, the contrast of all three displays |

The Connector also sends two messages of its own. Both switch all three
displays off, since a picture left standing for hours burns into an OLED:

| ID | When |
|---|---|
| −1 | the Connector closes |
| −2 | the Connector enters power saving, after the 600 s set in the `.mfmc` |

The next value that arrives lights its display again.

A few details worth knowing:

* **An empty value blanks its display.** Use that for a display that
  should be dark, for instance while its bus is unpowered.
* **The voltages are right-aligned on the digit cells.** DSEG14 gives every
  digit a 32-pixel cell and the point none, so `28.80` fills the 128 pixels
  and `9.80` starts one cell in, with each digit in its usual column. Send
  them already formatted: the firmware prints what it gets.
* **Brightness** is meant for the ANN LT BRT/DIM switch, which dims these
  displays in the aircraft, or for a dimmer knob. Until the first value
  arrives, the displays keep their driver's default. An empty value is
  ignored. It sets the OLED contrast register: even at 0 the displays are
  dimmer but still plainly lit. After a new module config is uploaded, the
  displays start again at the default until the Connector sends the value
  once more.

## Displays and address

The three displays hang off a PCA9548A, one per channel:

| Channel | Display |
|---|---|
| 0 | BATT 1 |
| 1 | BATT 2 |
| 2 | ADIRS |

The channel numbers live in `SF_OVHD/SF_OVHD.h`.

The custom device's only setting is the address of the PCA9548A, set by the
DIP switch on the board: all off is 0x70. The address also picks the display
driver, which is easy to miss:

* **even** address (0x70, 0x72 …) → **SH1106**
* **odd** address (0x71, 0x73 …) → **SSD1306**

The board is configured at 0x71, i.e. SSD1306, and the shipped `.mfmc` is set
to match. With SH1106 displays, move the DIP switch to an even address and
change the custom device to match.

## Building

The project is PlatformIO, usually run from VS Code. The extension does not
put `pio` on the PATH. Either open a terminal from the PlatformIO toolbar, or
call it by its full path, `%USERPROFILE%\.platformio\penv\Scripts\pio.exe`.

```
pio run -e SF_OVHD_mega
```

That produces version `0.0.1`. For a real one, set `VERSION`. The build
stamps it into both the firmware filename and `board.json`, and the two have
to agree or MobiFlight will not find the firmware. It has to be an
environment variable, so the syntax differs per shell:

```
rem cmd.exe
rmdir /s /q _build _dist
set "VERSION=1.1.0" && pio run -e SF_OVHD_mega
```

```
# PowerShell
Remove-Item -Recurse -Force _build, _dist -ErrorAction SilentlyContinue
$env:VERSION = "1.1.0"; pio run -e SF_OVHD_mega
```

```
# bash
rm -rf _build _dist
VERSION=1.1.0 pio run -e SF_OVHD_mega
```

The result is the installable ZIP in `_dist/`.

Two traps:

* **Delete `_build` and `_dist` first.** `copy_fw_files.py` only stamps the
  version into `board.json` when `_build/` does not exist yet, so a leftover
  folder ships the previous version's json.
* **The ZIP is only rebuilt when the firmware recompiles.** After changing
  nothing but a file under `Community/`, run
  `pio run -e SF_OVHD_mega -t clean` first.

On the first build, `get_CoreFiles.py` clones the MobiFlight core into
`src/`, which is not in the repository.

## How it is put together

### What it follows

The reference is **MobiFlight's own scaffolding**: the project layout,
`platformio.ini`, the build scripts and `MFCustomDevice.*` follow
[MobiFlight/CommunityTemplate](https://github.com/MobiFlight/CommunityTemplate),
and `custom_core_firmware_version` is kept at the core release the template
points to, currently **3.1.4**. Where this project departs from the template:

| | |
|---|---|
| Device type | `SF_OVHD`. `SF_OVHD_MAINBOARD`, the January 2025 name, is accepted too, so a board configured back then keeps its displays |
| `SERIAL_RX_BUFFER_SIZE` | 256, not the template's 96, see below |
| Device families | buttons, output shifters and the custom device only |
| Targets | Mega only, the board has a soldered ATmega2560 |

### Serial receive buffer

Redrawing a display pushes a 1 KB frame over I2C at 400 kHz, about 25 ms
during which the loop reads nothing. At 115200 baud close to 300 bytes can
arrive in that window, so the template's 96-byte buffer would drop
characters. Do not lower it.

### Memory

The ATmega2560 has 8 KB of RAM, and MobiFlight reserves most of it up
front: 1600 bytes for the device arena (`MF_MAX_DEVICEMEM`) and 1000 for
the input names (`MEMLEN_NAMES_BUFFER`). Those two are sized for the
Connector config, not for this code, and are the wrong place to economise.

What this project does instead:

* **`build_unflags` turns off the device families the panel does not
  have**: segment displays, character LCD, steppers, servos, analog inputs,
  input shifters and both multiplexers. Output shifters stay on, because the
  annunciators are DM13A chains. Buttons, encoders and outputs are core
  and always present. To get a family back, delete its line in
  `SF_OVHD/sf_ovhd_platformio.ini`.
* **No `String` anywhere.** The values from the Connector live in fixed
  buffers, so a redraw allocates nothing and the heap cannot fragment. The
  only allocation in the whole firmware is Adafruit's 1 KB frame buffer,
  taken once at startup.

The result uses about 50% of the RAM and 16% of the flash.

### Files

| File | |
|---|---|
| `SF_OVHD/SF_OVHD.cpp`, `.h` | the three displays: drawing, messages, channels |
| `SF_OVHD/OLEDInterface.h` | one interface over the SSD1306 and SH1106 drivers |
| `SF_OVHD/MFCustomDevice.cpp`, `.h` | the glue to the MobiFlight core, as in the template |
| `SF_OVHD/Fonts/` | GFX fonts. Two are used, DSEG14 Modern 20 pt and, from Adafruit GFX, FreeSans 18 pt |
| `SF_OVHD/Community/` | what goes into the package: board, device, module config, reset image |

## Credits

`OLEDInterface.h` and the fonts come from Gagagu's
[A320 EFIS/FCU display](https://github.com/gagagu/Mobiflight-A320-Efis-Fcu-Display-with-ESP32)
by way of [elral/MF_FCU_EFIS_OLEDs](https://github.com/elral/MF_FCU_EFIS_OLEDs),
the same base as the SF FCU firmware. The DSEG fonts are
[keshikan/DSEG](https://github.com/keshikan/DSEG). It could not have been
done without them.
