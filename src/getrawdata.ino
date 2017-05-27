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
 
bool getrawdata()
{
  for(int i=0;i<NUM_LEDS;i++)
  {
    leds[i] = CRGB::Purple;
    FastLED.show();
  }
  
  if(debug) { Serial.println("Disconnecting Blynk"); }
  Blynk.disconnect(); // Disconnect Blynk, cause API call has to be made 

  // Put current date in for retrieving correct date
  char currentDateBuffer[12];
  sprintf(currentDateBuffer,"%04u-%02u-%02u ", year(currentTime), month(currentTime), day(currentTime) );

  delay(750);
  
  // Make request http://api.sunrise-sunset.org/json?lat=36.7201600&lng=-4.4203400&date=2016-08-25 (with variable data of course)
  if (client.connect(server, 80)) 
  {
    if(debug) { Serial.println("Connecting to server ..."); }
    client.print("GET /json?lat=53.097001&lng=6.054036&date=");
    client.print(currentDateBuffer);
    client.println(" HTTP/1.1");
    client.println("Host: api.sunrise-sunset.org");
    client.println("Accept: text/html");
    client.println("Connection: close");
    client.println();
    
    if(debug) { Serial.println("Connected to server"); }
    
    delay(750);

    // Read all characters into one big, noisy array full of stuff (contents[])
    if(debug) { Serial.println("Now reading data into temp var"); }
    
    int i=0;
    String contents[15];
    String inputString = "";
    
    inputString.reserve(200);
    
    while (client.available())
    {
      char c = client.read();
      inputString += c;
       
      if (c == '\n') 
      {
        inputString.trim(); // Remove dumbass whitespaces and too much CR, LF etc.
        if(debug) { Serial.print(i); }
        if(debug) { Serial.print(": Received non-useless data, processing: "); }
        if(debug) { Serial.println(inputString); }
        contents[i] = inputString;
        inputString = "";
        i++;
      }
    }

    if(contents[0] == "HTTP/1.1 200 OK")
    {
      if(debug) { Serial.println(contents[0]); }
      if(debug) { Serial.println("Stopping client"); }
      client.stop();

      // Result set to this var
      useFullData = contents[11];
      
      if(debug) { Serial.println("Connecting Blynk"); }
      Blynk.connect();
      
      return 1;  
    }
    else
    {
      if(debug) { Serial.print("Error received: "); }
      if(debug) { Serial.println(contents[0]); }
      
      // Set LED's in error state
      for(int i=0;i<NUM_LEDS;i++) { leds[i] = CRGB::Red; FastLED.show(); }

      return 0;
    }
  }
}

