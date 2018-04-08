/*
 * This code displays animated GIFs on a 64x16 RGB LED display. 
 * It was used to operate an LED Top Hat: https://imgur.com/gallery/6iKDv 
 * 
 * Minor modifications by: Matthew Varas
 * 
 * The program is based on previous modifications to the SmartMatrix3 "Animated GIFs" sketch.
 * Original modifications were by performed by Louis Beaudoin to adapt the sketch to the 
 * WS2812 NeoPixel using the Adafruit NeoPixel library. https://github.com/pixelmatix/animatedgifs/tree/neopixel 
 * 
 * Additional References: https://learn.adafruit.com/smartmatrix-animated-gif-player/overview 
 * 
 * The required additional libraries for the project are: 
 *    Adafruit_NeoPixel
 *    Adafruit_GFX
 *    Adafruit_NeoMatrix
 * 
 * 
 * The 64x16 top hat project used four 16x16 WS2812 RGB LED Matricies. The code was modified
 * to adjust for the different pixel layout and aspect ratio. All other code is original to 
 * the previous projects. 
 * 
 * The sketch is run on a Teensy 3.6 via TeensyDuino with gifs loaded on a microSD card in the /gifs/ folder.
 * A SmartMatrix 4 shield was used for the serial breakout connector and level shifting. Note that the 
 * WS2812 is expecting a 5V data signal and the Teensy is 3.3V. While my testing showed that it would
 * still behave with a 3.3V signal, level shifting up to 5V is strongly recommended.
 * The WS2812 uses a one-wire serial protocol which was connected to Pin 17 because it is the 
 * data out line on the SmartMatrix 4 shield.
 * 
 * GIF file names must be 8 characters or less and end in .gif
 * 
 *  
 *  Two buttons were added to adjust output brightness. These are pretty poorly implemented due
 *  to lack of time, but they do function. Debounce is handled within the interrupt (hence "poorly
 *  implemented"). Buttons are active low and use the internal pull-up's in the MCU instead of 
 *  external resistors. 
 *  
 *  Pinouts: 
 *  Serial Data: Pin 17
 *  Brightness Up: Pin 5
 *  Brightness Down: Pin 4
 *  
 *  
 *  
 *  Gif files must be 64x16 pixels to be properly handled. This matches the original number of
 *  LED's (1024). I'm unsure if the code will work with any other total number of LED's.
 *  In order to change to a different pixel configuration the "Width" and "Height" 
 *  constants in "GIFParseFunctions.cpp" need to be updated to the correct dimensions. 
 *  Additionally, in this file the line that reads "Adafruit_NeoMatrix strip..." 
 *  must be updated to reflect the size of the matricies used as well as the tile configuration. 
 *  Examples of this can be found in the "tiletest" example from the Adafruit Neomatrix library. 
 *  
 *  A description of the matrix declaration from the tiletest example: 
 *  
 *  // MATRIX DECLARATION:
// Parameter 1 = width of EACH NEOPIXEL MATRIX (not total display)
// Parameter 2 = height of each matrix
// Parameter 3 = number of matrices arranged horizontally
// Parameter 4 = number of matrices arranged vertically
// Parameter 5 = pin number (most are valid)
// Parameter 6 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the FIRST MATRIX; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs WITHIN EACH MATRIX are
//     arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns WITHIN
//     EACH MATRIX proceed in the same order, or alternate lines reverse
//     direction; pick one.
//   NEO_TILE_TOP, NEO_TILE_BOTTOM, NEO_TILE_LEFT, NEO_TILE_RIGHT:
//     Position of the FIRST MATRIX (tile) in the OVERALL DISPLAY; pick
//     two, e.g. NEO_TILE_TOP + NEO_TILE_LEFT for the top-left corner.
//   NEO_TILE_ROWS, NEO_TILE_COLUMNS: the matrices in the OVERALL DISPLAY
//     are arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_TILE_PROGRESSIVE, NEO_TILE_ZIGZAG: the ROWS/COLUMS OF MATRICES
//     (tiles) in the OVERALL DISPLAY proceed in the same order for every
//     line, or alternate lines reverse direction; pick one.  When using
//     zig-zag order, the orientation of the matrices in alternate rows
//     will be rotated 180 degrees (this is normal -- simplifies wiring).
//   See example below for these values in action.
// Parameter 7 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 pixels)
//   NEO_GRB     Pixels are wired for GRB bitstream (v2 pixels)
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA v1 pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
 *  
 *  
 *  
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * ******* The following are the original notes for the sketch. *******
 * 
 * 
 * 
 * 
 * 

 * Animated GIFs Display Code for SmartMatrix and 32x32 RGB LED Panels
 *
 * Uses SmartMatrix Library for Teensy 3.1 written by Louis Beaudoin at pixelmatix.com
 *
 * Written by: Craig A. Lindley
 *
 * Copyright (c) 2014 Craig A. Lindley
 * Minor modifications by Louis Beaudoin (Pixelmatix)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

 /*
  * This example displays 32x32 GIF animations loaded from a SD Card connected to the Teensy 3.1
  * The GIFs must be 32x32 pixels exactly
  * Wiring is on the default Teensy 3.1 SPI pins, and chip select can be on any GPIO,
  * set by defining SD_CS in the code below
  * Function     | Pin
  * DOUT         |  11
  * DIN          |  12
  * CLK          |  13
  * CS (default) |  15
  *
  * This code first looks for .gif files in the /gifs/ directory
  * (customize below with the GIF_DIRECTORY definition) then plays random GIFs in the directory,
  * looping each GIF for DISPLAY_TIME_SECONDS
  *
  * This example is meant to give you an idea of how to add GIF playback to your own sketch.
  * For a project that adds GIF playback with other features, take a look at
  * Light Appliance:
  * https://github.com/CraigLindley/LightAppliance
  *
  * If you find any 32x32 GIFs that won't play properly, please attach them to a new
  * Issue post in the GitHub repo here:
  * https://github.com/pixelmatix/AnimatedGIFs/issues
  *
  * This sketch requires the SdFat Library: https://github.com/greiman/SdFat
  *
  * If you're having trouble compiling this sketch, view instructions in the README file, which is
  * available online here: https://github.com/pixelmatix/AnimatedGIFs/
  *
  */

