#pragma once

// Only used when -DHAS_CONFIG_IN_FLASH is enabled in sf_fcu_platformio.ini.
// This device takes its configuration from the Connector, so nothing is baked
// into flash. The file is kept so the build works either way.
const char CustomDeviceConfig[] PROGMEM = {""};
