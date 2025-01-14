#include "SF_OVHD.h"
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include "Fonts/FreeSans8pt7b.h"
#include "Fonts/FreeSans7pt7b.h"
#include "Fonts/FreeSans6pt7b.h"
#include "Fonts/DSEG7Classic_Regular14pt7b.h"
#include "Fonts/DSEG7Classic_Regular15pt7b.h"
#include "Fonts/DSEG7Classic_Regular16pt7b.h"
#include "Fonts/DSEG7Classic_Regular18pt7b.h"
#include "Fonts/DSEG7Classic_Regular20pt7b.h" //https://github.com/keshikan/DSEG and https://rop.nl/truetype2gfx/
#include "Fonts/DSEG7Classic_Regular22pt7b.h"

// Battery 1
String ovhdBatt1Value     = "0.0V";

// Battery 2
String ovhdBatt2Value     = "0.0V";

// ADIRS
String  AdirsValue = "ON BATT";

// light test switch
uint8_t lightTestOn = 0x00;

SF_OVHD::SF_OVHD()
{
    _initialised = false;
}

void SF_OVHD::attach(uint8_t addrI2C)
{
    _addrI2C = addrI2C;
    Wire.begin();
    Wire.setClock(400000);
    if (!FitInMemory(sizeof(OLEDInterface))) {
        // Error Message to Connector
        cmdMessenger.sendCmd(kStatus, F("Custom Device does not fit in Memory"));
        return;
    }
    if (_addrI2C & 0x01) {
        oled = new (allocateMemory(sizeof(OLEDInterface))) OLEDInterface(SSD1306);
    } else {
        oled = new (allocateMemory(sizeof(OLEDInterface))) OLEDInterface(SH1106);
    }
    _initialised = true;
}

void SF_OVHD::begin()
{
    if (!_initialised)
        return;

    //**************************
    // // Battery 1
    // //**************************
    setTCAChannel(TCA9548A_CHANNEL_BATT1);
    oled->begin(SCREEN_ADDRESS, true); // Address 0x3C default
    oled->display();
    updateDisplayBatt1();

    //**************************
    // // Battery 2
    // //**************************
    setTCAChannel(TCA9548A_CHANNEL_BATT2);
    oled->begin(SCREEN_ADDRESS, true); // Address 0x3C default
    oled->display();
    updateDisplayBatt2();


    //**********************************************
    // ADIRS.
    //**********************************************
    setTCAChannel(TCA9548A_CHANNEL_ADIRS);
    oled->begin(SCREEN_ADDRESS, true); // Address 0x3C default
    updateDisplayAdirs();

}

void SF_OVHD::detach()
{
    if (!_initialised)
        return;
    _initialised = false;
}

void SF_OVHD::set(int16_t messageID, char *message)
{
    /* **********************************************************************************
        Each messageID has it's own value
        check for the messageID and define what to do.
        Important Remark!
        MessageID == -1 will be send from the connector when Mobiflight is closed
        Put in your code to shut down your custom device (e.g. clear a display)
        MessageID == -2 will be send from the connector when PowerSavingMode is entered
        Put in your code to enter this mode (e.g. clear a display)

    ********************************************************************************** */
    // do something according your messageID
    // do something according your messageID
    switch (messageID) {
    case -1:
        // tbd., get's called when Mobiflight shuts down
    case -2:
        // tbd., get's called when PowerSavingMode is entered
    case 0:
        /* code */
        ovhdBatt1Value = message;
        updateDisplayBatt1();
        break;
    case 1:
        /* code */
        ovhdBatt2Value = message;
        updateDisplayBatt2();
        break;
    case 2:
        /* code */
        AdirsValue = message;
        updateDisplayAdirs();
        break;
    case 3:
        lightTestOn = atoi(message);
        updateDisplayBatt1();
        updateDisplayBatt2();
        updateDisplayAdirs();
        break;
    default:
        break;
    }
}

void SF_OVHD::update()
{
    // Do something which is required regulary
        updateDisplayBatt1();
        updateDisplayBatt2();
        updateDisplayAdirs();
}

/* ************************************************************************************************
 ************************************************************************************************
 ************************************************************************************************ */

/*
  switch multiplexer channel
*/
void SF_OVHD::setTCAChannel(byte i)
{
    Wire.beginTransmission(_addrI2C);
    Wire.write(1 << i);
    Wire.endTransmission();
    delay(5); // Pause
}

/*******************************************
Has to be redone, only tests
******************************************/
void SF_OVHD::updateDisplayBatt1(void)
{
    setTCAChannel(TCA9548A_CHANNEL_BATT1);
    // Clear the buffer
    oled->clearDisplay();
    oled->setTextColor(SSD1306_WHITE);
    if (lightTestOn == 1) {
        oled->setFont(&DSEG7Classic_Regular20pt7b);
        oled->setCursor(0, 60);
        oled->println("28,8V");
        oled->fillCircle(64, 60, 2, SSD1306_WHITE);
    } else {
        oled->setFont(&DSEG7Classic_Regular20pt7b);
        oled->setCursor(0, 60);
        oled->println(ovhdBatt1Value);        
    }
    oled->display();

} // updateDisplayBatt1

void SF_OVHD::updateDisplayBatt2(void)
{
    setTCAChannel(TCA9548A_CHANNEL_BATT2);
    // Clear the buffer
    oled->clearDisplay();
    oled->setTextColor(SSD1306_WHITE);
    if (lightTestOn == 1) {
        oled->setFont(&DSEG7Classic_Regular20pt7b);
        oled->setCursor(0, 60);
        oled->println("28,8V");
        oled->fillCircle(64, 60, 2, SSD1306_WHITE);
    } else {
        oled->setFont(&DSEG7Classic_Regular20pt7b);
        oled->setCursor(0, 60);
        oled->println(ovhdBatt2Value);        
    }
    oled->display();

} // updateDisplayBatt2

void SF_OVHD::updateDisplayAdirs(void)
{
    setTCAChannel(TCA9548A_CHANNEL_ADIRS);
    // Clear the buffer
    oled->clearDisplay();
    oled->setTextColor(SSD1306_WHITE);
    if (lightTestOn == 1) {
        oled->setFont(&DSEG7Classic_Regular20pt7b);
        oled->setCursor(0, 60);
        oled->println("ON BATT");
        oled->fillCircle(64, 60, 2, SSD1306_WHITE);
    } else {
        oled->setFont(&DSEG7Classic_Regular20pt7b);
        oled->setCursor(0, 60);
        oled->println(AdirsValue);        
    }
    oled->display();

} // updateDisplayAdirs
