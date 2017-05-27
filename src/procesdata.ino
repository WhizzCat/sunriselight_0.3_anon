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

bool procesdata()
{
  for(int i=0;i<NUM_LEDS;i++)
  {
    leds[i] = CRGB::Pink;
    FastLED.show();
  }

  // Temp variable for storing dates and times
  String parameter[10][2];

  // Remove first {"results":
  int firstOpening  = useFullData.indexOf('{');
  int secondOpening = useFullData.indexOf('{', firstOpening + 1);
  useFullData.remove(0, secondOpening + 1);

  // Remove ending ,"status":"OK"}
  int firstClosing  = useFullData.indexOf('}');
  useFullData.remove(firstClosing);

  if(debug) { Serial.println(useFullData); }

  // Put parameters and values into the temporary array parameters[][]
  for(int i=0;i<10;i++)
  {
    firstOpening  = useFullData.indexOf('\"');
    secondOpening = useFullData.indexOf('\"', firstOpening + 1);
    parameter[i][0] = useFullData.substring(firstOpening+1, secondOpening);
    useFullData.remove(0, secondOpening+1);

    firstOpening  = useFullData.indexOf('\"');
    secondOpening = useFullData.indexOf('\"', firstOpening + 1);
    parameter[i][1] = useFullData.substring(firstOpening+1, secondOpening);
    useFullData.remove(0, secondOpening+1);

    if(debug)
    {
      Serial.print("Parameter: ");
      Serial.print(parameter[i][0]);
      Serial.print(" contains: ");
      Serial.println(parameter[i][1]);
    }
  }

  // Convert times into usable UNIX times
  firstOpening        = parameter[0][1].indexOf(':');
  secondOpening       = parameter[0][1].indexOf(':', firstOpening+1);
  int thirdOpening    = parameter[0][1].indexOf(' ', secondOpening+1);

  // Store all the times in the time variables
  for(int i=0;i<10;i++)
  {
    if(i != 2)
    {
      int tempHour    = parameter[i][1].substring(0,firstOpening).toInt();
      int tempMinute  = parameter[i][1].substring(firstOpening+1,secondOpening).toInt();
      int tempSecond  = parameter[i][1].substring(secondOpening+1,thirdOpening).toInt();

      // Correct for 24h clock
      if(parameter[i][1].endsWith("PM") )
      {
        tempHour = tempHour + 12;
      }

      tmElements_t tm;

      tm.Hour   = tempHour;
      tm.Minute = tempMinute;
      tm.Second = tempSecond;
      tm.Day    = day(currentTime);
      tm.Month  = month(currentTime);
      tm.Year   = year(currentTime) - 1970;

      switch(i)
      {
        case 0:
          sunrise         = makeTime(tm);
          sunriseOriginal = makeTime(tm);  // Keep backup of original times for custom times
          //sunrise         = myTZ.toLocal(makeTime(tm), &tcr);
          //sunriseOriginal = myTZ.toLocal(makeTime(tm), &tcr);  // Keep backup of original times for custom times
          if(debug) { Serial.println(sunrise); }
        break;
        case 1:
          sunrise         = makeTime(tm);
          sunriseOriginal = makeTime(tm);  // Keep backup of original times for custom times
          if(debug) { Serial.println(sunset); }
        break;
        case 3:
          solar_noon = makeTime(tm);
          if(debug) { Serial.println(solar_noon); }
        break;
        case 4:
          civil_twilight_begin = makeTime(tm);
          if(debug) { Serial.println(civil_twilight_begin); }
        break;
        case 5:
          civil_twilight_end = makeTime(tm);
          if(debug) { Serial.println(civil_twilight_end); }
        break;
        case 6:
          nautical_twilight_begin = makeTime(tm);
          if(debug) { Serial.println(nautical_twilight_begin); }
        break;
        case 7:
          nautical_twilight_end = makeTime(tm);
          if(debug) { Serial.println(nautical_twilight_end); }
        break;
        case 8:
          astronomical_twilight_begin = makeTime(tm);
          if(debug) { Serial.println(astronomical_twilight_begin); }
        break;
        case 9:
          astronomical_twilight_end = makeTime(tm);
          if(debug) { Serial.println(astronomical_twilight_end); }
        break;
      }
    }
  }

  // Check if both hours of sunrise and sunset are filled, we assume it's ok then.
  if( (hour(sunrise) != 0) && (hour(sunset) != 0) )
  {
    for(int i=0;i<NUM_LEDS;i++)
    {
      leds[i] = CRGB::Black;
      FastLED.show();
    }
    return 1;
  }
  else
  {
    // Set LED's in error state
    for(int i=0;i<NUM_LEDS;i++) { leds[i] = CRGB::Red; FastLED.show(); }

    return 0;
  }
}
