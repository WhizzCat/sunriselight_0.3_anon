/*
 * Sunrise/sunset timer via API for use with RTC Widget in Blynk
 * 
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
 
 void httpupdate_setup()
{
  if(debug) { Serial.println(); }
  if(debug) { Serial.println("Booting Sketch..."); }
  WiFi.mode(WIFI_STA);
  WiFi.softAPdisconnect(true);

  while(WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    if(debug) { Serial.println("WiFi failed, retrying."); }
    for(int i=0;i<NUM_LEDS;i++)
      {
        leds[i] = CRGB::Orange;
        FastLED.show();
      }
  }

  MDNS.begin(host);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  if(debug) { Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host); }
  if(debug) { Serial.println(""); }
  if(debug) { Serial.print ( "IP address: " ); }
  if(debug) { Serial.println ( WiFi.localIP() ); }
}

