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
| Data | One shrouded, keyed IDC header per section board, ribbon straight through, pin 1 to pin 1. SMD header on the section boards |
| Backlight | A 2-pin JST PH (SMD) per section board, AWG wire to a 2-pole screw terminal on the mainboard, dimmed there |
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
* **Seams:** the v1 board has them drawn on `User.Drawings`: x = 154 and
  x = 181 / 223 mm, y = 60, 102 and 144 mm. Each section board's inner
  edges are those lines moved 1.5 mm inwards.
* **Outline:** each section board keeps today's outer edge where it has
  one, since the current board already fits the case there. The new inner
  edges are cut along the seams between panels, on the current layout.
  Today's board is continuous there, so nothing passes through the PCB
  plane at a seam. The cut only has to stay clear of parts, tracks and
  holes.
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
* The four TLC5927 (today spread over several panels; the schematic calls them DM13A)

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
  test all 44 LEDs are lit, about 0.85 A at 19 mA each. Added to the logic,
  that is past the 0.8 A limit and a USB port's 500 mA. The
  anodes therefore take their own rail, `+5V_LED`, straight from the
  MIC29302. On USB alone the annunciators stay dark, like the backlight,
  and the logic keeps working.
* **Two fuses, one per branch**, as on the FCU PSU: the backlight branch
  (~0.7 A, measured) and the regulator branch (~1 A in a light test). A
  fault on one does not take down the other.
* The MIC29302 dissipates well under 1 W in normal use, with a handful of
  annunciators lit. In a light test it is ~3 W, about 3.2 V across it at
  ~1 A. The light test lasts seconds, but the copper under the tab must be
  sized for it, or the anode rail given its own regulator.

### Hot-plug lock-up

Found on the current board, September 2026, first with 680 Ω on every
`R-EXT` and the original, faulty U8 fitted. **Re-checked with 1 kΩ and a new
U8: the result is the same.** Pushing the barrel into a live adapter still
leaves the annunciators dead. With the barrel already in and the adapter
plugged into the mains, everything works, including after the case was
closed and the panel fully reassembled. It is a power-up problem, not a
faulty chip.

| How the 9 V arrives | Annunciators |
|---|---|
| Barrel already in the board, adapter then plugged into the mains | work, every time |
| Barrel pushed into an adapter that is already live | dead, about 19 times in 20 |
| Bench supply | work |

Two different 9 V / 3 A adapters behave the same way. The OLEDs, the
switches and the backlight work in every case. Only the TLC5927 outputs stay
off, while data still passes through the chips.

Pushing the barrel into a live supply brings the 9 V in all at once, with
the jack's contacts bouncing and the input capacitors being charged in one
step. The ATmega has a reset and the firmware re-initialises the OLEDs.
The TLC5927 has no reset pin, so a bad start is kept until its supply goes
away. Why it did not show with the old 56 Ω `R-EXT` is not known. No
oscilloscope capture has been taken.

**On the current board:** leave the barrel plugged in and switch the
adapter at the mains.

**On the new mainboard:**

* **A robust 9 V input:** a bulk electrolytic across the input to damp
  the hot-plug ringing, and a TVS for the spikes. The panel jack, the wires
  and the screw terminal make the input path longer than today's, so the
  problem would get worse, not go away.
* **A firmware-controlled power-up of the TLC5927 — required.** The four
  chips' VDD goes through a P-MOSFET high-side switch, the same topology as
  the anode switch: gate pulled up to +5V (off by default) and pulled down
  by an N-MOSFET (BS170) from **D46**.
  * The chips stay unpowered until the firmware turns them on. The ATmega
    starts, the supply has settled, and only then do the TLC5927s get
    power. They start clean whatever the 9 V did on the way in.
  * `SF_OVHD` does it in `attach()`: drive D46 low, wait a few tens of
    milliseconds, drive it high. The same sequence can be exposed as a
    message, so the Connector can reset the chain without a power cycle.
  * **Watch for back-powering.** While the chips are unpowered, CLK, LE
    and SDI must not sit high. A CMOS input driven high with VDD at 0 V
    feeds the chip through its protection diodes, half-powers it and
    defeats the reset. MobiFlight's output shifter leaves LE high after
    every update, so the firmware pulls CLK, LE and SDI low before it cuts
    VDD. A 1 kΩ series resistor on each of the three lines, at the chips'
    end, limits the injected current if that ever fails. It also damps the
    long clock and latch traces.
  * This firmware is the only one that can bring the annunciators up. A
    board flashed with stock MobiFlight firmware would leave them dark,
    which is acceptable for this panel.

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

