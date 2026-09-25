#include "SF_OVHD.h"
#include <Fonts/FreeSans18pt7b.h>                // ADIRS, from Adafruit GFX
#include "Fonts/DSEG14Modern_Regular20pt7b.h"  // BATT 1 and 2, https://github.com/keshikan/DSEG via https://rop.nl/truetype2gfx/

// Values from the Connector, in fixed buffers so a redraw never touches the
// heap. Each holds more than its display can show at the font it uses.
char ovhdBatt1Value[8]  = "00.00";
char ovhdBatt2Value[8]  = "00.00";
char AdirsValue[16]     = "On batt";

// copies a value from the Connector, cutting it to the buffer rather than overrunning it
static void setValue(char *dest, size_t size, const char *src)
{
    strncpy(dest, src, size - 1);
    dest[size - 1] = 0x00;
}

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
    void *mem = MF_ALLOC_TYPE(OLEDInterface, 1);
    if (!mem) {
        // Error Message to Connector
        cmdMessenger.sendCmd(kStatus, F("Custom Device does not fit in Memory"));
        return;
    }
    if (_addrI2C & 0x01) {
        oled = new (mem) OLEDInterface(SSD1306);
    } else {
        oled = new (mem) OLEDInterface(SH1106);
    }
    // the driver itself may not have fitted; OLEDInterface has reported it
    if (!oled->ready())
        return;
    _initialised = true;
}

void SF_OVHD::begin()
{
    if (!_initialised)
        return;

    // The same three steps for each display: select its channel, initialise the
    // controller behind it, draw. The draw clears the buffer and pushes a
    // frame by itself, so nothing is pushed before it.

    // Battery 1
    setTCAChannel(TCA9548A_CHANNEL_BATT1);
    oled->begin(SCREEN_ADDRESS, true);
    updateDisplayBatt(TCA9548A_CHANNEL_BATT1, ovhdBatt1Value);

    // Battery 2
    setTCAChannel(TCA9548A_CHANNEL_BATT2);
    oled->begin(SCREEN_ADDRESS, true);
    updateDisplayBatt(TCA9548A_CHANNEL_BATT2, ovhdBatt2Value);

    // ADIRS
    setTCAChannel(TCA9548A_CHANNEL_ADIRS);
    oled->begin(SCREEN_ADDRESS, true);
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
    // attach() gives up without a display driver, and then there is nothing to draw on
    if (!_initialised)
        return;

    switch (messageID) {
    case -1: // the Connector is closing
    case -2: // the Connector enters power saving
        // Blank all three rather than leave the last values standing: a static
        // picture burns into an OLED. The next value that arrives redraws its
        // display as usual.
        blankDisplays();
        break;
    case 0:
        /* code */
        setValue(ovhdBatt1Value, sizeof(ovhdBatt1Value), message);
        updateDisplayBatt(TCA9548A_CHANNEL_BATT1, ovhdBatt1Value);
        break;
    case 1:
        /* code */
        setValue(ovhdBatt2Value, sizeof(ovhdBatt2Value), message);
        updateDisplayBatt(TCA9548A_CHANNEL_BATT2, ovhdBatt2Value);
        break;
    case 2:
        /* code */
        setValue(AdirsValue, sizeof(AdirsValue), message);
        updateDisplayAdirs();
        break;
    case 3:
        lightTestOn = atoi(message);
        updateDisplayBatt(TCA9548A_CHANNEL_BATT1, ovhdBatt1Value);
        updateDisplayBatt(TCA9548A_CHANNEL_BATT2, ovhdBatt2Value);
        updateDisplayAdirs();
        break;
    case 4: {
        // 0-100 from the Connector. Until the first one arrives the displays
        // keep their driver's default: 0xCF on SSD1306, 0xFF on SH1106.
        // An empty value is ignored rather than read as 0, which would be darkest.
        if (message[0] == 0x00)
            break;
        int16_t percent = atoi(message);
        setBrightness(percent < 0 ? 0 : (percent > 100 ? 100 : percent));
        break;
    }
    default:
        break;
    }
}

