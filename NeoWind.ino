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
const int skyStripNeoPixelPin = 6;
const int beakerStripNeoPixelPin = 4;

// How many NeoPixels we will be using, charge accordingly
int numPixels = 24;

// Instatiate the NeoPixel from the ibrary
Adafruit_NeoPixel skyStrip = Adafruit_NeoPixel(numPixels, skyStripNeoPixelPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel beakerStrip= Adafruit_NeoPixel(numPixels, beakerStripNeoPixelPin, NEO_GRB + NEO_KHZ800);

// Global RGB values, change to suit your needs
int skyRed = 0;
int skyGreen = 0;
int skyBlue = 255;

int newRed = 0;
int newGreen = 0;
int newBlue = 255;

int rDir = 0;
int gDir = 0;
int bDir = 0;

int brightDir = -1;
int brightness = 255;

int beakerRed = 255;
int beakerGreen = 0;
int beakerBlue = 0;


// various timers
MSTimer skylightTimer;
MSTimer changePixelsTimer;
MSTimer beakerTimer;

// Fan off/on
#define MIN_SKYLIGHT_TIMER               (4000)
#define MAX_SKYLIGHT_TIMER               (7000)


void setup() {
  Serial.begin(115200);
  
  skyStrip.begin();  // initialize the strip
  skyStrip.show();   // make sure it is visible
  skyStrip.clear();  // Initialize all pixels to 'off'

  beakerStrip.begin();  // initialize the strip
  beakerStrip.show();   // make sure it is visible
  beakerStrip.clear();  // Initialize all pixels to 'off'

  skylightTimer.setTimer(random(MIN_SKYLIGHT_TIMER,MAX_SKYLIGHT_TIMER));
  skylightTimer.start();
  
  changePixelsTimer.setTimer(100);
  changePixelsTimer.start();

  beakerTimer.setTimer(100);
  beakerTimer.start();

  // set the colors for the strip
  for( int i = 0; i < numPixels; i++ ) {
    skyStrip.setPixelColor(i, 0,0,255);
    beakerStrip.setPixelColor(i, beakerRed,beakerGreen,beakerBlue);
  }
}

void loop() {
  if( changePixelsTimer.isExpired()) {
    skyRed += rDir;
    skyGreen += gDir;
    skyBlue += bDir;

    if( skyRed == newRed ) {
      rDir = 0;
    }
    else if( skyGreen == newGreen ) {
      gDir = 0;
    }
    else if( skyBlue == newBlue ) {
      bDir = 0;
    }
    
    for( int i = 0; i < numPixels; i++ ) {
      skyStrip.setPixelColor(i, skyRed, skyGreen, skyBlue);
      beakerStrip.setPixelColor(i, beakerRed, beakerGreen, beakerBlue);
    }

    changePixelsTimer.start();
  }
   
   // show all pixels  
   skyStrip.show();
   beakerStrip.show();
   
   // wait for a short amount of time -- sometimes a short delay of 5ms will help
   // technically we only need to execute this one time, since we aren't changing the colors but we will build on this structure
   delay(10);

   checkSkylight(); 
   checkBeakerStrip();
}


void checkSkylight() {
  if( skylightTimer.isExpired() ) {
      Serial.println("light timer expired");
    
      // choose new color
      skyRed = newRed = random(0,30);
      skyGreen = newGreen = random(0,30);
      skyBlue = newBlue = random(120,255);

      // some janky code here
      rDir = setDir(skyRed, newRed);
      gDir = setDir(skyGreen, newGreen);
      bDir = setDir(skyBlue, newBlue);

      // reset timer
      skylightTimer.setTimer(random(MIN_SKYLIGHT_TIMER,MAX_SKYLIGHT_TIMER));
      skylightTimer.start();
  }
}

void checkBeakerStrip() {
    if( beakerTimer.isExpired() ) {
      brightness += brightDir;
      if( brightness < 0 ) {
        brightness = 0;
        brightDir = -brightDir;
        randomizeBeakerStripColors();
      }
      else if( brightness > 255 ) {
        brightness = 255;
        brightDir = -brightDir;
      }

      Serial.println(brightness);
      beakerStrip.setBrightness(brightness);
      beakerStrip.show();
      
      beakerTimer.start();
   }
}

void randomizeBeakerStripColors() {
  beakerRed = random(100,255);
  beakerGreen = random(0,25);
  beakerBlue = random(0,100);
}

// determine which wat the pixels will go
int setDir(int oldDir, int newDir) {
  if( newDir > oldDir) {
    return 1;
  }
  else if( newDir < oldDir) {
    return -1;
  }

  return 0;
}
