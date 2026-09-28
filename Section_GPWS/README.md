# GPWS section board (v2)

The v2 board under the GPWS panel. It carries the panel's switches, annunciator LEDs and backlight, at their v1 positions, and reaches the mainboard through one ribbon and one pair of backlight wires. There are no ICs on it. The design rules shared by the eight section boards are in [`docs/board-split.md`](../docs/board-split.md).

* **PCB:** 159.5 × 39 mm, 2 layers, 1.6 mm. Every v1 hole is kept.
* **Status:** routed with Freerouting. ERC, DRC and schematic parity are clean. Not yet ordered.

## Connectors

Both are new on v2 and sit on the **back**, so nothing new stands proud of the front.

| Ref | What | Goes to |
|---|---|---|
| J1 | Shrouded IDC header, 14 pins, SMD | J704 on the mainboard, straight ribbon, pin 1 to pin 1 |
| J2 | JST PH, 2 pins, SMD. Pin 1 `+9V_BL` (`+`), pin 2 the switched return (`−`) | screw terminal J604 on the mainboard |

J1 and J2 are along the top edge on the right. The bottom band, nearer the mainboard, is taken by the Korry pins, the annunciator LEDs and the frame holes, and a 2x7 shroud does not fit there.

The signal on every ribbon pin is in the design document: [GPWS — 14 pins](../docs/board-split.md#gpws--14-pins).

## Switches

| Ref | Switch | Type | v1 ref |
|---|---|---|---|
| SW1 | GPWS TERR | Korry | SW6 |
| SW2 | GPWS SYS | Korry | SW7 |
| SW3 | GPWS G/S MODE | Korry | SW8 |
| SW4 | GPWS LDG FLAPS 3 | Korry | SW9 |

## Annunciators

Anodes on `+5V_LED` from the ribbon, cathodes back down the ribbon to their TLC5927 output. No resistors: the TLC5927 sets the current.

| Ref | Colour | Legend | Chain bit | Driver | v1 ref |
|---|---|---|---|---|---|
| D1 | amber | `GPWS_TERR_FAULT` | `ANN_UPPER` 4 | U503 (v1 U8) | D188 |
| D2 | white | `GPWS_TERR_OFF` | `ANN_LOWER` 0 | U501 (v1 U6) | D76 |
| D3 | amber | `GPWS_SYS_FAULT` | `ANN_UPPER` 5 | U503 (v1 U8) | D190 |
| D4 | white | `GPWS_SYS_OFF` | `ANN_LOWER` 1 | U501 (v1 U6) | D78 |
| D5 | white | `GS_OFF` | `ANN_LOWER` 2 | U501 (v1 U6) | D79 |
| D6 | white | `FLAPS3_OFF` | `ANN_LOWER` 3 | U501 (v1 U6) | D80 |

## Backlight

9 LEDs, 1206 (D7–D15), on 9 V: 4 strings of two with 150 Ω, and one single LED with R5, 330 Ω. Each string runs at about 17 mA. `K` marks every cathode on the silkscreen.

## Assembly notes

* D15 is a single-LED string: on v1 it was paired with D140, now on EXT LT. R5, 330 Ω on the back, gives it the same 17 mA as a pair on 150 Ω.
* Every part has its value, polarity or pin 1 on the silkscreen of its own side. Each part's description in the schematic names its v1 reference.
