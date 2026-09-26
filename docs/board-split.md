# Splitting the mainboard — architecture

Status: **design on paper**. Nothing in `Kicad Files/` has been changed. This
is the plan to review before a new KiCad project is started.

## Why

The overhead runs on one four-layer board, 313 × 206 mm. It works, but it
costs well over €100 at JLCPCB once shipping and customs are added, and any
mistake means reordering the whole thing.

The panel is already divided into eight printed sections, each screwed down
on its own. The board does not need to be one piece: it can follow the
panels.

## The decisions

| Topic | Decision |
|---|---|
| Split | One mainboard plus **eight section boards, one per printed panel** |
| Printed parts | **Unchanged.** Panels, frames and Korry cases are not reprinted; only the case changes, to take the mainboard |
| Section boards | Cut out of the current layout: outline, **every hole**, every switch and LED at the same coordinates |
| Thickness | 1.6 mm, JLCPCB's standard, as today. The board is the spacer between panel and case |
| Layers | 2 on every board |
| Data | One shrouded, keyed IDC header per section board, ribbon straight through, pin 1 to pin 1 |
| Backlight | A 2-pole screw terminal per section board, AWG wire, dimmed on the mainboard |
| Connectors | On the **back** of the section boards only. Nothing new may stand proud of the front |
| Mainboard | On the floor of the lower-left case part, towards the centre of the case |
| USB | A ready-made panel-mount USB-B extension at the case wall; USB-B stays on the mainboard |
| Power in | Panel-mount 5.5 × 2.1 jack at the case wall, two AWG wires to a screw terminal |
| Pin map | **Every switch on the same ATmega pin, every annunciator on the same bit of the same chain.** Firmware, `.mfmc` and MobiFlight project stay valid |

## Mechanics

Today each printed panel is screwed through the big board into brass inserts
in the case. After the split, each panel clamps its own section board with
the same screws in the same inserts.

* **Holes:** every hole of today's board is kept on the section board it
  falls in, at the same position, whether or not its purpose is known.
* **Outline:** each section board must sit inside its panel's recess with
  the same clearance the current board has at its outer edge, or the panel
  will not close. The outer edges exist today. The inner edges, where two
  panels meet, are new and have to be taken from the panel recesses in
  SketchUp.
* **Case:** it is open inside, built for one board, and its parts are joined
  at floor level, so ribbons and wires pass under the section boards with
  nothing in the way. The only change is brass inserts on the floor of the
  lower-left part, for the mainboard.
* **Connector placement:** on each section board, the IDC header and the
  terminal go on the edge nearest the mainboard.

### Printed panels and section boards

Panel sizes are from the STL files. Contents are from the current board.

| Board | Panel | Switches | Annunciators | Backlight LEDs | Displays |
|---|---|---|---|---|---|
| ADIRS | 133 × 82 | 3 × ADR rotary, GND CTL | 1 | 18 | ADIRS OLED |
| FUEL | 178 × 40 | 7 Korry | 14 | 14 | — |
| ELEC | 178 × 40 | BATT 1, BATT 2, CREW SUPPLY | 5 | 10 | BATT 1 and BATT 2 OLED |
| GPWS | 160 × 40 | 4 Korry | 6 | 9 | — |
| AIR COND | 151 × 40 | APU BLEED, PACK 1, PACK 2, EXT PWR | 8 | 13 | — |
| EXT LT | 160 × 80 | 8 toggles | — | 31 | — |
| APU | 40 × 80 | MASTER, START | 4 | 5 | — |
| SIGNS | 109 × 80 | 3 ANTI ICE Korry, 3 toggles | 6 | 22 | — |
| **Total** | | **49 inputs** | **44** | **122** | 3 |

EXT PWR stays on AIR COND and GND CTL stays on ADIRS, because that is where
they are under the printed panels. ANTI ICE and SIGNS are one printed panel,
`OVHD_PANEL_SIGNS`. `ANTI-ICE-FRAME` is only the frame over it.

### Estimated ribbon lengths

With the mainboard in the lower-left part and each header on the nearest
edge, including the ~25 mm drop to the floor and some slack:

| Board | Ribbon |
|---|---|
| EXT LT | ~10 cm |
| GPWS, APU, AIR COND | ~10 cm |
| SIGNS, ADIRS, ELEC | ~12 cm |
| FUEL | ~15 cm |

