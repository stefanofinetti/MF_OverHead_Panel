#pragma once

#include "Arduino.h"
#include "OLEDInterface.h"

// address of the multiplexer to change the channels
#define TCA9548A_I2C_ADDRESS  0x70
#define TCA9548A_CHANNEL_BATT1 0
#define TCA9548A_CHANNEL_BATT2 1
#define TCA9548A_CHANNEL_ADIRS    3


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
    void updateDisplayBatt1(void);
    void updateDisplayBatt2(void);
    void updateDisplayAdirs(void);

};
