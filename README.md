# doctormord's FASTLEDs
I mixed the work of [McLighting](https://github.com/toblum/McLighting) and [Russell](https://github.com/russp81/LEDLAMP_FASTLEDs) with [FastLED](https://github.com/FastLED/FastLED) (FastLED library 3.1.3 as of this writing), the colorjs colorpicker, color spectrums created via FastLED Palette Knife, and some additional strip animations (included in the Arduino Sketch above).

FastLED 3.1.3 library:
https://github.com/FastLED/FastLED

McLighting library:
https://github.com/toblum/McLighting

Russel's implementation:
https://github.com/russp81/LEDLAMP_FASTLEDs

jscolor Color Picker:
http://jscolor.com/

FastLED Palette Knife:
http://fastled.io/tools/paletteknife/

If you aren't familiar with how to setup your ESP8266, see the readme on McLighting's git.  It's well written and should get you up and running.

In short you will:

1.  Configure the Arduino IDE to communicate with the ESP8266
2.  Upload the sketch (from this repo) The sketch is setup for a 240 pixel WS2812B GRB LED Strip.   
    (change the applicable options in "definitions.h" to your desire)
3.  On first launch, the ESP8266 will advertise it's own WiFi network for you to connect to, once you connect to it, launch your browser
    and the web interface is self explanatory.  (If the interface doesn't load, type in "192.168.4.1" into your browser and hit go)
4.  Once the ESP is on your wifi network, you can then upload the required files for the web interface by typing the in IP address
    of the ESP followed by "/edit" (i.e. 192.168.1.20/edit).  Then upload the files from the folder labeled "upload these" from this         repo. 
5.  Once you have finished uploading, type in the IP of the ESP into your browser and you should be up and running!

Forked from Russel, i removed Adafruit Neopixel references and library calls.

Added/changed so far:

* new effect: Fire (from WS2812FX)
* new effect: Fireworks [single color, rainbow, random] (from McLightning, ported to used FastLED instead off Adafruit Neopixel)
* new settings for effects in webinterface *.htm 
* integrated Arduino OTA 


~~I edited clockless_esp8266.h (in the FastLED platforms folder) and 
kept getting flickering until I incremented the WAIT_TIME up to 18us. 
(also I did "#define FASTLED_INTERRUPT_RETRY_COUNT 3" inside my sketch).~~

For reference, interrupts issue:  https://github.com/FastLED/FastLED/issues/306


# Jake's "Griswold" LED controller for Christmas Lights.

I bought 1000 WS2811 nodes for my outdoor Christmas light installation this year.
Based on the "Russell's FASTLEDs" project by @russp81, which is in turn based on the "McLighting" project by @toblum

It seemed necessary to name the thing after Clark Griswold, but really just to differentiate this fork from the originals.

![Clark Griswold](http://i.giphy.com/gB9wIPXav2Ryg.gif)

@russp81 mixed the work of @toblum with the @FastLED (FastLED library 3.1.3 as of this writing), the colorjs colorpicker, color spectrums created via FastLED Palette Knife, and some additional strip animations.

# License

As per the original [McLighting](https://github.com/toblum/McLighting) project, this project is released under the GNU LESSER GENERAL PUBLIC LICENSE Version 3, 29 June 2007.

	Griswold is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as 
	published by the Free Software Foundation, either version 3 of 
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Improvements

- Palettes stored as binary files on SPIFFS.  See below for more information on this.
- Display name of the current palette file in the web interface.
- Added ArduinoOTA support so I can update the firmware over WiFi, which will be important when its installed outside.
- Added the ability to store the settings in EEPROM and restore on boot.
- Merged the jscolor interface into the original McLighting interface
- Updated the McLighting interface to retrieve the current settings from the device, and update the UI with the current settings, rather than always default to the defaults.
- General code formatting clean-up.
- Added “RemoteDebug” library for serial console over telnet. (Optional #define)
- Fixed divide-by-zero error that occurs when fps=0 by preventing fps=0 from the UI.
- Updates to the “animated palette” function, now you can select a single palette, or the existing randomized palette after time delay.
- Rearchitected things a bit, now the colormodes.h functions render one single frame, and do not block the main thread.
- Added back the wipe and tv animations from the original McLighting project (removed in LEDLAMP_FASTLEDs)
- Modified TV animation to add some flicker (I like it better)
- Added “effect brightness” setting to allow you to dim the main effect independently of glitter.

Russell's FASTLEDs:
https://github.com/russp81/LEDLAMP_FASTLEDs

FastLED 3.1.3 library:
https://github.com/FastLED/FastLED

McLighting library:
https://github.com/toblum/McLighting

jscolor Color Picker:
http://jscolor.com/

FastLED Palette Knife:
http://fastled.io/tools/paletteknife/

RemoteDebug:
https://github.com/JoaoLopesF/RemoteDebug

# Palettes on SPIFFS

Normally, you use [PaletteKnife](http://fastled.io/tools/paletteknife/) to generate arrays with the palette info.  You then compile this data into your project.  I wanted to be able to update the palettes without recompiling, so I moved them to files in SPIFFS (/palettes directory).  There is a little python program that basically takes the logic from PaletteKnife and outputs a binary file with the palette data instead.  Load these binary files to SPIFFS using the [Arduino ESP8266 filesystem uploader](https://github.com/esp8266/arduino-esp8266fs-plugin) or manually.

# Portions of @russp81's original README

If you aren't familiar with how to setup your ESP8266, see the readme on McLighting's git.  It's well written and should get you up and running.

In short you will:

1.  Configure the Arduino IDE to communicate with the ESP8266
2.  Upload the sketch (from this repo) The sketch is setup for a 240 pixel WS2812B GRB LED Strip.   
    (change the applicable options in "definitions.h" to your desire)
3.  On first launch, the ESP8266 will advertise it's own WiFi network for you to connect to, once you connect to it, launch your browser
    and the web interface is self explanatory.  (If the interface doesn't load, type in "192.168.4.1" into your browser and hit go)
4.  Once the ESP is on your wifi network, you can then upload the required files for the web interface by typing the in IP address
    of the ESP followed by "/edit" (i.e. 192.168.1.20/edit).  Then upload the files from the folder labeled "upload these" from this         repo. 
5.  Once you have finished uploading, type in the IP of the ESP into your browser and you should be up and running!


My work was all on adding FastLED (and other tweaks / animations) into the McLighting sketch instead of using Adafruit NeoPixel.

I am a self taught coder who learns by a few books, google, and looking at other's code, 
and I just liked the things you can do in FastLED better, so I decided to tackle the 
idea of integrating FastLED into the already awesome work of @toblum.

I have a limited grasp on the h/w and s/w relationships (do's and don't do's, etc).  
I edited clockless_esp8266.h (in the FastLED platforms folder) and 
kept getting flickering until I incremented the WAIT_TIME up to 18us. 
(also I did "#define FASTLED_INTERRUPT_RETRY_COUNT 3" inside my sketch).

If I disabled interrupts altogether "#define FASTLED_ALLOW_INTERRUPTS 0", the strip would stop flickering but I would get
what I believe to be "watchdog resets" every 5 to 20 minutes depending on what animation was running, wifi traffic, etc...

For reference, I learned more about the interrupts issue from here:  https://github.com/FastLED/FastLED/issues/306

If anyone can shed more light on this I am all ears!  I'm not sure exactly what the implications are
for setting the WAIT_TIME = 18us??  Everything appears to function properly, and so far I have not seen 
a reset in a few hours.

Also, I added a separate color picker from (http://jscolor.com/).  My idea with this is to eventually create
spectrums using multiple color pickers via the web interface (instead of grinding out coding in the Arduino IDE)
and eventually animate them as well.  When I am finished with this project, I (we) will hopefully be able to build those
spectrums, save them to the ESP8266 flash memory, and have a universal routine in the Arduino Sketch that can handle 
the input / output of the spectrums to the strip (even running animations with them).  I also might even try making a web interface
to create custom animations from, but that seems like a pretty decent challenge from what I can tell. (I am just now finding my way 
around in html/css/js so I have A LOT of learning to do!)

I will say again, I'm a rookie who has tinkered around in a little of each major language with no formal education, so 
if you see something that doesn't look right, it probably isn't!  I am very open to suggestions / learning anything 
anyone is willing to share.

