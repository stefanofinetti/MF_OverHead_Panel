#pragma once

#include <Arduino.h>

// Supply of the four TLC5927 on the v2 mainboard. D46 (TLC_PWR_EN) drives a BS170
// that pulls the gate of the AO3401A high-side switch down: D46 high = chips
// powered. Undriven, R526 holds the BS170 off and the chips stay unpowered, so
// they only get power once the firmware has started and the 5 V has settled.
//
// The TLC5927 has no reset pin: a chip that starts badly keeps that state until
// its supply goes away. Switching the supply from here is the reset.
namespace TLCSupply
{
    // Switches the chips off now. update() switches them on again and rewrites
    // both annunciator chains, without blocking the loop.
    void powerCycle();

    // Called regularly; finishes a power cycle started by powerCycle().
    void update();
}
