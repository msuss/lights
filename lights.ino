#include "LPD8806.h"
#include "SPI.h"

// Simple test for 160 (5 meters) of LPD8806-based RGB LED strip

/*****************************************************************************/

// Number of RGB LEDs in strand:
int nLEDs = 160;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

// You can optionally use hardware SPI for faster writes, just leave out
// the data and clock pin parameters.  But this does limit use to very
// specific pins on the Arduino.  For "classic" Arduinos (Uno, Duemilanove,
// etc.), data = pin 11, clock = pin 13.  For Arduino Mega, data = pin 51,
// clock = pin 52.  For 32u4 Breakout Board+ and Teensy, data = pin B2,
// clock = pin B1.  For Leonardo, this can ONLY be done on the ICSP pins.
//LPD8806 strip = LPD8806(nLEDs);
uint8_t* getRGB(uint32_t c)
{
  uint8_t r=((c >> 8) & 0x7F);
  uint8_t g=((c >> 16) & 0x7F);
  uint8_t b=(c & 0x7F);
  uint8_t rgb[3];
  rgb[0] = r;
  rgb[1] = g;
  rgb[2] = b;
  return &rgb[0];
}

uint32_t darker(uint32_t c, int scale){
  uint8_t *rgb = getRGB(c);
  int r = rgb[0];
  int g = rgb[1];
  int b = rgb[2];
  Serial.print("Red: ");
  Serial.println(rgb[0]);
  Serial.print("Green: ");
  Serial.println(rgb[1]);
  Serial.print("Blue: ");
  Serial.println(rgb[2]);
  int newR=r/scale;
  int newG=g/scale;
  int newB=b/scale;
  if (newR==0 && r!=0 || newG==0 && g!=0 || newB==0 && b!=0) return c;
  return strip.Color(r/2, g/2, b/2);
}

uint32_t brighter(uint32_t c, int scale){
  uint8_t *rgb = getRGB(c);
  int r = rgb[0];
  int g = rgb[1];
  int b = rgb[2];
  Serial.print("Red: ");
  Serial.println(rgb[0]);
  Serial.print("Green: ");
  Serial.println(rgb[1]);
  Serial.print("Blue: ");
  Serial.println(rgb[2]);
  int newR=r*scale;
  int newG=g*scale;
  int newB=b*scale;
  if (newR>127 || newG>127 || newB>127) return c;
  return strip.Color(newR, newG, newB);
}


void setup() {
  Serial.begin(9600);
  
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
}

uint32_t currentColor=strip.Color(1,2,0);
boolean brightnessUp=true;
void loop() {
    uint8_t *rgb = getRGB(currentColor);
    allLit(currentColor, 10000); // White
    uint32_t newColor;
    if (brightnessUp) newColor=brighter(currentColor, 2);
    else newColor=darker(currentColor, 2);
    if (newColor==currentColor) brightnessUp=!brightnessUp;
    currentColor=newColor;
}

void allLit(uint32_t c, uint8_t wait) {
  int i;
  
  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

   for(i=0; i<strip.numPixels(); i++) {
     strip.setPixelColor(i, c);
   }
   
   delay(wait);
   strip.show(); 
}


// Chase one dot down the full strip.  Good for testing purposes.
void colorChase(uint32_t c, uint8_t wait) {
  int i;
  
  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

   for(i=0; i<strip.numPixels(); i++) {
     if (i<strip.numPixels()/2){
       if (i%3 ==0) strip.setPixelColor(i, c);
     }
     else if ((i+2)%3==0) strip.setPixelColor(i, c);
   }
   
   delay(wait);
   strip.show(); 
   
   for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

   for(i=0; i<strip.numPixels(); i++) {
     if ((i+1)%3 ==0) strip.setPixelColor(i, c);
   }
   
   delay(wait);

  strip.show(); // Refresh to turn off last pixel
  
   for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

   for(i=0; i<strip.numPixels(); i++) {
     if (i<strip.numPixels()/2){
       if ((i+2)%3 ==0) strip.setPixelColor(i, c);
     }
     else if (i%3==0) strip.setPixelColor(i, c);
   }
   
   delay(wait);

  strip.show(); // Refresh to turn off last pixel
}