None of these signals is fast: button levels, DC sinks switched only when an
annunciator changes, and I2C at 400 kHz. A ribbon adds roughly 50 pF/m per
conductor against the 400 pF I2C allows, so anything up to 40 cm is fine.

## The mainboard

About **150 × 90 mm**, 2 layers, on the floor of the lower-left case part.

### What moves onto it

Everything on today's board that belongs to no panel:

* ATmega2560 (today under GPWS)
* CH340G with its 12 MHz crystal, the 16 MHz crystal, USB-B
* J4, the ISP header, and **JP1, unchanged**. Its middle pin is the MCU
  rail. 1–2 joins it to the board's +5V; 2–3 joins it to J4's VCC, so an
  ISP programmer powers the MCU alone while the bootloader is burned.
* MIC29302, AMS1117
* PCA9548A with the three-way address DIP switch (today under ADIRS)
* The four DM13A (today spread over several panels)

### Power

```
 panel jack 9 V ──► screw terminal ──► reverse protection ──┬──► fuse ──► backlight +9V ──► 8 section terminals
                                                             │                 (return via Q1, see Dimming)
                                                             └──► fuse ──► MIC29302 ──► +5V_EXT ──┬──► +5V_LED (annunciator anodes)
                                                                                                 │
 USB VBUS (via extension) ─────────────────────────────────────────────────────► TPS2115A ◄───────┘
                                                                                     │
                                                                               +5V (logic) ──► JP1 ──► MCU rail
                                                                                     │
                                                                                     └──► AMS1117 ──► +3V3 (PCA9548A, OLEDs)
```

* **TPS2115A** picks the external 5 V whenever the 9 V is present and falls
  back to USB VBUS otherwise, with no path back into the USB port. The
  circuit is copied from the FCU mainboard (`U8`: D0 to ground, D1 from a
  100k/100k divider on the external rail, `ILIM` set to about 0.8 A). It
  replaces D162, today's series diode on VBUS.
* **The annunciator anodes do not go through the TPS2115A.** In a light
  test all 44 LEDs are lit, about 660 mA at 15 mA each. Added to the logic,
  that sits right on the 0.8 A limit and is beyond a USB port's 500 mA. The
  anodes therefore take their own rail, `+5V_LED`, straight from the
  MIC29302. On USB alone the annunciators stay dark, like the backlight,
  and the logic keeps working.
* **Two fuses, one per branch**, as on the FCU PSU: the backlight branch
  (~1.3 A) and the regulator branch (~0.8 A in a light test). A fault on
  one does not take down the other.
* The MIC29302 dissipates about 1.2 W in normal use and up to ~3 W in a light
  test (4 V across it at ~0.8 A). It keeps a copper area under its tab as
  today.

### I2C

* **Upstream pull-ups on +5V.** Today R7 and R8 pull SDA and SCL to 3.3 V,
  below the 3.5 V an ATmega at 5 V needs to read a high. It works, but out of
  spec. The PCA9548A is meant for exactly this translation: upstream
  pull-ups to 5 V, downstream to 3.3 V, the chip itself on 3.3 V. Check the
  values against the datasheet's application section when drawing.
* Downstream pull-ups, 4.7k to +3V3 per channel, stay on the mainboard.
* Channels 0, 1 and 2 as today (BATT 1, BATT 2, ADIRS). Channels 3–7 stay
  free.

### Annunciators: fixed current, dimmable

* **The trimmers go.** Each DM13A gets one fixed 1206 resistor on `REXT`.
  Today `REXT` sees 56 Ω plus a 0–100 Ω trimmer, i.e. 56–156 Ω. The DM13A
  datasheet sets 3–60 mA with `REXT` between about 22 kΩ and 0.5 kΩ, so the
  whole trimmer range asks for several times the chip's maximum. The chip
  sits at its limit and the trimmer changes nothing. That is why it never
  worked. The LEDs have probably been driven above their rating since.
* **Values: two for white and blue, one shared by the two green/amber
  chips**, since green and amber look alike at the same current. To be
  chosen by trying them on the current board: replace one 56 Ω (R49–R52)
  with a 2.2 kΩ, trimmer at zero, and look. Expect 2–4 kΩ for 10–15 mA.
  The curve is not a simple 1/R, so the eye decides.
* **The 100 nF on `REXT`:** footprint kept, not fitted, until the full
  datasheet confirms whether it belongs there.
