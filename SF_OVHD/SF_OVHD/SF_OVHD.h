#pragma once

#include "Arduino.h"
#include "OLEDInterface.h"

// The multiplexer's address comes from the Connector, not from here.
// Channel of the PCA9548A each display is wired to:
#define TCA9548A_CHANNEL_BATT1 0
#define TCA9548A_CHANNEL_BATT2 1
#define TCA9548A_CHANNEL_ADIRS 2


class SF_OVHD
{
public:
    SF_OVHD();
    void begin();
    void attach(uint8_t addrI2C);
    void detach();
    void set(int16_t messageID, char *message);
    void update();

private:
    bool          _initialised;
    uint8_t       _addrI2C;
    OLEDInterface *oled;

    void setTCAChannel(byte i);
    void blankDisplays(void);
    void setBrightness(uint8_t percent);
    void updateDisplayBatt(uint8_t channel, const char *value);
    void updateDisplayAdirs(void);

};
