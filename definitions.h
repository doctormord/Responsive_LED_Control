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

//#define FASTLED_ALLOW_INTERRUPTS 0

/// Serial
#define DEBUG_WEBSOCKETS(...) Serial.printf( __VA_ARGS__ )


#define FASTLED_USE_PROGMEM 1
// Note, you need to patch FastLEDs in order to use this.  You'll get an
// error related to <avr\pgmspace.h>. Saves more than 3k given the palettes
//
// Simply edit <fastled_progmem.h> and update the include (Line ~29):
//      #if FASTLED_INCLUDE_PGMSPACE == 1
//      #if (defined(__AVR__))
//      #include <avr\pgmspace.h>
//      #else
//      #include <pgmspace.h>
//      #endif
//      #endif

#define FASTLED_INTERRUPT_RETRY_COUNT 3
#include "FastLED.h"
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define HOSTNAME_PREFIX "GRISWOLD"

//#define REMOTE_DEBUG

#define DATA_PIN 2
//#define CLK_PIN   4
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 120
#define MAX_CURRENT 2000  // limit to max current
#define FASTLED_HZ 100    // maximum FASTLED refresh rate ( default = 400)
CRGB leds[NUM_LEDS];

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

enum MODE { HOLD,
    OFF,
    ALL,
    MIXEDSHOW,
    RAINBOW,
    CONFETTI,
    SINELON,
    JUGGLE,
    BPM,
    PALETTE_ANIMS,
    RIPPLE,
    COMET,
    THEATERCHASE,
    WIPE,
    TV,
    FIRE,
    FIRE_RAINBOW,
    FIREWORKS,
    FIREWORKS_SINGLE,
    FIREWORKS_RAINBOW,};
    
enum DIRECTION {
  BACK = 0,
  FORWARD = 1, };

// These globals moved to the settings struct
//MODE mode = OFF;   // Standard mode that is active when software starts
//uint8_t FPS = 50;               // Global variable for storing the frames per second
//uint8_t brightness = 255;       // Global variable for storing the brightness (255 == 100%)
//uint8_t show_length = 15;       // Global variable for storing the show_time (in seconds)
//uint8_t ftb_speed = 50;         // Global variable for fade to black speed
//uint8_t glitter_density = 50;   // Global variable for glitter density
long lastMillis = 0; // Global variable for timechecking last show cycle time
long theaterMillis = 0;
long paletteMillis = 0; // Global variable for timechecking color palette shifts
//bool exit_func = false; // Global helper variable to get out of the color modes when mode changes
//bool GLITTER_ON = false;        // Global to add / remove glitter to any animation

//******Palette Animation Globals*******************************************
uint8_t targetPaletteIndex;
uint8_t currentPaletteIndex;
uint8_t colorIndex;
DIRECTION anim_direction = FORWARD;
CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
TBlendType currentBlending;

//***************************************************************************

//***************RIPPLE******************************************************
int color;
int center = 0;
int step = -1;
int maxSteps = 32;
float fadeRate = 0.8;
int diff;

//background color
uint32_t currentBg = random(256);
uint32_t nextBg = currentBg;
//******************************************************************************

byte dothue = 0;
int lead_dot = 0;

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

struct ledstate // Data structure to store a state of a single led
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

typedef struct ledstate LEDState; // Define the datatype LEDState
LEDState ledstates[NUM_LEDS]; // Get an array of led states to store the state of the whole strip
//LEDState main_color;                // Store the "main color" of the strip used in single color modes
//LEDState glitter_color;             // Store the "glitter color" of the strip for glitter mode

// Supporting the "Glitter Wipe" effect
#define SPARKLE_SPREAD (_max(NUM_LEDS/80,3))
#define WIPE_SPEED  (_max(NUM_LEDS/120,1))
int16_t wipePos = 0;

#ifdef REMOTE_DEBUG
RemoteDebug Debug;
#define DBG_OUTPUT_PORT Debug
#else
#define DBG_OUTPUT_PORT Serial
#endif
