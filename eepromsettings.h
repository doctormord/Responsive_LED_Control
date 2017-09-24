// Copyright (c) 2016 @jake-b, @russp81, @toblum
// Griswold LED Lighting Controller

// Griswold is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as 
// published by the Free Software Foundation, either version 3 of 
// the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Griswold is a fork of the LEDLAMP project at 
//        https://github.com/russp81/LEDLAMP_FASTLEDs

// The LEDLAMP project is a fork of the McLighting Project at
//        https://github.com/toblum/McLighting

#include <EEPROM.h>

// EEPROM stuff
// adapted from https://github.com/esp8266/Arduino/issues/1090

#pragma pack(push)  // push current alignment to stack
#pragma pack(1)     // set alignment to 1 byte boundary
typedef struct {
  MODE mode;
  uint8_t fps = 50;  // Global variable for storing the frames per second
  uint8_t overall_brightness = 255;  // Global variable for storing the brightness (255 == 100%)
  uint8_t show_length = 15;  // Global variable for storing the show_time (in seconds)
  uint8_t ftb_speed = 50;        // Global variable for fade to black speed
  uint8_t glitter_density = 50;  // Global variable for glitter density
  bool glitter_on = false;  // Global to add / remove glitter to any animation
  LEDState main_color;  // Store the "main color" of the strip used in single
  // color modes
  LEDState glitter_color;  // Store the "glitter color" of the strip for glitter mode
  uint8_t effect_brightness = 255;  // Brightness used for effect animations
  int8_t palette_ndx = -1; // Palette to use for PALETTE_ANIMS.  -1 is change periodically
  uint8_t confetti_dens = 1;  // Density for the confetti effect.  More confetti needed for longer strings.
  bool glitter_wipe_on = false;
  uint8_t filler[46];  // in case adding data in config avoiding loosing current conf by bad crc
  uint16_t crc;
} EEPROMSettings;
#pragma pack(pop)

EEPROMSettings settings;

uint16_t crc16Update(uint16_t crc, uint8_t a) {
  int i;
  crc ^= a;
  for (i = 0; i < 8; ++i) {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }
  return crc;
}

void loadDefaults() {
  settings.mode = OFF;
  settings.fps = 50;  // Global variable for storing the frames per second
  settings.overall_brightness = 255;  // Global variable for storing the brightness (255 == 100%)
  settings.effect_brightness = 128;  // Global variable for storing the palette brightness (255 == 100%)
  settings.show_length = 15;  // Global variable for storing the show_time (in seconds)
  settings.ftb_speed = 50;        // Global variable for fade to black speed
  settings.glitter_density = 50;  // Global variable for glitter density
  settings.glitter_on = false;  // Global to add / remove glitter to any animation
  settings.main_color = { 128, 128, 128};  // Store the "main color" of the strip used in single color modes
  settings.glitter_color = {128, 128, 128};
  settings.palette_ndx = -1;
  settings.confetti_dens = 1;
  settings.glitter_wipe_on = false;
}

bool readSettings(bool clear_on_error) {
  uint16_t crc = ~0;
  uint8_t* pconfig = (uint8_t*)&settings;
  uint8_t data;

  // For whole size of config structure
  for (uint16_t i = 0; i < sizeof(EEPROMSettings); ++i) {
    // read data
    data = EEPROM.read(i);

    // save into struct
    *pconfig++ = data;

    // calc CRC
    crc = crc16Update(crc, data);
  }

  // CRC Error ?
  if (crc != 0) {
    DBG_OUTPUT_PORT.println("Settings CRC failed on read from EEPROM");
    // Clear config if wanted
    if (clear_on_error) {
      memset(&settings, 0, sizeof(EEPROMSettings));

      // Set defaults
      loadDefaults();
      return false;
    }
  }

  DBG_OUTPUT_PORT.println("Settings successfully read from EERPOM");
  return true;
}

bool saveSettings(void) {
  uint8_t* pconfig;
  bool ret_code;

  // Init pointer
  pconfig = (uint8_t*)&settings;

  // Init CRC
  settings.crc = ~0;

  // For whole size of config structure, pre-calculate CRC
  for (uint16_t i = 0; i < sizeof(EEPROMSettings) - 2; ++i)
    settings.crc = crc16Update(settings.crc, *pconfig++);

  // Re init pointer
  pconfig = (uint8_t*)&settings;

  // For whole size of config structure, write to EEP
  for (uint16_t i = 0; i < sizeof(EEPROMSettings); ++i)
    EEPROM.write(i, *pconfig++);

  // Physically save
  EEPROM.commit();

  // Read Again to see if saved ok, but do
  // not clear if error this avoid clearing
  // default config and breaks OTA
  ret_code = readSettings(false);

  DBG_OUTPUT_PORT.print("Write settings to EEPROM: ");

  if (ret_code)
    DBG_OUTPUT_PORT.println(F("OK!"));
  else
    DBG_OUTPUT_PORT.println(F("Error!"));

  // return result
  return (ret_code);
}

void printSettings() {
  DBG_OUTPUT_PORT.println("Current settings in RAM:");

  DBG_OUTPUT_PORT.printf("mode:               %d\n",
                         settings.mode);
  DBG_OUTPUT_PORT.printf("fps:                %d\n", 
                         settings.fps);  // Global variable for storing the frames per second
  DBG_OUTPUT_PORT.printf("overall_brightness: %d\n", 
                         settings.overall_brightness);  // Global variable for storing the brightness (255 == 100%)
  DBG_OUTPUT_PORT.printf("effect_brightness:  %d\n",
                         settings.effect_brightness);  // Global variable for storing the brightnes (255 == 100%)
  DBG_OUTPUT_PORT.printf("show_length:        %d\n",
                         settings.show_length);  // Global variable for storing
  // the show_time (in seconds)
  DBG_OUTPUT_PORT.printf("ftb_speed:          %d\n",
                          settings.ftb_speed);  // Global variable for fade to black speed
  DBG_OUTPUT_PORT.printf("glitter_density:    %d\n",
                         settings.glitter_density);  // Global variable for glitter density
  DBG_OUTPUT_PORT.printf("glitter_on:         %d\n",
                         settings.glitter_on);  // Global to add / remove glitter to any animation
  DBG_OUTPUT_PORT.printf("main_color:         %d,%d,%d\n",
                         settings.main_color.red, settings.main_color.green,
                         settings.main_color.blue);  // Store the "main color" of the strip used in single color modes
  DBG_OUTPUT_PORT.printf("glitter_color:      %d,%d,%d\n", settings.glitter_color.red,
                         settings.glitter_color.green, settings.glitter_color.blue);
  DBG_OUTPUT_PORT.printf("palette_ndx:        %d\n", settings.palette_ndx); // selected palette
  DBG_OUTPUT_PORT.printf("confetti_dens:      %d\n", settings.confetti_dens); // selected palette
}

void initSettings() {
  EEPROM.begin(sizeof(EEPROMSettings));
  if (readSettings(true)) {
    DBG_OUTPUT_PORT.println("Successfully read settings from EEPROM");
  } else {
    DBG_OUTPUT_PORT.println(
        "Failed read settings from EEPROM, defaults loaded.");
  }
}
