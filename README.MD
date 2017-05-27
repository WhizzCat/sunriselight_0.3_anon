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

NOTES

Sunrise colors:

01) Off           : 0   , 0   , 0     Start counting from 0 (sunrise seconds)
02) Red-ish       : 255 , 51  , 0
03) Dark orange   : 255 , 102 , 51
04) Light orange  : 255 , 204 , 102
05) Yellow-ish    : 255 , 255 , 102
06) Light green   : 204 , 255 , 153
07) Sea green     : 102 , 204 , 153
08) Light blue    : 51  , 204 , 255
09) Blue-ish      : 51  , 153 , 255
10) Purple-ish    : 102 , 102 , 255
11) Purple-blue   : 51  , 51  , 204
12) White         : 255 , 255 , 255   Elapsed time here for sunrise is 110 minutes, 6600 seconds

Considerations/assumes:

- 12 steps involved
- Sunrise takes longer than sunset
- Noon has a certain period of white light
- Example day takes 9:30 (sunset - sunrise), that is 34200 seconds
- Assuming sunrise takes two hours (7200 seconds)
- Assuming sunset takes one hour (3600 seconds)
- White period would be 34200 - (7200+3600) = 23400 seconds
- 12 steps in 6600 seconds, each step is 550 seconds (minus white)
- 12 steps in 3000 seconds, each step is 250 seconds (minus black)

from 01 to 02
- determine if it's actually sunrise (compare sunrise to currentTime)

- Get highest and lowest values
- Make them abs()
- Calculate step size based on division of interval for fading
- biggest is now 255, smallest 0
[0] = 255 / 550   == 0.463
[1] = 51 / 550  == 0.093
[2] = 0 / 0 == INVALID

++++++++++++++++++++++++++++++++
Color codes
++++++++++++++++++++++++++++++++

Red                   Error
Sea green             SunSync is off, default color of LED's when in manual
Orange                TimeSync is in progress (wether manual LED is on or off, this still tries)
Green                 TimeSync is succesful
Purple                Getting data from API is in progress
Pink                  Processing data from API is in progress
Lavender

++++++++++++++++++++++++++++++++
Wiring
++++++++++++++++++++++++++++++++

Green         = Data In
White/orange  = GND
White/green   = Vss
