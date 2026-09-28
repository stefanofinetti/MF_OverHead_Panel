# EXT LT section board (v2)

The v2 board under the EXT LT panel. It carries the panel's switches, annunciator LEDs and backlight, at their v1 positions, and reaches the mainboard through one ribbon and one pair of backlight wires. There are no ICs on it. The design rules shared by the eight section boards are in [`docs/board-split.md`](../docs/board-split.md).

* **PCB:** 159.5 × 79.5 mm, 2 layers, 1.6 mm. Every v1 hole is kept.
* **Status:** routed with Freerouting. ERC, DRC and schematic parity are clean. Not yet ordered.

## Connectors

Both are new on v2 and sit on the **back**, so nothing new stands proud of the front.

| Ref | What | Goes to |
|---|---|---|
| J1 | Shrouded IDC header, 14 pins, SMD | J706 on the mainboard, straight ribbon, pin 1 to pin 1 |
| J2 | JST PH, 2 pins, SMD. Pin 1 `+9V_BL` (`+`), pin 2 the switched return (`−`) | screw terminal J606 on the mainboard |

J1 and J2 are at the right, over the mainboard, above its TLC5927 block.

The signal on every ribbon pin is in the design document: [EXT LT — 14 pins](../docs/board-split.md#ext-lt--14-pins).

## Switches

| Ref | Switch | Type | v1 ref |
|---|---|---|---|
| SW1 | EXTLT STROBE | toggle | S7 |
| SW2 | EXTLT BEACON | toggle | S6 |
| SW3 | EXTLT WING | toggle | S5 |
| SW4 | EXTLT NAV & LOGO | toggle | S4 |
| SW5 | EXTLT RWY TURN | toggle | S8 |
| SW6 | EXTLT LDG LIGHT L | toggle | S1 |
| SW7 | EXTLT LDG LIGHT R | toggle | S2 |
| SW8 | EXTLT NOSE LIGHT | toggle | S3 |

## Backlight

31 LEDs, 1206 (D1–D31), on 9 V: 15 strings of two with 150 Ω, and one single LED with R16, 330 Ω. Each string runs at about 17 mA. `K` marks every cathode on the silkscreen.

## Assembly notes

* No annunciators, so the ribbon carries no `+5V_LED`.
* Toggles: ON-OFF-ON for LDG LIGHT L, LDG LIGHT R, NOSE LIGHT and NAV & LOGO, read on both contacts (1 and 3); ON-OFF for STROBE, BEACON, WING and RWY TURN, read on contact 1. Lever up closes contact 1, the lower pin. They are generic three-terminal PCB-pin lever toggles from AliExpress, fitted by hand. The footprint is the v1 one, whose name still carries the E-Switch number 100SP1T1B4M2QE: that is not the part fitted, and the schematic gives the toggles no part number.
* D31 is a single-LED string: on v1 it was paired with D139, now on GPWS. R16, 330 Ω on the back, gives it the same 17 mA as a pair on 150 Ω.
* Every part has its value, polarity or pin 1 on the silkscreen of its own side. Each part's description in the schematic names its v1 reference.