* **Dimming through `~EN`.** Today the four `~EN` pins are tied to ground.
  They are joined and driven from **D45**, a PWM pin free today, through an
  inverter: an N-MOSFET (BS170, through-hole) with a 10k pull-up on `~EN`
  to +5V and a pull-down on its gate.
  * D45 high → MOSFET on → `~EN` low → annunciators lit
  * D45 low or not driven → pull-up → `~EN` high → annunciators dark
  * So 255 means bright in the Connector, and at power-up, reset or with the
    Connector closed the annunciators stay dark.
  * Brightness = the fixed `REXT` current × the PWM duty. Colour balance
    stays where the resistors put it.

### Backlight dimming

* Low-side N-MOSFET (**IRLIZ44N**, as on the FCU dimmer: logic level, isolated
  TO-220 tab), gate from **D44** through 100 Ω, 10k pull-down. All eight
  section returns join at its drain.
* D44 low or not driven → backlight off. PWM from a MobiFlight output on
  D44, 0–255, meant to follow the INTEG LT knob.
* D46 stays free.

### Connectors on the mainboard

* 8 shrouded IDC headers: 10, 14, 14, 16, 16, 20, 20 and 26 pins, 136 in all
* 9 screw terminals, 2-pole, 5.08 mm: one for the 9 V in, eight for the
  backlight (pin 1 `+9V_BL`, pin 2 `BL_RET`, the switched return)
* USB-B for the extension
* Optional, if space allows: an unfitted SIP pull-up network next to each
  header, to fit only if a long ribbon ever shows bouncing inputs

150 × 90 mm has about 480 mm of edge. The headers, terminals and USB-B take
about 340 mm of it. That fits, but tightly. If it does not fit with generous
spacing, group the backlight terminals in fours, or make the board bigger.

## Section boards

Each one carries its switches, its annunciator LEDs and its backlight LEDs
with their resistors. It has no ICs, and the annunciator LEDs have no
resistors, because the DM13A is a current sink. Anodes go to `+5V_LED` from
the ribbon, cathodes back down the ribbon to their DM13A output.

### Backlight strings

Today there are 61 strings, each of 2 LEDs and one 150 Ω resistor on 9 V. A
string of two white LEDs (V_F ≈ 3 V) draws about 20 mA, so the total is about
1.2 A.

The split changes three things:

* **Three strings span two panels** and have to be rewired:
  D139 (GPWS) + D140 (EXT LT), D141 (SIGNS) + D142 (AIR COND),
  D148 (APU) + D149 (SIGNS). D141 and D149 then pair up on SIGNS.
* That leaves **GPWS, AIR COND, EXT LT and APU with an odd LED count**. Each
  gets one single-LED string on 9 V, at about 300 Ω for the same ~20 mA
  (R = (9 − V_F) / I). Confirm against the LEDs' actual V_F. It dissipates
  about 0.12 W, fine for a 1206.
* **Four strings have both LEDs on one panel but their resistor across the
  seam**: R57, R61, R64 and R66. The resistor moves next to its LEDs.

| Board | Strings | Backlight current ≈ |
|---|---|---|
| ADIRS | 9 | 180 mA |
| FUEL | 7 | 140 mA |
| ELEC | 5 | 100 mA |
| GPWS | 4 + 1 single | 100 mA |
| AIR COND | 6 + 1 single | 140 mA |
| EXT LT | 15 + 1 single | 320 mA |
| APU | 2 + 1 single | 60 mA |
| SIGNS | 11 | 220 mA |
| **Total** | **63** | **~1.26 A** |

22–24 AWG is ample for the largest.

### Ribbon pinouts

Conventions:

* Pin numbers are IDC numbers. Consecutive numbers are neighbouring
  conductors in the ribbon.
* Pin 1 is always GND. Inputs sit together, then a GND, then the
  annunciators, then `+5V_LED`. Where there are displays, SDA and SCL are
  never neighbours: a GND lies between them.
* Inputs are listed in physical order, left to right, and switch commons
  return on GND. `Dnn` is the Arduino Mega pin, as in the `.mfmc`.
* `L`*n* is bit *n* of the `ANN_LOWER` chain (latch D23, clock D24, data
  D22): U6 carries bits 0–15 (white) and U7 bits 16–31 (blue). `U`*n* is bit
  *n* of `ANN_UPPER` (latch D27, clock D26, data D25): U8 carries bits 0–15
  (amber) and U5 bits 16–31 (green and amber).