// Only called with -DMF_CUSTOMDEVICE_HAS_UPDATE, which is off: the displays are
// redrawn when a value arrives and at no other time.
void SF_OVHD::update()
{
}

/* ************************************************************************************************
 ************************************************************************************************
 ************************************************************************************************ */

/*
  clear all three displays
*/
void SF_OVHD::blankDisplays(void)
{
    const uint8_t channels[] = {TCA9548A_CHANNEL_BATT1, TCA9548A_CHANNEL_BATT2, TCA9548A_CHANNEL_ADIRS};
    for (uint8_t i = 0; i < sizeof(channels); i++) {
        setTCAChannel(channels[i]);
        oled->clearDisplay();
        oled->display();
    }
}

/*
  set the contrast of all three displays; nothing is redrawn, the picture stays
*/
void SF_OVHD::setBrightness(uint8_t percent)
{
    uint8_t       contrast   = (uint16_t)percent * 255 / 100;
    const uint8_t channels[] = {TCA9548A_CHANNEL_BATT1, TCA9548A_CHANNEL_BATT2, TCA9548A_CHANNEL_ADIRS};
    for (uint8_t i = 0; i < sizeof(channels); i++) {
        setTCAChannel(channels[i]);
        oled->setContrast(contrast);
    }
}

/*
  switch multiplexer channel
*/
void SF_OVHD::setTCAChannel(byte i)
{
    Wire.beginTransmission(_addrI2C);
    Wire.write(1 << i);
    // The PCA9548A switches on the STOP that endTransmission() waits for, so the
    // channel is live by the time this returns and needs no pause.
    Wire.endTransmission();
}

/*
  width the cursor advances over a text, summed from the font's own glyph table
*/
static int16_t textAdvance(const GFXfont *font, const char *text)
{
    uint8_t   first  = pgm_read_byte(&font->first);
    uint8_t   last   = pgm_read_byte(&font->last);
    GFXglyph *glyphs = (GFXglyph *)pgm_read_ptr(&font->glyph);
    int16_t   width  = 0;
    for (; *text; text++) {
        uint8_t c = *text;
        if (c < first || c > last)
            continue;
        width += pgm_read_byte(&glyphs[c - first].xAdvance);
    }
    return width;
}

/*******************************************
Has to be redone, only tests
******************************************/
void SF_OVHD::updateDisplayBatt(uint8_t channel, const char *value)
{
    const char *text = (lightTestOn == 1) ? "28.80" : value;

    setTCAChannel(channel);
    // Clear the buffer
    oled->clearDisplay();
    oled->setTextColor(SSD1306_WHITE);
    oled->setFont(&DSEG14Modern_Regular20pt7b);
    // Right-aligned on the digit cells, as on a segment display: DSEG14 gives
    // every digit the same advance and the point none, so "28.80" fills the
    // 128 pixels from x=0 and "9.80" starts one cell in, at x=32.
    int16_t x = SCREEN_WIDTH - textAdvance(&DSEG14Modern_Regular20pt7b, text);
    oled->setCursor(x < 0 ? 0 : x, 60);
    oled->println(text);
    if (lightTestOn == 1)
        oled->fillCircle(64, 60, 2, SSD1306_WHITE);
    oled->display();

} // updateDisplayBatt

void SF_OVHD::updateDisplayAdirs(void)
{
    setTCAChannel(TCA9548A_CHANNEL_ADIRS);
    // Clear the buffer
    oled->clearDisplay();
    oled->setTextColor(SSD1306_WHITE);
    if (lightTestOn == 1) {
        oled->setFont(&FreeSans18pt7b);
        oled->setCursor(0, 40);
        oled->println("On Batt");
        oled->fillCircle(64, 60, 2, SSD1306_WHITE);
    } else {
        oled->setFont(&FreeSans18pt7b);
        oled->setCursor(0, 40);
        oled->println(AdirsValue);        
    }
    oled->display();

} // updateDisplayAdirs
