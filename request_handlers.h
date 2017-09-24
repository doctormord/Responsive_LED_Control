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

// ***************************************************************************
// Request handlers
// ***************************************************************************
void getArgs() {
  if (server.arg("rgb") != "") {
    uint32_t rgb = (uint32_t) strtol(server.arg("rgb").c_str(), NULL, 16);
    settings.main_color.red = ((rgb >> 16) & 0xFF);
    settings.main_color.green = ((rgb >> 8) & 0xFF);
    settings.main_color.blue = ((rgb >> 0) & 0xFF);
  } else {
    settings.main_color.red = server.arg("r").toInt();
    settings.main_color.green = server.arg("g").toInt();
    settings.main_color.blue = server.arg("b").toInt();
  }
  if (server.arg("d") != "") {
    settings.fps = server.arg("d").toInt();
    if (settings.fps == 0) settings.fps = 40; // prevent divide by zero!
  }
  if (settings.main_color.red > 255) {
    settings.main_color.red = 255;
  }
  if (settings.main_color.green > 255) {
    settings.main_color.green = 255;
  }
  if (settings.main_color.blue > 255) {
    settings.main_color.blue = 255;
  }

  if (settings.main_color.red < 0) {
    settings.main_color.red = 0;
  }
  if (settings.main_color.green < 0) {
    settings.main_color.green = 0;
  }
  if (settings.main_color.blue < 0) {
    settings.main_color.blue = 0;
  }

  DBG_OUTPUT_PORT.print("Mode: ");
  DBG_OUTPUT_PORT.print(settings.mode);
  DBG_OUTPUT_PORT.print(", Color: ");
  DBG_OUTPUT_PORT.print(settings.main_color.red);
  DBG_OUTPUT_PORT.print(", ");
  DBG_OUTPUT_PORT.print(settings.main_color.green);
  DBG_OUTPUT_PORT.print(", ");
  DBG_OUTPUT_PORT.print(settings.main_color.blue);
  DBG_OUTPUT_PORT.print(", Delay:");
  DBG_OUTPUT_PORT.print(settings.fps);
  DBG_OUTPUT_PORT.print(", Brightness:");
  DBG_OUTPUT_PORT.println(settings.overall_brightness);
  DBG_OUTPUT_PORT.print(", show_length:");
  DBG_OUTPUT_PORT.println(settings.show_length);
}

void handleMinimalUpload() {
  char temp[1500];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf_P ( temp, 1500,
       PSTR("<!DOCTYPE html>\
				<html>\
					<head>\
						<title>ESP8266 Upload</title>\
						<meta charset=\"utf-8\">\
						<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\
						<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
					</head>\
					<body>\
						<form action=\"/edit\" method=\"post\" enctype=\"multipart/form-data\">\
							<input type=\"file\" name=\"data\">\
							<input type=\"text\" name=\"path\" value=\"/\">\
							<button>Upload</button>\
						</form>\
					</body>\
				</html>"),
             hr, min % 60, sec % 60
           );
  server.send ( 200, "text/html", temp );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.send ( 404, "text/plain", message );
}

char* listStatusJSON() {
  char json[512];
  File file;
  openPaletteFileWithIndex(currentPaletteIndex, &file); 
  snprintf_P(json, sizeof(json), PSTR("{\"mode\":%d, \"FPS\":%d,\"show_length\":%d, \"ftb_speed\":%d, \"overall_brightness\":%d, \"effect_brightness\":%d, \"color\":[%d, %d, %d], \"glitter_color\":[%d,%d,%d], \"glitter_density\":%d, \"glitter_on\":%d, \"confetti_density\":%d, \"palette_name\": \"%s\", \"glitter_wipe_on\": %d}"), settings.mode, settings.fps, settings.show_length, settings.ftb_speed, settings.overall_brightness, settings.effect_brightness, settings.main_color.red, settings.main_color.green, settings.main_color.blue, settings.glitter_color.red, settings.glitter_color.green, settings.glitter_color.blue, settings.glitter_density, settings.glitter_on, settings.confetti_dens, file.name(), settings.glitter_wipe_on);
  file.close();
  return json;
}