All 49 inputs were checked against the current board, from the switch pad to
the ATmega pin, and agree with the `.mfmc`. The 44 annunciator bits were
checked against the DM13A outputs and agree with the MobiFlight project.

#### ADIRS — 20 pins

| Pin | Signal | | Pin | Signal |
|---|---|---|---|---|
| 1 | GND | | 2 | ADIR 1 OFF — D49 |
| 3 | ADIR 1 NAV — D14 | | 4 | ADIR 1 ATT — D15 |
| 5 | ADIR 3 OFF — D48 | | 6 | ADIR 3 NAV — D63 |
| 7 | ADIR 3 ATT — D62 | | 8 | ADIR 2 OFF — D47 |
| 9 | ADIR 2 NAV — D65 | | 10 | ADIR 2 ATT — D64 |
| 11 | GND CTL — D69 | | 12 | GND |
| 13 | GND CTL ON — L16 | | 14 | +5V_LED |
| 15 | GND | | 16 | +3V3 (OLED) |
| 17 | SCL2 | | 18 | GND |
| 19 | SDA2 | | 20 | GND |

The middle rotary (PCB label `ADR_2`) is ADIR 3 and the right one (`ADR_3`)
is ADIR 2. That is not a mistake: the A320 panel reads IR 1, IR 3, IR 2 from
left to right, and the `.mfmc` names follow the aircraft.

#### FUEL — 26 pins

| Pin | Signal | | Pin | Signal |
|---|---|---|---|---|
| 1 | GND | | 2 | LT TK PUMP 1 — D17 |
| 3 | LT TK PUMP 2 — D16 | | 4 | CTR TK PUMP 1 — D8 |
| 5 | X FEED — D4 | | 6 | CTR TK PUMP 2 — D9 |
| 7 | RT TK PUMP 1 — D6 | | 8 | RT TK PUMP 2 — D7 |
| 9 | GND | | 10 | LT 1 OFF — L5 |
| 11 | LT 1 FAULT — U12 | | 12 | LT 2 OFF — L6 |
| 13 | LT 2 FAULT — U11 | | 14 | CTR 1 OFF — L7 |
| 15 | CTR 1 FAULT — U10 | | 16 | X FEED ON — L13 |
| 17 | X FEED OPEN — U18 | | 18 | CTR 2 OFF — L11 |
| 19 | CTR 2 FAULT — U9 | | 20 | RT 1 OFF — L10 |
| 21 | RT 1 FAULT — U8 | | 22 | RT 2 OFF — L9 |
| 23 | RT 2 FAULT — U16 | | 24 | +5V_LED |
| 25 | +5V_LED | | 26 | GND |

Two anode pins: 14 LEDs are about 210 mA in a light test. The PCB labels the
centre-tank pumps `L_XFER` and `R_XFER`, and the `.mfmc` and the project
call them CTR TK PUMP 1 and 2. They are the same switches.

#### ELEC — 20 pins

| Pin | Signal | | Pin | Signal |
|---|---|---|---|---|
| 1 | GND | | 2 | CREW SUPPLY — D66 |
| 3 | BATT 1 — D67 | | 4 | BATT 2 — D68 |
| 5 | GND | | 6 | CREW SUPPLY OFF — L4 |
| 7 | BATT 1 OFF — L15 | | 8 | BATT 1 FAULT — U14 |
| 9 | BATT 2 OFF — L14 | | 10 | BATT 2 FAULT — U13 |
| 11 | +5V_LED | | 12 | GND |
| 13 | +3V3 (OLEDs) | | 14 | SCL0 (BATT 1) |
| 15 | GND | | 16 | SDA0 (BATT 1) |
| 17 | GND | | 18 | SCL1 (BATT 2) |
| 19 | GND | | 20 | SDA1 (BATT 2) |

#### GPWS — 14 pins

| Pin | Signal | | Pin | Signal |
|---|---|---|---|---|
| 1 | GND | | 2 | TERR — D57 |
| 3 | SYS — D58 | | 4 | G/S MODE — D59 |
| 5 | LDG FLAP 3 — D60 | | 6 | GND |
| 7 | TERR OFF — L0 | | 8 | TERR FAULT — U4 |
| 9 | SYS OFF — L1 | | 10 | SYS FAULT — U5 |
| 11 | G/S MODE OFF — L2 | | 12 | LDG FLAP 3 OFF — L3 |
| 13 | +5V_LED | | 14 | GND |

