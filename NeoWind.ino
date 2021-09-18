/********************************************************************************************************
  NeoWind
  Written by Scott Kildall

  Runs a 200ma fan and NeoPixel ring at the same time

  The 200ma fan is too much current for the Arduino without USB power,
  so we run it from 5V and use the fan pin as the ground.
  
  This will light just a NeoPixel strip of NeoPixels
  
---------------------------------------------------------------------------------------------------------
NeoPixel Information for initializing the strip, below
  60ma/pixel for current load
  Parameter 1 = number of pixels in strip
  Parameter 2 = pin number (most are valid)
  Parameter 3 = pixel type flags, add together as needed:
    NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
    NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
    NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
    NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

**********************************************************************************************************/

#include <Adafruit_NeoPixel.h>

#include "MSTimer.h"

// the data pin for the NeoPixels
int neoPixelPin = 6;

// How many NeoPixels we will be using, charge accordingly
int numPixels = 24;

// Instatiate the NeoPixel from the ibrary
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

// Global RGB values, change to suit your needs
int r = 0;
int g = 0;
int b = 255;

// FLIPPED because of the wiring
boolean fanNotRunning = false;

// various timers
MSTimer fanTimer;
MSTimer lightTimer;

// Fan off/on
#define MIN_FAN_RUNNING_TIMER         (20000)
#define MAX_FAN_RUNNING_TIMER         (21000)

#define MIN_FAN_OFF_TIMER             (10000)
#define MAX_FAN_OFF_TIMER             (11000)

void setup() {
  Serial.begin(115200);
  
  strip.begin();  // initialize the strip
  strip.show();   // make sure it is visible
  strip.clear();  // Initialize all pixels to 'off'

  fanTimer.setTimer(random(MIN_FAN_RUNNING_TIMER, MAX_FAN_RUNNING_TIMER));
  fanTimer.start();
  
  pinMode(7,OUTPUT);
  digitalWrite(7,fanNotRunning);
}

void loop() {
   // set the colors for the strip
   for( int i = 0; i < numPixels; i++ )
       strip.setPixelColor(i, r, g, b);
   
   // show all pixels  
   strip.show();
   
   // wait for a short amount of time -- sometimes a short delay of 5ms will help
   // technically we only need to execute this one time, since we aren't changing the colors but we will build on this structure
   delay(10);

    checkFan();
}


// looks for an expired timer, changs the state accordingly
void checkFan() {
  // turn fan on/off
   if( fanTimer.isExpired() ) {
    
        
        
       fanNotRunning = !fanNotRunning;
       digitalWrite(7,fanNotRunning);   // fan running
      
      if( fanNotRunning ) {
        Serial.println("Fan OFF");
        // fan now off, set timer
        fanTimer.setTimer(random(MIN_FAN_OFF_TIMER, MAX_FAN_OFF_TIMER));
        fanTimer.start();   
      }
      else {
        Serial.println("Fan ON");
        
        // now turn back on
        fanTimer.setTimer(random(MIN_FAN_RUNNING_TIMER, MAX_FAN_RUNNING_TIMER));
        fanTimer.start();  
      }
   }
}
