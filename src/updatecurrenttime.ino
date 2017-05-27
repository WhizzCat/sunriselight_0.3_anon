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

 void updatecurrenttime()
{
  // If SunSync is disabled, skip this whole thing and set a color in case ZeRGBa is not touched/synced yet
  if(!enableSunSync)
  {
    if(debug) { Serial.println("Sunsync disabled (manual or timeout)"); }
    return;
  }

  if(year() == 1970)
  {
    if(debug) { Serial.println("Time sync not OK yet, trying again"); }

    lcd.clear();
    lcd.print(0, 0, "SunSync On");
    lcd.print(0, 1, "TimeSync n/ OK");

    // Set LED's in error state
    for(int i=0;i<NUM_LEDS;i++) { leds[i] = CRGB::Red; FastLED.show(); }

    // Keep running this bit of code until TimeSync is OK
    return;
  }
  else
  {
    if(debug) { Serial.print("Time sync OK, continue, UNIX-time (local): "); }

    currentTime = now();

    if(debug) { Serial.println(currentTime); }

    switch(modeSunSync)
    {
      case 0:
        // Auto fading with sunset/rise, the whole nine yards
        if(procesOK)
        {
          setcolor();
        }
      break;
      case 1:
        // Simple on/off at sunrise/sunset times, color setting via ZeRGBa widget
        if(procesOK)
        {
          setcolorsimple();
        }
      break;
      case 2:
        if(procesOK)
        {
          //setcolor();
        }
      break;
    }

    if(!displaySet)
    {
      char currentTimeBuffer[8];
      char currentSunriseBuffer[8];
      char currentSunsetBuffer[8];

      sprintf(currentTimeBuffer   ,"%02u:%02u ", hour(currentTime), minute(currentTime) );
      sprintf(currentSunriseBuffer,"%02u:%02u ", hour(sunrise)    , minute(sunrise) );
      sprintf(currentSunsetBuffer ,"%02u:%02u ", hour(sunset)     , minute(sunset) );

      String lineOne = "SynOK " + String(currentTimeBuffer) + "M" + String(modeSunSync) +" " + String(sunType[sunTypeIndex]);
      String lineTwo = "R:" + String(currentSunriseBuffer) + "S:" + String(currentSunsetBuffer);

      lcd.clear();
      lcd.print(0, 0, lineOne);
      lcd.print(0, 1, lineTwo);

      displaySet = 1;
    }
  }

  /*
   * Setup some stuff to determine if today has passed or not
   */

  String sunriseDate = String(year(sunrise)) + String(month(sunrise)) + String(day(sunrise));
  String compareDate = String(year(currentTime)) + String(month(currentTime)) + String(day(currentTime));

  if(debug) { Serial.println("Date compare: "); }
  if(debug) { Serial.println(sunriseDate); }
  if(debug) { Serial.println(compareDate); }

  // Reset fetch and proces values because they need to get new date and times
  if( (sunriseDate != compareDate) && (fetchOK) && (procesOK) )
  {
    if(debug) { Serial.println("A new day has dawned"); }
    fetchOK = 0;        // Update raw data
    procesOK = 0;       // Proces again
    displaySet = 0;     // Set new values to display
  }

  /*
   * END Setup some stuff to determine if today has passed or not
   */

  /*
   * GET RAW DATA SET
   */

  // if fetchOK is set to 1 and skip
  if(fetchOK)
  {
    return;
  }
  else
  {
    bool getrawdataResults = getrawdata();

    // if result gives back 1, it's OK
    if(getrawdataResults)
    {
      if(debug)
      {
        Serial.println("***************************");
        Serial.println("* Fetch OK                *");
        Serial.println("***************************");
      }
      fetchOK = 1;
    }
    else
    {
      if(debug)
      {
        Serial.println("***************************");
        Serial.println("* Fetch FAILED            *");
        Serial.println("***************************");
      }
      lcd.clear();
      lcd.print(0, 0,  "Fetch FAILED");
      fetchOK = 0;
    }
  }

  /*
   * END GET RAW DATA SET
   */

  /*
   * PROCES DATA INTO SOMETHING USABLE
   */

  // If proces is ok, set to 1 and skip
  if(procesOK)
  {
    return;
  }
  else
  {
    bool procesdataResult = procesdata();

    if(procesdata)
    {
      if(debug)
      {
        Serial.println("***************************");
        Serial.println("* Proces OK               *");
        Serial.println("***************************");
      }
      procesOK = 1;
    }
    else
    {
      if(debug)
      {
        Serial.println("***************************");
        Serial.println("* Proces FAILED           *");
        Serial.println("***************************");
      }
      lcd.clear();
      lcd.print(0, 0,  "Proces FAILED");
      procesOK = 0;
    }
  }

  /*
   * END PROCES DATA INTO SOMETHING USABLE
   */
}