#include <math.h>
#include <stdlib.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include "GIFDecoder.h"

#define DISPLAY_TIME_SECONDS 10

// range 0-255
const int defaultBrightness = 50;                             // Initial Brightness at power-up
const int minBrightness = 10;                                 // Minimum brightness value for use with brightness buttons
const int maxBrightness = 200;                                // Maximum brightness value for use with brightness buttons

const rgb24 COLOR_BLACK = {
    0, 0, 0 };

int currentBrightness = defaultBrightness;                    // Track the current brightness level

    
// Initialize the LED Matricies. 
// See the "tiletest" example in the Adafruit NeoMatrix library for explanation of paramaters
Adafruit_NeoMatrix strip = Adafruit_NeoMatrix(16, 16, 4, 1, 17,
  NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_TILE_PROGRESSIVE +
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);


// Chip select for SD card on the Teensy 3.6
#define SD_CS BUILTIN_SDCARD

#define GIF_DIRECTORY "/gifs/"

int num_files;

// Function to clear the display (write all pixels to black)
void screenClearCallback(void) {
  for (int i=0; i<1024; i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
}

void updateScreenCallback(void) {
  strip.show();
}

void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue) {
  strip.drawPixel(x, y, strip.Color(red, green, blue));
}



// Initialize brightness up/down pushbutton interrupts
void initButtons(void) {
  pinMode(5, INPUT_PULLUP);
  digitalWrite(5, HIGH);
  attachInterrupt(digitalPinToInterrupt(5), brightnessUp, FALLING);

  pinMode(4, INPUT_PULLUP);
  digitalWrite(4, HIGH);
  attachInterrupt(digitalPinToInterrupt(4), brightnessDown, FALLING);
}


// Setup method runs once, when the sketch starts
void setup() {

    setScreenClearCallback(screenClearCallback);
    setUpdateScreenCallback(updateScreenCallback);
    setDrawPixelCallback(drawPixelCallback);

    // Seed the random number generator
    randomSeed(analogRead(14));

    Serial.begin(115200);

    // Initialize matrix
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setBrightness(defaultBrightness);

    // Initialize brightness adjustment buttons
    initButtons();

    // initialize the SD card at full speed
    pinMode(SD_CS, OUTPUT);
    if (!SD.begin(SD_CS)) {
        Serial.println("No SD card");
        while(1);
    }

    // Determine how many animated GIF files exist
    num_files = enumerateGIFFiles(GIF_DIRECTORY, false);

    if(num_files < 0) {
        Serial.println("No gifs directory");
        while(1);
    }

    if(!num_files) {
        Serial.println("Empty gifs directory");
        while(1);
    }
}





// Brightness Up Interrupt Handler
void brightnessUp() {
  int tempBrightness;

  // Debounce
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  
  if (interrupt_time - last_interrupt_time > 200UL)   // Ignore interupts for 200 milliseconds
  {
    tempBrightness = currentBrightness;               
    
    if (tempBrightness >= maxBrightness) {            // Brightness is at maximum
      tempBrightness = maxBrightness;                 // No change
    }
    else {                                            // Brightness is below maximum
      tempBrightness += 10;                           // Increase brightness by 10
    }
    strip.setBrightness(tempBrightness);              // Send new brightness value to LED's
    currentBrightness = tempBrightness;               // Store new brightness value
    //Serial.print("Brightness = ");                  // Debug
    //Serial.println(currentBrightness);
    }

    last_interrupt_time = interrupt_time;             // Debounce
}



// Brightness Down Interrupt
void brightnessDown() {
  int tempBrightness;

  // Debounce
  volatile static unsigned long last_interrupt_time1 = 0;
  unsigned long interrupt_time1 = millis();
  
  if (interrupt_time1 - last_interrupt_time1 > 200UL)   // Ignore interupts for 200 milliseconds
  {
    tempBrightness = currentBrightness; 
    
    if (tempBrightness <= minBrightness) {              // Brightness is already at minimum
      tempBrightness = minBrightness;                   // No change
    }
    else {                                              // Brightness is above minimum
      tempBrightness -= 10;                             // Decrease brightness by 10
    }
    strip.setBrightness(tempBrightness);                // Send new brightness value to LED's
    currentBrightness = tempBrightness;                 // Store new brightness value
//    Serial.print("Brightness = ");                    // Debug
//    Serial.println(currentBrightness);
    }

    last_interrupt_time1 = interrupt_time1;             // Debounce
}





void loop() {

    unsigned long futureTime;
    char pathname[30];

    int index = random(num_files);

    // Do forever
    while (true) {
        // Can clear screen for new animation here, but this might cause flicker with short animations
        screenClearCallback();
        
        getGIFFilenameByIndex(GIF_DIRECTORY, index++, pathname);
        if (index >= num_files) {
            index = 0;
        }

        // Calculate time in the future to terminate animation
        futureTime = millis() + (DISPLAY_TIME_SECONDS * 1000);

        while (futureTime > millis()) {
            processGIFFile(pathname);
        }
    }
}