**The four drivers are TI TLC5927, not DM13A.** They were bought from
Digi-Key, and the TLC5927 has the DM13A's pinout, so the KiCad symbol and
the schematic say DM13A. Everything below comes from the TLC5927 datasheet
(SLVS677C) and from measurements on the board.

* **Output current**, with the configuration the chip powers up in:
  I_OUT = 1.25 V / R_ext × 15. That is about 26 mA at 720 Ω and 52 mA at
  360 Ω. The output range is 10–120 mA, and 120 mA is the absolute maximum.
* **The trimmers go. Each TLC5927 gets a fixed 1 kΩ 1206 on `R-EXT`**, all
  four the same, for ~19 mA per LED (1.25 V / 1 kΩ × 15). The Korry LEDs
  are plain rectangular LEDs, and the rule is to keep them at or below
  25 mA. 1 kΩ is the value fitted on the current board, and all four chips
  pass repeated full MobiFlight test cycles with it. 680 Ω (~28 mA) looked
  as good but sits above that limit.
* **Never below ~160 Ω, never above ~1.9 kΩ.** At power-up the chip runs
  with CM = 1, which is specified for 10–120 mA. That puts `R-EXT` between
  ~160 Ω (120 mA, the absolute maximum) and ~1.9 kΩ (10 mA). The 2–4.7 kΩ
  values tried along the way were below the chip's range, and that is why
  they were dim.
* **Measured against the formula.** The LED current was read as the step in
  the 9 V supply current when one LED is switched on:

  | `R-EXT` at pin 23 | TLC5927 formula | Measured | Light |
  |---|---|---|---|
  | 56–156 Ω (the original resistor and trimmer) | 120–335 mA asked, **held at 120 mA** | ~70–85 mA | far too bright |
  | 300 Ω | 63 mA | 39 mA | too bright |
  | 680 Ω | 28 mA | 17–18 mA | right, on every colour |
  | **1 kΩ** | **19 mA** | — | **fitted, all four chips** |
  | 2.1 kΩ | 9 mA | ~7 mA | dim |
  | 3.6 kΩ | 5 mA | a few mA | very dim |
  | 4.7 kΩ | 4.0 mA | ~4 mA | dim |

  Where the currents are low, measurement and formula agree. Where they are
  higher, the measurement reads low. The likely reason is that USB feeds
  part of the +5V through D162, so the bench supply does not see all of
  it. That has not been checked at these currents.
* **Why the trimmer never worked:** over its whole travel, 56–156 Ω asks
  for more than 120 mA. The chip sits at its own limit, so turning the
  trimmer changes nothing, and the LEDs ran far above their rating.
* **The 100 nF on `R-EXT`:** the TLC5927 datasheet does not call for one.
  Footprint kept, not fitted.
* **`OE` stays tied to GND, hard.** On the TLC5927, `OE` is also the mode
  pin. A one-clock-wide pulse on it, sampled by `CLK`, switches the chip
  into Special mode. There `LE` writes the configuration latch instead of
  the outputs. So **no PWM on `OE`**: MobiFlight's PWM is not synchronised
  with the shift-register clock, and could produce exactly that pulse. The
  dimming inverter on `~EN` that this plan used to have is dropped.
* **Dimming on the anode rail instead.** A P-MOSFET high-side switch on
  `+5V_LED`, its gate pulled up to `+5V_LED` (off by default) and pulled
  down by an N-MOSFET (BS170) from **D45**, a PWM pin free today.
  * D45 high → both MOSFETs on → anodes powered → annunciators lit.
  * D45 low or not driven → anodes off → annunciators dark. So 255 means
    bright in the Connector, and at power-up, reset or with the Connector
    closed they stay dark.
  * Brightness = the `R-EXT` current × the PWM duty. The chips never see
    their `OE` move.
  * The P-MOSFET must carry the light test: 44 × 19 mA ≈ 0.85 A.
* **A failed driver: U8.** With 1 kΩ on all four chips, U6, U7 and U5
  worked, and U8, the first chip of `ANN_UPPER`, did not:
  * every LED on it lit once and then the chip stopped updating its
    outputs until a power cycle, while still passing data on to U5;
  * OUT8–OUT11, the FUEL fault LEDs, never lit at all, even as the first
    command after power-up.

  Everything around it measured normal: GND 0 Ω, VDD 5.0 V steady, `R-EXT`
  1.23 V, `OE` 0 V. Every output sat at 3.5 V at rest, the same on the FUEL
  LEDs as on the working GPWS and BATT ones, so the LEDs were sound.
  **Replacing U8 with a new TLC5927 fixed it**: two full test cycles, every
  LED on every pass. The most likely cause is the original 56–156 Ω
  `R-EXT`, which held its outputs at the 120 mA maximum, plus the >100 mA
  tests on it. That cannot be proven, but nothing else set U8 apart.
