# ADIRS section board (v2)

The v2 board under the ADIRS panel. It carries the panel's switches, annunciator LEDs and backlight, at their v1 positions, and reaches the mainboard through one ribbon and one pair of backlight wires. There are no ICs on it. The design rules shared by the eight section boards are in [`docs/board-split.md`](../docs/board-split.md).

* **PCB:** 132.5 × 81.5 mm, 2 layers, 1.6 mm. Every v1 hole is kept.
* **Status:** routed with Freerouting. ERC, DRC and schematic parity are clean. Not yet ordered.

## Connectors

Both are new on v2 and sit on the **back**, so nothing new stands proud of the front.

| Ref | What | Goes to |
|---|---|---|
| J1 | Shrouded IDC header, 20 pins, SMD | J701 on the mainboard, straight ribbon, pin 1 to pin 1 |
| J2 | JST PH, 2 pins, SMD. Pin 1 `+9V_BL` (`+`), pin 2 the switched return (`−`) | screw terminal J601 on the mainboard |

J1 and J2 are on the back between ADR 1 and ADR 3.

The signal on every ribbon pin is in the design document: [ADIRS — 20 pins](../docs/board-split.md#adirs--20-pins).

## Switches

| Ref | Switch | Type | v1 ref |
|---|---|---|---|
| SW1 | ADIRS ADR 1 | ADR rotary | SW25 |
| SW2 | ADIRS ADR 3 | ADR rotary | SW26 |
| SW3 | ADIRS ADR 2 | ADR rotary | SW27 |
| SW4 | ADIRS GND CTL | Korry | SW24 |

## Annunciators

Anodes on `+5V_LED` from the ribbon, cathodes back down the ribbon to their TLC5927 output. No resistors: the TLC5927 sets the current.

| Ref | Colour | Legend | Chain bit | Driver | v1 ref |
|---|---|---|---|---|---|
| D1 | blue | `GND_CTL_ON` | `ANN_LOWER` 16 | U502 (v1 U7) | D109 |

## Backlight

18 LEDs, 1206 (D2–D19), on 9 V, in 9 strings of two with 150 Ω (R1–R9). Each string runs at about 17 mA. `K` marks every cathode on the silkscreen.

## Assembly notes

* **SW2, in the middle, is IR 3, and SW3, on the right, is IR 2**, as on the A320 (IR 1, IR 3, IR 2 from left to right) and in the `.mfmc`. The v1 footprints were labelled the other way round; the copper decides.
* Rotary positions: 1 OFF, 2 NAV, 3 ATT; common on pin 9, to GND.
* J3 is the ADIRS OLED socket, where it was on v1: 1 GND, 2 +3V3, 3 SCL, 4 SDA, on PCA9548A channel 2.
* GND CTL is on this board because that is where it sits under the printed panel.
* There is no 3D model of the ADR rotary yet, so the render shows only its pads.
* Every part has its value, polarity or pin 1 on the silkscreen of its own side. Each part's description in the schematic names its v1 reference.