#### AIR COND — 16 pins

| Pin | Signal | | Pin | Signal |
|---|---|---|---|---|
| 1 | GND | | 2 | APU BLEED — D61 |
| 3 | PACK 1 — D41 | | 4 | PACK 2 — D40 |
| 5 | EXT PWR — D39 | | 6 | GND |
| 7 | APU BLEED ON — L25 | | 8 | APU BLEED FAULT — U29 |
| 9 | PACK 1 OFF — L8 | | 10 | PACK 1 FAULT — U30 |
| 11 | PACK 2 OFF — L12 | | 12 | PACK 2 FAULT — U31 |
| 13 | EXT PWR ON — L21 | | 14 | EXT PWR AVAIL — U19 |
| 15 | +5V_LED | | 16 | GND |

#### EXT LT — 14 pins

| Pin | Signal | | Pin | Signal |
|---|---|---|---|---|
| 1 | GND | | 2 | STROBE — D31 |
| 3 | BEACON — D32 | | 4 | WING — D33 |
| 5 | NAV & LOGO 1 — D34 | | 6 | NAV & LOGO OFF — D35 |
| 7 | GND | | 8 | RWY TURN OFF — D30 |
| 9 | LDG L ON — D10 | | 10 | LDG L RETRACT — D11 |
| 11 | LDG R ON — D12 | | 12 | LDG R RETRACT — D13 |
| 13 | NOSE T.O. — D37 | | 14 | NOSE OFF — D36 |

Top row first, then bottom row. No annunciators and no `+5V_LED`.

#### APU — 10 pins

| Pin | Signal | | Pin | Signal |
|---|---|---|---|---|
| 1 | GND | | 2 | MASTER SW — D28 |
| 3 | START — D29 | | 4 | GND |
| 5 | MASTER SW ON — L17 | | 6 | MASTER SW FAULT — U23 |
| 7 | START ON — L24 | | 8 | START AVAIL — U24 |
| 9 | +5V_LED | | 10 | GND |

#### SIGNS — 16 pins

| Pin | Signal | | Pin | Signal |
|---|---|---|---|---|
| 1 | GND | | 2 | ANTI ICE WING — D54 |
| 3 | ANTI ICE ENG 1 — D55 | | 4 | ANTI ICE ENG 2 — D56 |
| 5 | SEAT BELTS — D19 | | 6 | NO SMOKING — D5 |
| 7 | EMER EXIT LT ON — D2 | | 8 | EMER EXIT LT OFF — D3 |
| 9 | GND | | 10 | WING ON — L18 |
| 11 | WING FAULT — U21 | | 12 | ENG 1 ON — L19 |
| 13 | ENG 1 FAULT — U20 | | 14 | ENG 2 ON — L20 |
| 15 | ENG 2 FAULT — U22 | | 16 | +5V_LED |

### Free resources

* Shift register bits: `ANN_LOWER` 22, 23, 26–31; `ANN_UPPER` 0–3, 6, 7, 15,
  17, 25–28. 20 outputs in all.
* PCA9548A channels 3–7.
* ATmega pins: D46, plus whatever the `.mfmc` does not use. D44 and D45 are
  taken by the two dimmers.

## Firmware and MobiFlight

* **The firmware does not change.** The pin map is the same, and the two
  dimmers are plain MobiFlight outputs, which are core.
* **The `.mfmc` gains two outputs**, D44 (backlight) and D45 (annunciators),
  both PWM.
* **The MobiFlight project gains two output rows** for them, typically
  INTEG LT for D44 and ANN LT BRT/DIM for D45. The OLEDs already have
  message 4. None of the existing 96 rows changes.

## Before drawing

1. **REXT values.** Try 2.2 kΩ, then 3.3 kΩ, on the current board and pick
   one per colour.
2. **Backlight LED V_F.** Measure one, to set the single-string resistor.
3. **The `REXT` capacitor.** Read it in the full DM13A datasheet.
4. **Inner panel outlines.** Take them from SketchUp, with the same
   clearance as today's outer edge.
5. **Mainboard outline.** Measure the free floor of the lower-left case part.