* **Short detection is ruled out.** The TLC5927 flags an LED as shorted
  when its output sits above 2.4–3.1 V with the channel on. At the
  currents used here, amber and green LEDs leave about 3 V on their
  outputs, inside that window, and that looked like a cause for a while.
  But U5 drives several amber LEDs in the same conditions and never locked,
  and a new U8 fixed the problem without any change to the voltages. Nothing
  in the new design needs to keep the outputs below the window.

### Backlight dimming

* Low-side N-MOSFET (**IRLIZ44N**, as on the FCU dimmer: logic level, isolated
  TO-220 tab), gate from **D44** through 100 Ω, 10k pull-down. All eight
  section returns join at its drain.
* D44 low or not driven → backlight off. PWM from a MobiFlight output on
  D44, 0–255, meant to follow the INTEG LT knob.
* D46 switches the TLC5927 supply (see *Hot-plug lock-up*).

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

### Schematic status

The mainboard schematic is drawn: `Mainboard/OVHD_Mainboard`, seven sheets
(Power, USB and supply, MCU, I2C, Annunciators, Connectors), 126 parts,
labels on pins rather than wires. ERC: no errors, no warnings. The exported
netlist was checked against this document: every `.mfmc` button runs from
its v1 ATmega pin to exactly one ribbon pin, every annunciator from its
TLC5927 output to exactly one ribbon pin, and all 136 IDC pins match the
pinout tables below.

Parts chosen while drawing, open to review:

| Where | Part |
|---|---|
| 9 V input | 1N5822 reverse diode (as the FCU PSU), SMBJ12CA TVS, 470 µF 25 V electrolytic |
| Fuses | MF-RHT200 on both branches (backlight, regulator) |
| 5 V | MIC29302WU, 3.6k / 1.2k as v1 |
| Supply selector, USB, ISP, JP1, 3.3 V | the FCU mainboard's circuit, pin for pin |
| Anode and TLC5927 supply switches | AO3401A (SOT-23) driven by BS170 (TO-92), 10k / 100R / 100k |
| Backlight dimmer | IRLIZ44N, 100R gate, 10k pull-down |
| Annunciator drivers | TLC5927IDWR, DW24-M footprint, R-EXT 1k, 1k series on CLK/LE/SDI |
| Ribbons | shrouded IDC headers 2x5 to 2x13; backlight on 5.08 mm 2-pole terminals |

## Section boards

Each one carries its switches, its annunciator LEDs and its backlight LEDs
with their resistors. It has no ICs, and the annunciator LEDs have no
resistors, because the TLC5927 is a current sink. Anodes go to `+5V_LED` from
the ribbon, cathodes back down the ribbon to their TLC5927 output.

### Layout rules and status

* **Parts:** everything from v1 stays at its v1 coordinates, and the build
  script checks every pad against the v1 board. New parts (the IDC header,
  the JST PH and any new resistor) go on the back.
* **Sheet:** each board is moved by a whole number of millimetres onto the
  middle of its A4 sheet, and its grid origin is set to where v1's (0, 0)
  now lies. With coordinates relative to the grid origin, KiCad shows the v1
  positions. The offset is in the title block (APU: v2 = v1 + (−54, −80) mm).
* **Connectors:** SMD, on the back, on the edge nearest the mainboard:
  `IDC-Header_2xNN_P2.54mm_Vertical_SMD` and
  `JST_PH_B2B-PH-SM4-TB_1x02-1MP_P2.00mm_Vertical` (pin 1 `+9V_BL`, pin 2
  `BL_RET`). Keep them at least ~4 mm from a mounting hole, where the case
  insert bears on the back.
* **Silkscreen:** everything needed to solder the board without the
  schematic goes on the silkscreen of the side the part is on:
  * `K` at every backlight LED cathode, and the annunciator colour and
    function (`FAULT amber`, `ON blue`, `AVAIL green`);
  * resistor values;
  * pin 1 of the Korry and the IDC;
  * `+` and `-` at the JST;
  * the mainboard connector each cable goes to.
