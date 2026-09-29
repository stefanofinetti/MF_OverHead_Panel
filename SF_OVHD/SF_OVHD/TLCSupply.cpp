#include "TLCSupply.h"
#include "OutputShifter.h"

// the core's power-saving state (mobiflight.cpp), so the chains are rewritten
// dark if the Connector has the board in power saving
extern bool powerSavingMode;

namespace TLCSupply
{
    // D46, TLC_PWR_EN on the schematic
    const uint8_t SUPPLY_PIN = 46;

    // SDI, LE and CLK of both chains, as the .mfmc sets them up: ANN_LOWER data 22,
    // latch 23, clock 24; ANN_UPPER data 25, clock 26, latch 27. They must be low
    // while the chips have no supply: a CMOS input held high feeds the chip
    // through its protection diode, half-powers it and defeats the reset. The
    // core's output shifter leaves LE high after every update.
    const uint8_t SIGNAL_PINS[] = {22, 23, 24, 25, 26, 27};

    // +5V_TLC has no bleed resistor. Its 10.4 uF (C501-C505) discharge only through
    // the chips' own supply current, and the datasheet gives no reset threshold,
    // so the off time is generous.
    const uint16_t OFF_MS = 300;

    // margin for the supply to rise through the AO3401A before the chains are
    // written again
    const uint16_t SETTLE_MS = 20;

    enum State : uint8_t { IDLE, OFF, SETTLING };

    State    state = IDLE;
    uint32_t since = 0;

    static void signalsLow()
    {
        for (uint8_t i = 0; i < sizeof(SIGNAL_PINS); i++) {
            pinMode(SIGNAL_PINS[i], OUTPUT);
            digitalWrite(SIGNAL_PINS[i], LOW);
        }
    }

    void powerCycle()
    {
        signalsLow();
        pinMode(SUPPLY_PIN, OUTPUT);
        digitalWrite(SUPPLY_PIN, LOW);
        state = OFF;
        since = millis();
    }

    void update()
    {
        switch (state) {
        case OFF:
            // A command for the annunciators can arrive meanwhile, and the core then
            // leaves LE high: pull the lines low again on every pass.
            signalsLow();
            if (millis() - since >= OFF_MS) {
                digitalWrite(SUPPLY_PIN, HIGH);
                state = SETTLING;
                since = millis();
            }
            break;
        case SETTLING:
            if (millis() - since >= SETTLE_MS) {
                // The chips come up with their latches in an unknown state. Write
                // both chains again: what the Connector last set, or all dark in
                // power saving. Anything that arrived while they were off is in it.
                OutputShifter::PowerSave(powerSavingMode);
                state = IDLE;
            }
            break;
        case IDLE:
            break;
        }
    }
}
