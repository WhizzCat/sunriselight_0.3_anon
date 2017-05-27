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

/*
 * V1     Debug
 * V2     SunSync
 * V3     ZeRGBa for manual color
 * V4     Twilight selection
 * V6     LCD Widget
 * V7     Slider for SunSync Mode selector
 * V8     Slider for Standalone Mode selector
 * V10, V11, V12  RGB values for checking
 */

BLYNK_CONNECTED()
{
  Blynk.syncAll();
  //Blynk.syncVirtual(V1); // Debug
  //Blynk.syncVirtual(V2); // Enable/disable SunSync
  //Blynk.syncVirtual(V3); // ZeRGBa sync
  //Blynk.syncVirtual(V4); // Twilight fading settings
}

BLYNK_WRITE(V0)  {  if(param.asInt()) { resetFunc(); } }

BLYNK_WRITE(V1)
{
  debug = param.asInt();
}

BLYNK_WRITE(V2)
{
  enableSunSync = param.asInt();
  displaySet = 0;

  if(!enableSunSync)
  {
    timer.disable(updateCurrentTimeTimer);

    Blynk.syncVirtual(V3); // ZeRGBa sync
    Blynk.virtualWrite(V3, rZ, gZ, bZ);

    lcd.clear();
    lcd.print(0, 0, "SunSync Off");
    lcd.print(0, 1, rZ);
    lcd.print(4, 1, gZ);
    lcd.print(8, 1, bZ);
  }
  else
  {
    timer.enable(updateCurrentTimeTimer);
  }
}

BLYNK_WRITE(V3)
{
  rZ = param[0].asInt();
  gZ = param[1].asInt();
  bZ = param[2].asInt();

  if(!enableSunSync)
  {
    lcd.clear();
    lcd.print(0, 0, "SunSync Off");
    lcd.print(0, 1, rZ);
    lcd.print(4, 1, gZ);
    lcd.print(8, 1, bZ);

    if(modeNoSync == 0)
    {
      for(int i=0;i<NUM_LEDS;i++)
      {
        leds[i] = CRGB(rZ,gZ,bZ);
        FastLED.show();
      }
    }
    else
    {
      lcd.clear();
      lcd.print(0, 0, "Error");
      lcd.print(0, 1, "Mode 0 n/ on");
    }

  }
}

BLYNK_WRITE(V4)
{
  sunTypeIndex = param.asInt();

  switch(sunTypeIndex)
  {
    case 1:
      intervalFadingRise  = (sunrise - civil_twilight_begin) / numberOfColors;
      intervalFadingSet   = (civil_twilight_end - sunset) / numberOfColors;
      sunrise = civil_twilight_begin;
      sunset  = civil_twilight_end;
      sunTypeIndex = 0;
      displaySet = 0;

      if(debug)
      {
        Serial.print("Selection Civil Sunrise: ");
        Serial.print(hour(sunrise));
        Serial.print(":");
        Serial.println(minute(sunrise));

        Serial.print("Selection Civil Sunset: ");
        Serial.print(hour(sunset));
        Serial.print(":");
        Serial.println(minute(sunset));

        Serial.println("Selection Civil Fading (/numberOfColors): ");
        Serial.println(intervalFadingRise);
        Serial.println(intervalFadingSet);
      }

    break;
    case 2:
      intervalFadingRise  = (sunrise - nautical_twilight_begin) / numberOfColors;
      intervalFadingSet   = (nautical_twilight_end - sunset) / numberOfColors;
      sunrise = nautical_twilight_begin;
      sunset  = nautical_twilight_end;
      sunTypeIndex = 1;
      displaySet = 0;

      if(debug)
      {
        Serial.print("Selection Nautical Sunrise: ");
        Serial.print(hour(sunrise));
        Serial.print(":");
        Serial.println(minute(sunrise));

        Serial.print("Selection Nautical Sunset: ");
        Serial.print(hour(sunset));
        Serial.print(":");
        Serial.println(minute(sunset));

        Serial.println("Selection Nautical Fading (/numberOfColors): ");
        Serial.println(intervalFadingRise);
        Serial.println(intervalFadingSet);
      }

    break;
    case 3:
      intervalFadingRise  = (sunrise - astronomical_twilight_begin) / numberOfColors;
      intervalFadingSet   = (astronomical_twilight_end - sunset) / numberOfColors;
      sunrise = astronomical_twilight_begin;
      sunset  = astronomical_twilight_end;
      sunTypeIndex = 2;
      displaySet = 0;

      if(debug)
      {
        Serial.print("Selection Astronomical Sunrise: ");
        Serial.print(hour(sunrise));
        Serial.print(":");
        Serial.println(minute(sunrise));

        Serial.print("Selection Astronomical Sunset: ");
        Serial.print(hour(sunset));
        Serial.print(":");
        Serial.println(minute(sunset));

        Serial.println("Selection Astronomical Fading (/numberOfColors): ");
        Serial.println(intervalFadingRise);
        Serial.println(intervalFadingSet);
      }

    break;
    case 4:
      intervalFadingRise = 550;
      intervalFadingSet  = 250;
      sunrise = sunriseOriginal;
      sunset  = sunsetOriginal;
      sunTypeIndex = 3;
      displaySet = 0;

      if(debug)
      {
        Serial.print("Selection User Sunrise: ");
        Serial.print(hour(sunrise));
        Serial.print(":");
        Serial.println(minute(sunrise));

        Serial.print("Selection User Sunset: ");
        Serial.print(hour(sunset));
        Serial.print(":");
        Serial.println(minute(sunset));

        Serial.println("Selection User Fading (/12): ");
        Serial.println(intervalFadingRise);
        Serial.println(intervalFadingSet);
      }

    break;
  }
}

BLYNK_WRITE(V7)
{
  // Sunrise mode selector, exit if SunSync is disabled
  if(!enableSunSync)
  {
    return;
  }

  modeSunSync = param.asInt();
  displaySet = 0;
}

BLYNK_WRITE(V8)
{
  // Standalone mode selector, exit if SunSync is enabled
  if(enableSunSync)
  {
    return;
  }

  modeNoSync = param.asInt();

  switch(modeNoSync)
  {
    case 0:
      Blynk.syncVirtual(V3); // ZeRGBa sync
      Blynk.virtualWrite(V3, rZ, gZ, bZ);
    break;
  }
}

BLYNK_WRITE(V9)
{
  enableUpdate = param.asInt();
}