* **Libraries:** `Libraries/OVHD.pretty` and `Libraries/OVHD.kicad_sym`
  hold the custom parts every board shares. The footprints are extracted
  from the v1 board, because three of their four source libraries are no
  longer installed: Korry G-Switch PS-7054DVB-6PN, the rectangular
  annunciator LED, the ADR rotary and the toggle. The symbols (Korry,
  toggle) come from the v1 schematic, with every pin made passive.
* **Rules:** 2 layers, 1.6 mm. Tracks are 0.3 mm, or 0.6 mm for `GND`,
  `+5V_LED`, `+9V_BL` and `BL_RET`. Clearance is 0.25 mm and vias
  0.8/0.4 mm. Routed with Freerouting 2.4.1 (Java 25 or later), then
  checked with KiCad DRC.

| Board | Status |
|---|---|
| APU | `Section_APU/`, the trial board. Schematic 24 parts, ERC 0. PCB 39 × 79.5 mm, routed, DRC 0, 0 unconnected, 0 schematic-parity issues. J1 (2x5) and J2 on the left edge, towards the mainboard. D148 is a single-LED string with R3 470 Ω on the back |
| GPWS | `Section_GPWS/`. Schematic 38 parts, ERC 0. PCB 159.5 × 39 mm, routed, DRC 0, 0 unconnected, 0 schematic-parity issues. 4 Korry, 6 annunciators, 4 strings of two and D139 on its own with R5 470 Ω (its v1 partner D140 and resistor R75 go to EXT LT). J1 (2x7) and J2 along the top edge on the right: the bottom band, nearer the mainboard, is taken by the Korry pins, the annunciator LEDs and the frame holes, and a 2x7 shroud does not fit there |
| ADIRS, FUEL, ELEC, AIR COND, EXT LT, SIGNS | next |

### Backlight strings

Today there are 61 strings, each of 2 LEDs and one 150 Ω resistor on 9 V.
Measured on the current board, everything but the annunciators draws
0.76–0.81 A from the 9 V supply. The logic accounts for roughly 0.1 A of
that (estimated, not measured), so the backlight takes about **0.7 A, or
~11 mA per string**. It will vary with the supply and the temperature.

The split changes three things:

* **Three strings span two panels** and have to be rewired:
  D139 (GPWS) + D140 (EXT LT), D141 (SIGNS) + D142 (AIR COND),
  D148 (APU) + D149 (SIGNS). D141 and D149 then pair up on SIGNS.
* That leaves **GPWS, AIR COND, EXT LT and APU with an odd LED count**. Each
  gets one single-LED string on 9 V. For the same current as a pair, and so
  the same brightness, R = V / (2·I) + 75 Ω, which follows from
  V = 2·V_F + 150 Ω·I for the pair. With the rail at ~8.6 V after the reverse
  diode and ~11 mA per string, that is ~466 Ω: **470 Ω**. At 10 mA or 13 mA
  it would be 505 or 406 Ω, so the brightness stays within about 15%. To
  confirm it, measure the voltage across one 150 Ω with the backlight on:
  divided by 150, it gives the exact string current.
* **Four strings have both LEDs on one panel but their resistor across the
  seam**: R57, R61, R64 and R66. The resistor moves next to its LEDs.

| Board | Strings | Backlight current ≈ |
|---|---|---|
| ADIRS | 9 | 100 mA |
| FUEL | 7 | 80 mA |
| ELEC | 5 | 55 mA |
| GPWS | 4 + 1 single | 55 mA |
| AIR COND | 6 + 1 single | 80 mA |
| EXT LT | 15 + 1 single | 180 mA |
| APU | 2 + 1 single | 35 mA |
| SIGNS | 11 | 120 mA |
| **Total** | **63** | **~0.7 A** |

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

All 49 inputs and all 44 annunciators were checked by **following the copper**
of the as-built board: the project as sent to JLCPCB on 3 March 2025,
restored in `Kicad Files/`. Net names were not trusted, because schematic and
copper disagree in places. Every button in the `.mfmc` reaches exactly one
switch contact, and every annunciator bit reaches exactly one LED.

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
| 5 | SEAT BELTS — D19 (contact 1) | | 6 | NO SMOKING — D5 (contact 1) |
| 7 | EMER EXIT LT ON — D2 | | 8 | EMER EXIT LT OFF — D3 |
| 9 | GND | | 10 | WING ON — L18 |
| 11 | WING FAULT — U21 | | 12 | ENG 1 ON — L19 |
| 13 | ENG 1 FAULT — U20 | | 14 | ENG 2 ON — L20 |
| 15 | ENG 2 FAULT — U22 | | 16 | +5V_LED |

