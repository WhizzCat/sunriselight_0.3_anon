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
 
// This is where all the magic coloring happens when sunSync is enabeled
void setcolor()
{
  int currentFromStep[3];
  int currentToStep[3];

  //SUNRISE
  int maxStepRisePeriod = (numberOfColors-1) * intervalFadingRise;
  int currentStepRise = currentTime - sunrise;
  int currentColorRise = (currentStepRise / intervalFadingRise);

  int secondsIntoCurrentStepRise = currentStepRise - (currentColorRise * intervalFadingRise);
  
  //SUNSET
  int maxStepSetPeriod = (numberOfColors-1) * intervalFadingSet;
  int currentStepSet = currentTime - sunset;
  int currentColorSet = (currentStepSet / intervalFadingSet) + 1;
  
  int secondsIntoCurrentStepSet = (currentStepSet - (currentColorSet * intervalFadingSet) ) + intervalFadingSet;

  // Sunset
  if( (currentTime > sunset) && (currentTime < (sunset + maxStepSetPeriod) ) )
  {
    for(int i=0;i<3;i++)
    {
      currentFromStep[i]  = steps[map(currentColorSet, 0, numberOfColors, numberOfColors, 0)][i];
      currentToStep[i]    = steps[map(currentColorSet, 0, numberOfColors, numberOfColors, 0)-1][i];
    }

    calculatesteps(currentFromStep, currentToStep);

    if(currentFromStep[0] > currentToStep[0])
    {
      currentRed    = currentFromStep[0] - (stepSize[0] * (secondsIntoCurrentStepSet+1));
    }
    else if(currentFromStep[0] < currentToStep[0])
    {
      currentRed    = currentFromStep[0] + (stepSize[0] * secondsIntoCurrentStepSet);
    }

    if(currentFromStep[1] > currentToStep[1])
    {
      currentGreen    = currentFromStep[1] - (stepSize[1] * (secondsIntoCurrentStepSet+1));
    }
    else if(currentFromStep[1] < currentToStep[1])
    {
      currentGreen    = currentFromStep[1] + (stepSize[1] * secondsIntoCurrentStepSet);
    }

    if(currentFromStep[2] > currentToStep[2])
    {
      currentBlue    = currentFromStep[2] - (stepSize[2] * (secondsIntoCurrentStepSet+1));
    }
    else if(currentFromStep[2] < currentToStep[2])
    {
      currentBlue    = currentFromStep[2] + (stepSize[2] * secondsIntoCurrentStepSet);
    }
    
    for(int i=0;i<NUM_LEDS;i++)
    {
      leds[i] = CRGB(currentRed, currentGreen, currentBlue);
      FastLED.show();
    }
  }
  //Sunrise
  else if( (currentTime > sunrise) && (currentTime < (sunrise + maxStepRisePeriod) ) )
  {
    for(int i=0;i<3;i++)
    {
      currentFromStep[i]  = steps[currentColorRise][i];
      currentToStep[i]    = steps[currentColorRise+1][i];
    }
    
    calculatesteps(currentFromStep, currentToStep);

    if(currentFromStep[0] > currentToStep[0])
    {
      currentRed    = currentFromStep[0] - (stepSize[0] * secondsIntoCurrentStepRise);
    }
    else if(currentFromStep[0] < currentToStep[0])
    {
      currentRed    = currentFromStep[0] + (stepSize[0] * (secondsIntoCurrentStepRise+1));
    }

    if(currentFromStep[1] > currentToStep[1])
    {
      currentGreen    = currentFromStep[1] - (stepSize[1] * secondsIntoCurrentStepRise);
    }
    else if(currentFromStep[1] < currentToStep[1])
    {
      currentGreen    = currentFromStep[1] + (stepSize[1] * (secondsIntoCurrentStepRise+1));
    }

    if(currentFromStep[2] > currentToStep[2])
    {
      currentBlue    = currentFromStep[2] - (stepSize[2] * secondsIntoCurrentStepRise);
    }
    else if(currentFromStep[2] < currentToStep[2])
    {
      currentBlue    = currentFromStep[2] + (stepSize[2] * (secondsIntoCurrentStepRise+1));
    }
    
    for(int i=0;i<NUM_LEDS;i++)
    {
      leds[i] = CRGB(currentRed, currentGreen, currentBlue);
      FastLED.show();
    }
  }
  // Daytime
  else if( (currentTime > (sunrise + maxStepRisePeriod) ) && (currentTime < sunset) )
  {
    for(int i=0;i<NUM_LEDS;i++)
    {
      leds[i] = CRGB::White;
      FastLED.show();
    }
  }
  // Nighttime
  else if( currentTime > (sunset + maxStepSetPeriod) )
  {
    for(int i=0;i<NUM_LEDS;i++)
    {
      leds[i] = CRGB::Black;
      FastLED.show();
    }
  }

  Blynk.virtualWrite(V10, currentRed);
  Blynk.virtualWrite(V11, currentGreen);
  Blynk.virtualWrite(V12, currentBlue);
}

void calculatesteps(int *fromColor, int *toColor)
{ 
  int stepArray[3] = { 
                        (toColor[0] - fromColor[0]), 
                        (toColor[1] - fromColor[1]), 
                        (toColor[2] - fromColor[2])
                     };

  // Get highest and lowest value
  byte maxIndex = 0;
  byte minIndex = 0;

  int max = stepArray[maxIndex];
  int min = stepArray[minIndex];

  for(int i=0;i<3;i++)
  {
    // Convert values to absolutes
    if(stepArray[i] < 0)
    {
      stepArray[i] = abs(stepArray[i]);
    }
      
    if (max<stepArray[i])
    {
      max = stepArray[i];
      maxIndex = i;
    }
    
    if (min>stepArray[i])
    {
      min = stepArray[i];
      minIndex = i;
    }
  }
  
  if(debug)
  {
    Serial.print("Maximum steps: ");
    Serial.println(stepArray[maxIndex]);
    Serial.print("Minimum steps: ");
    Serial.println(stepArray[minIndex]);
  }
  
  for(int i=0;i<3;i++)
  {
    stepSize[i] = stepArray[i] / intervalFadingRise;
    if(debug) { Serial.println(stepSize[i]); }
  }
}

