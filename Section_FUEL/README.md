# FUEL section board (v2)

The v2 board under the FUEL panel. It carries the panel's switches, annunciator LEDs and backlight, at their v1 positions, and reaches the mainboard through one ribbon and one pair of backlight wires. There are no ICs on it. The design rules shared by the eight section boards are in [`docs/board-split.md`](../docs/board-split.md).

* **PCB:** 177.5 × 40 mm, 2 layers, 1.6 mm. Every v1 hole is kept.
* **Status:** routed with Freerouting. ERC, DRC and schematic parity are clean. Not yet ordered.

## Connectors

Both are new on v2 and sit on the **back**, so nothing new stands proud of the front.

| Ref | What | Goes to |
|---|---|---|
| J1 | Shrouded IDC header, 26 pins, SMD | J702 on the mainboard, straight ribbon, pin 1 to pin 1 |
| J2 | JST PH, 2 pins, SMD. Pin 1 `+9V_BL` (`+`), pin 2 the switched return (`−`) | screw terminal J602 on the mainboard |

J1 and J2 are on the back along the top edge, left of the middle M3 hole.

The signal on every ribbon pin is in the design document: [FUEL — 26 pins](../docs/board-split.md#fuel--26-pins).

## Switches

| Ref | Switch | Type | v1 ref |
|---|---|---|---|
| SW1 | FUEL LT TK PUMP 1 | Korry | SW17 |
| SW2 | FUEL LT TK PUMP 2 | Korry | SW18 |
| SW3 | FUEL L XFER | Korry | SW19 |
| SW4 | FUEL X FEED | Korry | SW20 |
| SW5 | FUEL R XFER | Korry | SW21 |
| SW6 | FUEL RT TK PUMP 1 | Korry | SW22 |
| SW7 | FUEL RT TK PUMP 2 | Korry | SW23 |

## Annunciators

Anodes on `+5V_LED` from the ribbon, cathodes back down the ribbon to their TLC5927 output. No resistors: the TLC5927 sets the current.

| Ref | Colour | Legend | Chain bit | Driver | v1 ref |
|---|---|---|---|---|---|
| D1 | amber | `LT_TK_PUMP_1_FAULT` | `ANN_UPPER` 12 | U503 (v1 U8) | D50 |
| D2 | white | `LT_TK_PUMP_1_OFF` | `ANN_LOWER` 5 | U501 (v1 U6) | D120 |
| D3 | amber | `LT_TK_PUMP_2_FAULT` | `ANN_UPPER` 11 | U503 (v1 U8) | D51 |
| D4 | white | `LT_TK_PUMP_2_OFF` | `ANN_LOWER` 6 | U501 (v1 U6) | D121 |
| D5 | amber | `L_XFER_FAULT` | `ANN_UPPER` 10 | U503 (v1 U8) | D52 |
| D6 | white | `L_XFER_OFF` | `ANN_LOWER` 7 | U501 (v1 U6) | D122 |
| D7 | green | `XFEED_OPEN` | `ANN_UPPER` 18 | U504 (v1 U5) | D2 |
| D8 | white | `XFEED_ON` | `ANN_LOWER` 13 | U501 (v1 U6) | D123 |
| D9 | amber | `R_XFER_FAULT` | `ANN_UPPER` 9 | U503 (v1 U8) | D75 |
| D10 | white | `R_XFER_OFF` | `ANN_LOWER` 11 | U501 (v1 U6) | D124 |
| D11 | amber | `RT_TK_PUMP_1_FAULT` | `ANN_UPPER` 8 | U503 (v1 U8) | D77 |
| D12 | white | `RT_TK_PUMP_1_OFF` | `ANN_LOWER` 10 | U501 (v1 U6) | D125 |
| D13 | amber | `RT_TK_PUMP_2_FAULT` | `ANN_UPPER` 16 | U504 (v1 U5) | D108 |
| D14 | white | `RT_TK_PUMP_2_OFF` | `ANN_LOWER` 9 | U501 (v1 U6) | D127 |

## Backlight

14 LEDs, 1206 (D15–D28), on 9 V, in 7 strings of two with 150 Ω (R1–R7). Each string runs at about 17 mA. `K` marks every cathode on the silkscreen.

## Assembly notes

* R2, R4 and R7 are new, 150 Ω on the back, beside their LEDs. On v1 the resistors of those three strings (R66, R64, R61) sat across the seam, on the ELEC side.
* The ribbon has two `+5V_LED` pins: 14 annunciators at ~19 mA are about 270 mA in a light test.
* SW3 and SW5 are labelled `L XFER` and `R XFER` on the board. They are CTR TK PUMP 1 and 2 in the `.mfmc` and in the MobiFlight project.
* The seam with ELEC is inset 1 mm instead of 1.5, because D102 and D104 of v1 sit 2 mm from it.
* Every part has its value, polarity or pin 1 on the silkscreen of its own side. Each part's description in the schematic names its v1 reference.
