# Overhead Panel

A compact Airbus A319/320/321/330 overhead panel for MobiFlight: 3D-printed
panels and Korry-style push buttons, a single mainboard that carries every
switch, light and display, and the firmware for its three OLED displays.

The panel covers **ADIRS**, **AIR COND**, **ANTI ICE**, **APU**, **ELEC**,
**EXT LT**, **FUEL**, **GPWS** and **SIGNS**.

## What is in the repository

| Folder | What it holds |
|---|---|
| `STL Files/` | Everything to print: one STL per filament colour for each panel, a ready `.3mf` for multi-colour printers, the Korry buttons and cases, the four case parts, the frames and the support |
| `STL Files/FaceDown_Printing/` | The APU panel redesigned to print face down. The other panels will follow |
| `STL Files/Levers/` | Heads for the exterior light levers |
| `3D Files/` | The SketchUp sources for the panel, the Korry buttons and the support, if you want to change dimensions |
| `Kicad Files/` | The mainboard: schematic, layout and the Fabrication Toolkit settings for JLCPCB |
| `SF_OVHD/` | The MobiFlight firmware and the installable Community package, with the complete module config for the panel. See [its readme](SF_OVHD/README.md) |

`OverheadPanel.zip` is an older snapshot of `Kicad Files/` from January 2025,
kept for reference. The current project is the folder.

## The mainboard

One four-layer board, 313 × 206 mm, sits behind the whole panel. The inner
layers are a +9 V plane and a ground plane. It is designed to be soldered by
hand: nearly all SMD parts are 1206, and the crystals are the 7 × 5 mm
hand-soldering kind.

* **Microcontroller:** an ATmega2560 on the board itself, with a CH340G for
  USB. MobiFlight sees it as a Mega.
* **Power:** a 9 V barrel jack for the lights. A MIC29302 makes 5 V for the
  logic and an AMS1117 makes 3.3 V for the displays. USB-B carries the data.
  JP1 is the power selector.
* **Inputs:** 24 Korry push buttons (G-Switch PS-7054DVB-6PN), 11 toggle
  switches (E-Switch 100SP1T1B4M2QE) for EXT LT and SIGNS, and three 8-way
  rotary switches for the ADIRS mode selectors. All of them go straight to
  the ATmega's pins.
* **Annunciators:** four DM13A constant-current drivers, one per legend
  colour (white, green, blue and orange, i.e. amber), in two shift register chains. Each
  colour has its own trimmer, so the four can be balanced against each
  other.
* **Backlighting:** 123 LEDs on the 9 V rail through fixed 150 Ω resistors.
* **Displays:** three 128 × 64 I2C OLEDs (BATT 1, BATT 2 and ADIRS) on 4-pin
  sockets, behind a PCA9548A multiplexer. A three-way DIP switch sets the
  multiplexer's address.

The BOM is in the KiCad project (KiCad 9 or later). Before ordering, run the
Fabrication Toolkit plugin to regenerate the Gerbers, BOM and positions. The
files in `Kicad Files/production/` date from 3 March 2025, and the board has
changed since.

## Building one

1. **Print** the panels, buttons, cases and frames from `STL Files/`. Each
   panel comes as separate STLs in black, grey, white and transparent, or as
   one `.3mf` ready for a multi-colour printer.
2. **Order and assemble the board** from `Kicad Files/`.
3. **Burn the bootloader.** A new ATmega2560 is blank. J4 (BootLoader) is a
   standard 6-pin ISP header: burn the Arduino Mega bootloader through it
   once, with any ISP programmer. After that the board is flashed over USB
   like a Mega.
4. **Install the firmware.** Build or download the `SF_OVHD` Community
   package, extract it into MobiFlight's `Community` folder, and flash the
   board from the Connector. [The firmware readme](SF_OVHD/README.md) has
   the details.
5. **Load the module config.** The package carries
   `SF OVHD Mainboard.mfmc`, which already has all 49 switches, both
   annunciator chains and the three displays on their pins. Then bind them
   to your aircraft's variables in your MobiFlight project.

## Software

* [KiCad](https://www.kicad.org/), to change the board
* [SketchUp](https://www.sketchup.com), to change the printed parts
* [VS Code](https://code.visualstudio.com/) with
  [PlatformIO](https://platformio.org/), to build the firmware
* [MobiFlight](https://www.mobiflight.com/)

```bash
git clone https://github.com/stefanofinetti/MF_OverHead_Panel.git
```

## License

This code and all the items are released under the GPLv3.0 license. Feel free
to use them as you wish, as long as you redistribute the source code.
A mention would be nice, though, if you use this work: I invested a good
many hours in it, just for the amazing MobiFlight community.

## Acknowledgements

This project couldn't have seen the light without the excellent work, and
kind support, of [GaGagu](https://github.com/gagagu) and
[ElRal](https://github.com/elral). Their work is amazing, so please have a
look at their repositories.
