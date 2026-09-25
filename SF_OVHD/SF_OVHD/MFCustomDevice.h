#pragma once

#include <Arduino.h>
#include "SF_OVHD.h"

// only one entry required if you have only one custom device
enum {
    SF_OVHD_DEVICE = 1
};
class MFCustomDevice
{
public:
    MFCustomDevice();
    void attach(uint16_t adrPin, uint16_t adrType, uint16_t adrConfig, bool configFromFlash = false);
    void detach();
    void update();
    void set(int16_t messageID, char *setPoint);

private:
    bool     getStringFromMem(uint16_t addrMem, char *buffer, bool configFromFlash);
    bool     _initialized = false;
    SF_OVHD *_my_SF_OVHD;
    uint8_t  _customType = 0;
};