SEAT BELTS and NO SMOKING are on-off toggles, so they need one contact
each. The v1 copper wires both contacts of each: SEAT BELTS contact 1 to D19
and contact 3 to D18, NO SMOKING contact 1 to D38 and contact 3 to D5. The
v1 schematic calls the unused ones `…_OFF`, but they are not buttons. v2
keeps only contact 1 of each. EMER EXIT LT, NAV & LOGO, NOSE and
both LDG lights really have three positions, and keep both contacts.

**Lever up closes contact 1, the lower pin, on every toggle.** Every
single-contact toggle is read on contact 1. On v1 NO SMOKING alone was read
on contact 3 (D5), and the MobiFlight row was inverted to make up for it.
On v2 D5 moves to contact 1. **When moving to v2, remove the inversion on
the NO SMOKING row.** The three-position toggles keep both contacts on the
same pins as v1, contact 1 up and contact 3 down.

**Toggle parts in the BOM.** The v1 schematic gives all eleven toggles the
E-Switch MPN 100SP1T1B4M2QE, which is not what is fitted. The v2 BOM lists
them as generic three-terminal PCB-pin lever toggles, from AliExpress and
fitted by hand, not orderable from JLCPCB:

* ON-OFF-ON (5): LDG L, LDG R, NOSE, NAV & LOGO, EMER EXIT LT
* ON-OFF (6): WING, BEACON, STROBE, RWY TURN, SEAT BELTS, NO SMOKING

The v1 footprint stays, since they fit it.

### Free resources

* Shift register bits: `ANN_LOWER` 22, 23, 26–31; `ANN_UPPER` 0–3, 6, 7, 15,
  17, 25–28. 20 outputs in all.
* PCA9548A channels 3–7.
* ATmega pins: whatever the `.mfmc` does not use. D44, D45 and D46 are
  taken by the two dimmers and the TLC5927 supply switch.

## Firmware and MobiFlight

* **The pin map does not change,** and the two dimmers are plain
  MobiFlight outputs, which are core.
* **`SF_OVHD` gains one job: powering the TLC5927 up.** In `attach()` it pulls
  the two chains' CLK, LE and SDI (D22–D27) low, drives D46 low, waits a
  few tens of milliseconds, then drives D46 high. D46 must not appear in
  the `.mfmc`: the custom device owns it. A message that repeats the
  sequence on demand is optional.
* **The `.mfmc` gains two outputs**, D44 (backlight) and D45 (annunciators),
  both PWM.
* **The MobiFlight project gains two output rows** for them, typically
  INTEG LT for D44 and ANN LT BRT/DIM for D45. The OLEDs already have
  message 4. None of the existing 96 rows changes.

## Before drawing

1. ~~R-EXT value~~ — 1 kΩ on all four TLC5927, ~19 mA, within the 25 mA
   limit set for the Korry LEDs.
2. ~~Hot-plug re-check with the new U8~~ — still dead on a hot plug, so
   the TLC5927 power-up switch is in the design.
3. ~~Backlight LED V_F~~ — not needed: the single-LED strings get 470 Ω,
   worked out from the pair. Optional check: the voltage across one 150 Ω.
4. ~~Inner panel outlines~~ — not needed. The outer edges are today's PCB
   outline, which already fits the case. The inner cuts follow the panel
   seams on the current layout, where nothing crosses the PCB plane, and
   stay clear of parts, tracks and holes.
5. ~~Mainboard outline~~ — proposed in KiCad. The case is then adapted to it
   in the 3D model.
6. ~~TLC5927 reset~~ — decided: VDD switch from D46, driven by `SF_OVHD`.
   If an oscilloscope is at hand, a capture of +5V during a hot plug would
   still be worth having.

## Lessons carried into the next version

* **Draw the part that is fitted.** The schematic says DM13A, but the
  board carries TLC5927. They are pin-compatible, with different current
  formulas and a different `OE`. The new schematic uses the TLC5927 symbol,
  with the manufacturer part number in the BOM.
* **Size `R-EXT` from the TLC5927 formula**, 1.25 V / R × 15, and stay inside
  160 Ω–1.9 kΩ. The original 56 Ω + 0–100 Ω trimmer sat past the chip's
  limit over its whole travel.
* **`OE` stays hard-wired to GND.** It is the mode pin, so it gets no PWM,
  no pull-up and no test point that could be touched by accident.
* **The annunciator drivers need a clean power-up.** On a hot-plugged
  supply they can come up with frozen outputs. The new board powers them
  from the firmware, after the rest of the board is up.
* **Spares are at hand:** about twenty TLC5927 from the same Digi-Key order.
