# SF OVHD — firmware

MobiFlight custom device that drives the three OLED displays of the A320
overhead panel: the BATT 1 and BATT 2 voltmeters and the ADIRS display.

The 49 switches and the annunciator lights are plain MobiFlight buttons and
output shifters, configured in the Connector. They do not go through the
custom device. `Community/config/SF OVHD Mainboard.mfmc` has the whole
module config the board runs, ready to load.

## What this tracks

The reference is **MobiFlight's own scaffolding**: the project layout,
`platformio.ini`, the build scripts and `MFCustomDevice.*` follow
[MobiFlight/CommunityTemplate](https://github.com/MobiFlight/CommunityTemplate),
and `custom_core_firmware_version` is kept at the core firmware release the
template points to — currently **3.1.4**. `src/` is not in the repository;
`get_CoreFiles.py` clones the core on the first build.

`OLEDInterface.h` and the fonts come from Gagagu's
[A320 EFIS/FCU display](https://github.com/gagagu/Mobiflight-A320-Efis-Fcu-Display-with-ESP32)
by way of [elral/MF_FCU_EFIS_OLEDs](https://github.com/elral/MF_FCU_EFIS_OLEDs),
the same base as the SF FCU firmware. It could not have been done without
them.

## What differs from the template

| | |
|---|---|
| Device type | `SF_OVHD`; `SF_OVHD_MAINBOARD`, the January 2025 name, is accepted too |
| `SERIAL_RX_BUFFER_SIZE` | 256, not the template's 96 — see below |
| Device families | buttons, output shifters and the custom device only |
| Targets | Mega only |

### Multiplexer channels

The three displays hang off a PCA9548A, one display per channel:

| Channel | Display |
|---|---|
| 0 | BATT 1 voltage |
| 1 | BATT 2 voltage |
| 2 | ADIRS |

The channel numbers live in `SF_OVHD.h`. Change them there if the displays
are wired to different channels.

### Messages

| ID | Label | Value |
|---|---|---|
| 0 | Battery 1 Voltage | shown as sent, e.g. `28.63` |
| 1 | Battery 2 Voltage | shown as sent |
| 2 | Adirs message | shown as text, e.g. `On Batt` |
| 3 | Light test | `1` shows the test picture on all three, `0` goes back to the values |
| −1 | *Connector closing* | all three displays go dark |
| −2 | *power saving*, after the 600 s in the `.mfmc` | all three displays go dark |

An empty value blanks its display too. Use that from the Connector for a
display that should be dark, for instance while its bus is unpowered.

The voltages are right-aligned on the digit cells: DSEG14 gives every digit a
32-pixel cell and the point none, so `28.80` fills the 128 pixels and `9.80`
starts one cell in, with each digit in its usual column.

### Serial receive buffer

Redrawing one display pushes a 1 KB frame over I2C at 400 kHz, about 25 ms
during which the loop reads nothing. At 115200 baud close to 300 bytes can
arrive in that window, so the template's 96-byte receive buffer would drop
characters. Do not lower it.

### Footprint

The ATmega2560 has 8 KB of RAM, and MobiFlight reserves most of it up
front: 1600 bytes for the device arena (`MF_MAX_DEVICEMEM`) and 1000 for
the input names (`MEMLEN_NAMES_BUFFER`). Those two are sized for the
Connector config, not for this code, and are the wrong place to economise.

What this project does instead:

* **`build_unflags` turns off the device families the panel does not
  have**: segment displays, character LCD, steppers, servos, analog inputs,
  input shifters and both multiplexers. Output shifters stay on, because the
  annunciators are DM13A chains. Buttons, encoders and outputs are core
  and always present. To get a family back, delete its line.
* **No `String` anywhere.** The values from the Connector live in fixed
  buffers, so a redraw allocates nothing and the heap cannot fragment. The
  only allocation in the whole firmware is Adafruit's 1 KB frame buffer,
  taken once at startup.

## I2C address and display type

The custom device takes the address of the PCA9548A, which the DIP switch
on the mainboard sets: all off is 0x70.

The address also picks the display driver, which is easy to miss:

* **even** address (0x70) → **SH1106**
* **odd** address (0x71) → **SSD1306**

The board is configured at 0x71, i.e. SSD1306, and the shipped `.mfmc` is
set to match.

## Building

PlatformIO installed through the VS Code extension does not put `pio` on the
PATH. Either open a terminal from the PlatformIO toolbar, or call it by its
full path, `%USERPROFILE%\.platformio\penv\Scripts\pio.exe`.

```
pio run -e SF_OVHD_mega
```

That produces version `0.0.1`. For a real one set `VERSION` — the build
stamps it into both the firmware filename and `board.json`, which have to
agree or MobiFlight will not find the firmware. The version has to be an
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
# bash, and what the GitHub workflow uses
rm -rf _build _dist
VERSION=1.1.0 pio run -e SF_OVHD_mega
```

Deleting `_build` and `_dist` first is not optional. `copy_fw_files.py`
only stamps the version into `board.json` when `_build/` does not exist
yet, so a leftover folder ships the previous version's json. The ZIP is
also only rebuilt when the firmware itself recompiles. After changing
nothing but a file under `Community/`, run `pio run -e SF_OVHD_mega -t clean`
first.

The result is an installable ZIP in `_dist/`. Extract it into the `Community`
folder of your MobiFlight installation, then flash from the Connector.