void getStatusJSON() {
  server.send ( 200, "application/json", listStatusJSON() );
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      DBG_OUTPUT_PORT.printf("WS: [%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        DBG_OUTPUT_PORT.printf("WS: [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;

    case WStype_TEXT:
      DBG_OUTPUT_PORT.printf("WS: [%u] get Text: %s\n", num, payload);

      // # ==> Set main color
      if (payload[0] == '#') {
        // decode rgb data
        uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);
        settings.main_color.red = ((rgb >> 16) & 0xFF);
        settings.main_color.green = ((rgb >> 8) & 0xFF);
        settings.main_color.blue = ((rgb >> 0) & 0xFF);
        DBG_OUTPUT_PORT.printf("Set main color to: [%u] [%u] [%u]\n", settings.main_color.red, settings.main_color.green, settings.main_color.blue);
        webSocket.sendTXT(num, "OK");
      }

      // # ==> Set glitter color
      if (payload[0] == 'G') {
        // decode rgb data
        uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);
        settings.glitter_color.red = ((rgb >> 16) & 0xFF);
        settings.glitter_color.green = ((rgb >> 8) & 0xFF);
        settings.glitter_color.blue = ((rgb >> 0) & 0xFF);
        DBG_OUTPUT_PORT.printf("Set glitter color to: [%u] [%u] [%u]\n", settings.glitter_color.red, settings.glitter_color.green, settings.glitter_color.blue);
        webSocket.sendTXT(num, "OK");
      }      

      // # ==> Set delay
      if (payload[0] == '?') {
        // decode delay data
        uint8_t d = (uint8_t) strtol((const char *) &payload[1], NULL, 10);
        settings.fps = ((d >> 0) & 0xFF);
        if (settings.fps == 0) settings.fps = 1;   // Prevent divide by zero.
        DBG_OUTPUT_PORT.printf("WS: Set FPS: [%u]\n", settings.fps);
        webSocket.sendTXT(num, "OK");
      }

      // # ==> Set brightness
      if (payload[0] == '%') {
        uint8_t b = (uint8_t) strtol((const char *) &payload[1], NULL, 10);
        settings.overall_brightness = ((b >> 0) & 0xFF);
        DBG_OUTPUT_PORT.printf("WS: Set brightness to: [%u]\n", settings.overall_brightness);
        FastLED.setBrightness(settings.overall_brightness);
        webSocket.sendTXT(num, "OK");
      }

      // e ==> Set effect brightness
      if (payload[0] == 'e') {
        uint8_t b = (uint8_t) strtol((const char *) &payload[1], NULL, 10);
        settings.effect_brightness = ((b >> 0) & 0xFF);
        DBG_OUTPUT_PORT.printf("WS: Set effect brightness to: [%u]\n", settings.effect_brightness);
        webSocket.sendTXT(num, "OK");
      }

      // # ==> Set show_length
      if (payload[0] == '^') {
        uint8_t b = (uint8_t) strtol((const char *) &payload[1], NULL, 10);
        settings.show_length = ((b >> 0) & 0xFF);
        DBG_OUTPUT_PORT.printf("WS: Set show_length to: [%u]\n", settings.show_length);
        webSocket.sendTXT(num, "OK");
      }

      // # ==> Set fade to black speed
      if (payload[0] == '_') {
        uint8_t b = (uint8_t) strtol((const char *) &payload[1], NULL, 10);
        settings.ftb_speed = ((b >> 0) & 0xFF);
        DBG_OUTPUT_PORT.printf("WS: Set fade to black speed to: [%u]\n", settings.ftb_speed);
        webSocket.sendTXT(num, "OK");
      } 

      // # ==> Set fade glitter density
      if (payload[0] == '+') {
        uint8_t b = (uint8_t) strtol((const char *) &payload[1], NULL, 10);
        settings.glitter_density = ((b >> 0) & 0xFF);
        DBG_OUTPUT_PORT.printf("WS: Set fade to glitter density to: [%u]\n", settings.glitter_density);
        webSocket.sendTXT(num, "OK");
      }           


      // * ==> Set main color and light all LEDs (Shortcut)
      if (payload[0] == '*') {
        // decode rgb data
        uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);

        settings.main_color.red = ((rgb >> 16) & 0xFF);
        settings.main_color.green = ((rgb >> 8) & 0xFF);
        settings.main_color.blue = ((rgb >> 0) & 0xFF);

        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB(settings.main_color.red, settings.main_color.green, settings.main_color.blue);
        }
        FastLED.show();
        DBG_OUTPUT_PORT.printf("WS: Set all leds to main color: [%u] [%u] [%u]\n", settings.main_color.red, settings.main_color.green, settings.main_color.blue);
        //exit_func = true;
        settings.mode = ALL;
        webSocket.sendTXT(num, "OK");
      }

      // ! ==> Set single LED in given color
      if (payload[0] == '!') {
        // decode led index
        uint64_t rgb = (uint64_t) strtol((const char *) &payload[1], NULL, 16);

        uint8_t led =          ((rgb >> 24) & 0xFF);
        if (led < NUM_LEDS) {
          ledstates[led].red =   ((rgb >> 16) & 0xFF);
          ledstates[led].green = ((rgb >> 8)  & 0xFF);
          ledstates[led].blue =  ((rgb >> 0)  & 0xFF);
          DBG_OUTPUT_PORT.printf("WS: Set single led [%u] to [%u] [%u] [%u]!\n", led, ledstates[led].red, ledstates[led].green, ledstates[led].blue);

          for (uint8_t i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB(ledstates[i].red, ledstates[i].green, ledstates[i].blue);
            
          }
          FastLED.show();
        }
        //exit_func = true;
        settings.mode = ALL;
        webSocket.sendTXT(num, "OK");
      }

      // ! ==> Activate mode
      if (payload[0] == '=') {
        // we get mode data
        String str_mode = String((char *) &payload[0]);

        //exit_func = true;

        if (str_mode.startsWith("=off")) {
          settings.mode = OFF;
        }
        if (str_mode.startsWith("=all")) {
          settings.mode = ALL;
        }
        if (str_mode.startsWith("=mixedshow")) {
          settings.mode = MIXEDSHOW;
        }    
        if (str_mode.startsWith("=rainbow")) {
          settings.mode = RAINBOW;
        } 
        if (str_mode.startsWith("=confetti")) {
          settings.mode = CONFETTI;
        } 
        if (str_mode.startsWith("=sinelon")) {
          settings.mode = SINELON;
        } 
        if (str_mode.startsWith("=juggle")) {
          settings.mode = JUGGLE;
        }          
        if (str_mode.startsWith("=bpm")) {
          settings.mode = BPM;
        }  
        if (str_mode.startsWith("=palette_anims")) {
          if (settings.palette_ndx != -1) {
             currentPaletteIndex = settings.palette_ndx;
             loadPaletteFromFile(settings.palette_ndx, &targetPalette);
          }
          settings.mode = PALETTE_ANIMS;
        }   
        if (str_mode.startsWith("=ripple")) {
          settings.mode = RIPPLE;
        }   
        if (str_mode.startsWith("=comet")) {
          settings.mode = COMET;
        }     
        if (str_mode.startsWith("=theaterchase")) {
          settings.mode = THEATERCHASE;
        }                   
        if (str_mode.startsWith("=add_glitter")) {
          settings.glitter_on = true;
        }
        if (str_mode.startsWith("=stop_glitter")) {
          settings.glitter_on = false;
        }                                                                                   
        if (str_mode.startsWith("=start_glitter_wipe")) {
          settings.glitter_wipe_on = true;
        }                                                                                   
        if (str_mode.startsWith("=stop_glitter_wipe")) {
          settings.glitter_wipe_on = false;
        }                                                                                   
        if (str_mode.startsWith("=wipe")) {
          settings.mode = WIPE;
        }                                                                                   
        if (str_mode.startsWith("=tv")) {
          settings.mode = TV;
        }
        if (str_mode.startsWith("=fire")) {
          settings.mode = FIRE;
        }   
        if (str_mode.startsWith("=frainbow")) {
          settings.mode = FIRE_RAINBOW;
        }   
        if (str_mode.startsWith("=fworks")) {
          settings.mode = FIREWORKS;
        }
        if (str_mode.startsWith("=fwsingle")) {
          settings.mode = FIREWORKS_SINGLE;
        }           
        if (str_mode.startsWith("=fwrainbow")) {
          settings.mode = FIREWORKS_RAINBOW;
        }                                                                                              
        DBG_OUTPUT_PORT.printf("Activated mode [%u]!\n", settings.mode);
        webSocket.sendTXT(num, "OK");
      }

      // $ ==> Get status Info.
      if (payload[0] == '$') {
        DBG_OUTPUT_PORT.printf("Get status info.");
        
        String json = listStatusJSON();
        DBG_OUTPUT_PORT.println(json);
        webSocket.sendTXT(num, json);
      }

      // ` ==> Restore defaults.
      if (payload[0] == '`') {
        DBG_OUTPUT_PORT.printf("Restore defaults.");
        loadDefaults();
        String json = listStatusJSON();
        DBG_OUTPUT_PORT.println(json);
        webSocket.sendTXT(num, json);
      }

      // | ==> Save settings.
      if (payload[0] == '|') {
        DBG_OUTPUT_PORT.printf("Save settings.");
        saveSettings();
        webSocket.sendTXT(num, "OK");      
      }

      // \ ==> Load settings.
      if (payload[0] == '\\') {
        DBG_OUTPUT_PORT.printf("Load settings.");
        readSettings(false);
        
        String json = listStatusJSON();
        DBG_OUTPUT_PORT.println(json);
        webSocket.sendTXT(num, json);
      }

      // { ==> Change palette
      if (payload[0] == '{') {
        if (length == 2) {
          if (payload[1] == '+') {
            DBG_OUTPUT_PORT.printf("Current pallet_ndx=%d\n", settings.palette_ndx);      
            settings.palette_ndx++;
            if (settings.palette_ndx >= paletteCount) {
              settings.palette_ndx = 0;              
            }
            
            targetPaletteIndex = settings.palette_ndx;
            loadPaletteFromFile(settings.palette_ndx, &targetPalette);
            
            if (settings.glitter_wipe_on) {
              wipeInProgress = true;
            }
            
            DBG_OUTPUT_PORT.printf("Next palette: %d\n", settings.palette_ndx);            
            } else if (payload[1] == '-') {
            DBG_OUTPUT_PORT.printf("Current pallet_ndx=%d\n", settings.palette_ndx);      
            settings.palette_ndx--;
            if (settings.palette_ndx < 0) {
              settings.palette_ndx = paletteCount-1;              
            }
            
            targetPaletteIndex = settings.palette_ndx;
            loadPaletteFromFile(settings.palette_ndx, &targetPalette);
            
            if (settings.glitter_wipe_on) {
              wipeInProgress = true;
            }
            DBG_OUTPUT_PORT.printf("Next palette: %d\n", settings.palette_ndx);            
            } else if (payload[1] == 'r') {
            DBG_OUTPUT_PORT.printf("Randomize palette.\n");
            settings.palette_ndx = -1;          
            ChangePalettePeriodically(true);
          } else if (payload[1] == 'd') {
            DBG_OUTPUT_PORT.printf("Change direction: %d\n", anim_direction);
            anim_direction = (DIRECTION)!anim_direction;
          }
        }
        String json = listStatusJSON();
        DBG_OUTPUT_PORT.println(json);
        webSocket.sendTXT(num, json);
      }

      // " ==> Confetti Density
      if (payload[0] == '"') {
        uint8_t b = (uint8_t) strtol((const char *) &payload[1], NULL, 10);
        settings.confetti_dens = ((b >> 0) & 0xFF);
        DBG_OUTPUT_PORT.printf("WS: Set confetti density to: [%u]\n", settings.confetti_dens);
        webSocket.sendTXT(num, "OK");
      }
      break;
  }
}

void checkForRequests() {
  webSocket.loop();
  server.handleClient();
  }
