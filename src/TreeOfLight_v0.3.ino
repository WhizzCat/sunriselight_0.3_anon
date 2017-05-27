/*
 * The MIT License
 *
 * Copyright 2016 Bastiaan Visee <info at lichtsignaal.nl>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * Sunrise/sunset timer via API for use with RTC Widget in Blynk
 *
 * Basic program flow
 * - Connect to Blynk
 * - Sync with RTC widget
 * - Disconnect Blynk
 * - Get API raw data
 * - Connect back to Blynk
 * - Process API raw data into something usable (a lot of time variables actually)
 * - Do stuff with said information
 *
 * - Feature list:
 *
 *  + ZeRGBa widget attached for manual control with a hint of randomness
 *  + Debug button, how cool is that...
 *  + Different color's for indicating the status of stuff going on
 *      Red means error (only for runAllFunctions stuff)
 *      Pink means time not synced
 *      Orange means no wifi connection
 *      Purple means start getting raw data
 *      Green means start processing raw data
 *      Off means all data processed (and coloring takes over, based on chosen program)
 *  + If time sync is unable to complete, switch to ZeRGBa widget
 *  + If Sun sync is enabled, start fetching data
 *
 * - Wish-list
 *  + Color fader to set color with time of sunrise/sunset, very slowly
 *  + Predefined mode, with 3 colors displaying from sunrise + 2 to sunset - 2 (white)
 *    from sunset to sunrise (blue-ish) and from between sunrise/set to sunrise/set +2 (orange-ish)
 *  + Color interval changer to be made in Blynk slider (e.g. how much time between color fades from sunrise to ... 2 hours?)
 *  + Predefined setting on/off to Blynk button
 *  + Different twilight settings for SunSync mode (civil, nautical and astronomical)
 *  + Variable number of steps for more colors to fade between
 *  + Preset fading with different twilight settings instead of fixed period
 *  + Customizable timeout for getting raw data
 *  + ...
 *  + What else? (No, I don't do Nespresso, get a Jura instead...)
 */

//#define BLYNK_DEBUG Serial
//
#define BLYNK_PRINT Serial

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//** Includes

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <Time.h>
#include <SimpleTimer.h>
#include "FastLED.h"
//** END

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//** Functions
void blynk_buttons();
void blynk_setup();
void fastled_setup();
bool getrawdata();
void httpupdate_setup();
bool procesdata();
void setcolor();
void setcolorsimple();
void updatecurrenttime();
//** END

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//** Fetching data from the API
char server[] = "api.sunrise-sunset.org";
WiFiClient client;
//** END

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//** FastLED
#define DATA_PIN    D4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    5
#define BRIGHTNESS  255
CRGB leds[NUM_LEDS];
//** END

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//** HTTP UPDATE
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

const char* host = "treeoflight";
const char* ssid = "wifi_ssid_here";
const char* password = "wifi_password_here";

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
//** END

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//** Blynk
char auth[] = "token_here";
WidgetRTC rtc;
WidgetLCD lcd(V6);
//** END

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//** GLOBAL VARIABLES

#define numberOfColors 12

time_t currentTime;                       // Please note, set Blynk app to +0.00 (for now), adjustment is done with TimeZone lib
time_t sunrise;
time_t sunset;
time_t sunriseOriginal;
time_t sunsetOriginal;
time_t solar_noon;
time_t civil_twilight_begin;
time_t civil_twilight_end;
time_t nautical_twilight_begin;
time_t nautical_twilight_end;
time_t astronomical_twilight_begin;
time_t astronomical_twilight_end;

String useFullData;                       // String with contents[11], the real results
char* sunType[4] = { "C", "N", "A", "U" };         // Selected sunrise/set types (C, N, A or U).
int sunTypeIndex;

bool debug          = 1;                  // Debug on/off
bool fetchOK        = 0;                  // Keep track if fetch went ok
bool procesOK       = 0;                  // Keep track if proces went ok
bool enableSunSync  = 1;                  // Sync with sunrise/sunset per default
bool displaySet     = 0;                  // LCD set to prevent too many updates
bool enableUpdate   = 0;                  // Enable/disable the HTTPClient handler for updates

double stepSize[3];                       // Contains steps for color fading
double intervalFadingRise = 550.0;        // Default step period for fading sunrise
double intervalFadingSet  = 250.0;        // Default step period for fading sunset

int currentRed, currentBlue, currentGreen;// RGB values for autofader in SunSync mode
int rZ, gZ, bZ;                           // R, G and B parameters for ZeRGBa widget
int modeSunSync = 0;                      // Set SunSync mode
                                          // 0 is for auto w/ fader
                                          // 1 is for simple on/off at sunrise/set times
                                          // 2 is for 2 different colors (sunrise/ste +/- fadePeriod) color 1 and in the day color 2
                                          // 3 is for ... night mode, very faint?

int modeNoSync = 0;                       // No SunSync mode
                                          // 0 is ZeRGBa control
                                          // 1 is X-Mas madness
                                          // 2 is ... ?
int steps[numberOfColors][3] =  {
                                  { 0   ,0   ,0    },  // Black / off
                                  { 255 ,51  ,0    },  // Red-ish
                                  { 255 ,102 ,51   },  // Dark orange
                                  { 255 ,204 ,102  },  // Light orange
                                  { 255 ,255 ,102  },  // Yelllow-ish
                                  { 204 ,255 ,153  },  // Light green
                                  { 102 ,204 ,153  },  // Sea green
                                  { 51  ,204 ,255  },  // Light blue
                                  { 51  ,153 ,255  },  // Blue-ish
                                  { 102 ,102 ,255  },  // Purple-ish
                                  { 51  ,51  ,204  },  // Purple-blue-ish
                                  { 255 ,255 ,255  }   // White
                                };
//** END

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//** FUNCTION declarations for SimpleTimer
void updatecurrenttime();
//** END

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//** SimpleTimer object
SimpleTimer timer;
//** Timers
byte updateCurrentTimeTimer = timer.setInterval(1000, updatecurrenttime);         // Always runs to keep track of the time
//** END

void(* resetFunc) (void) = 0;

void setup()
{
  Serial.begin(115200);

  httpupdate_setup();
  fastled_setup();
  blynk_setup();
}

void loop()
{
  if(enableUpdate)
  {
    httpServer.handleClient();
  }

  Blynk.run();
  timer.run();
}
