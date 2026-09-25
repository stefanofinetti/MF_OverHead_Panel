#include "MFCustomDevice.h"
#include "commandmessenger.h"
#include "allocateMem.h"
#include "MFEEPROM.h"
#if defined(HAS_CONFIG_IN_FLASH)
#include "MFCustomDevicesConfig.h"
#else
const char CustomDeviceConfig[] PROGMEM = {};
#endif

extern MFEEPROM MFeeprom;

/* **********************************************************************************
    The custom device pins, type and configuration is stored in the EEPROM.
    While loading the config the adresses in the EEPROM are transferred to the
    constructor. Within the constructor you have to copy the EEPROM content to a
    buffer and evaluate it. The buffer is used for all 3 types (pins, type,
    configuration), so do it step by step.

    This device takes one value only, the I2C address of the multiplexer, but the
    type string "SF_OVHD" passes through the same buffer.
********************************************************************************** */
#define MEMLEN_STRING_BUFFER 40

// reads a string from EEPROM or Flash at given address which is '.' terminated and saves it to the buffer
bool MFCustomDevice::getStringFromMem(uint16_t addrMem, char *buffer, bool configFromFlash)
{
    char     temp    = 0;
    uint8_t  counter = 0;
    uint16_t length  = MFeeprom.get_length();
    do {
        if (configFromFlash) {
            temp = pgm_read_byte_near(CustomDeviceConfig + addrMem++);
            if (addrMem > sizeof(CustomDeviceConfig))
                return false;
        } else {
            temp = MFeeprom.read_byte(addrMem++);
            if (addrMem > length)
                return false;
        }
        buffer[counter++] = temp;              // save character and locate next buffer position
        if (counter >= MEMLEN_STRING_BUFFER) { // nameBuffer will be exceeded
            return false;                      // abort copying to buffer
        }
    } while (temp != '.');      // reads until limiter '.' and locates the next free buffer position
    buffer[counter - 1] = 0x00; // replace '.' by NULL, terminates the string
    return true;
}

MFCustomDevice::MFCustomDevice()
{
    _initialized = false;
}

/* **********************************************************************************
    Within the connector pins, a device name and a config string can be defined.
    These informations are stored in the EEPROM or Flash like for the other devices.
    While reading the config from the EEPROM or Flash this function is called.
    It is the first function which will be called for the custom device.
    If it fits into the memory buffer, the constructor for the customer device
    will be called.
********************************************************************************** */

void MFCustomDevice::attach(uint16_t adrPin, uint16_t adrType, uint16_t adrConfig, bool configFromFlash)
{
    if (adrPin == 0) return;

    char   *params, *p = NULL;
    char    parameter[MEMLEN_STRING_BUFFER];
    uint8_t addrI2C;

    /* **********************************************************************************
        Read the Type from the EEPROM or Flash, copy it into a buffer and evaluate it.
        The string does NOT get stored as this would need a lot of RAM, instead a
        variable is used to store the type.
    ********************************************************************************** */
    getStringFromMem(adrType, parameter, configFromFlash);
    if (strcmp(parameter, "SF_OVHD") == 0)
        _customType = SF_OVHD_DEVICE;

    if (_customType == SF_OVHD_DEVICE) {
        /* **********************************************************************************
            Check if the device fits into the device buffer
        ********************************************************************************** */
        void *mem = MF_ALLOC_TYPE(SF_OVHD, 1);
        if (!mem) {
            // Error Message to Connector
            cmdMessenger.sendCmd(kStatus, F("Custom Device does not fit in Memory"));
            // nothing was constructed, so do not let detach() dereference the pointer
            _customType = 0;
            return;
        }
        /* **********************************************************************************************
            Read the pins from the EEPROM or Flash, copy them into a buffer.
            '"isI2C": true' is set in the device.json file, so the first value is the I2C
            address of the PCA9548A the three displays hang off.
        ********************************************************************************************** */
        getStringFromMem(adrPin, parameter, configFromFlash);
        params  = strtok_r(parameter, "|", &p);
        addrI2C = atoi(params);

        /* **********************************************************************************
            The config string is not used by this device, so it is not read here.
        ********************************************************************************** */

        _my_SF_OVHD = new (mem) SF_OVHD();
        _my_SF_OVHD->attach(addrI2C);
        _my_SF_OVHD->begin();
        _initialized = true;
    } else {
        cmdMessenger.sendCmd(kStatus, F("Custom Device is not supported by this firmware version"));
    }
}

/* **********************************************************************************
    The custom devives gets unregistered if a new config gets uploaded.
    Keep it as it is, mostly nothing must be changed.
    It gets called from CustomerDevice::Clear()
********************************************************************************** */
void MFCustomDevice::detach()
{
    _initialized = false;
    if (_customType == SF_OVHD_DEVICE) {
        _my_SF_OVHD->detach();
    }
}

/* **********************************************************************************
    Within in loop() the update() function is called regularly.
    Within the loop() you can define a time delay where this function gets called
    or as fast as possible. See comments in loop().
    It is only needed if you have to update your custom device without getting
    new values from the connector.
    It gets called from CustomerDevice::update()
********************************************************************************** */
void MFCustomDevice::update()
{
    if (!_initialized) return;

    if (_customType == SF_OVHD_DEVICE) {
        _my_SF_OVHD->update();
    }
}

/* **********************************************************************************
    If an output for the custom device is defined in the connector,
    this function gets called when a new value is available.
    It gets called from CustomerDevice::OnSet()
********************************************************************************** */
void MFCustomDevice::set(int16_t messageID, char *setPoint)
{
    if (!_initialized) return;

    if (_customType == SF_OVHD_DEVICE) {
        _my_SF_OVHD->set(messageID, setPoint);
    }
}
