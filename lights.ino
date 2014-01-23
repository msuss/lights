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


uint32_t darker(uint32_t c, double scale){
  uint8_t *rgb = getRGB(c);
  int r = rgb[0];
  int g = rgb[1];
  int b = rgb[2];
  int newR=r/scale;
  int newG=g/scale;
  int newB=b/scale;
  if ((newR==0 && r!=0) || (newG==0 && g!=0) || (newB==0 && b!=0)) 
  {
    Serial.println("NOPE");
    return c;
  }
  return strip.Color(newR, newG, newB);
}

uint32_t brighter(uint32_t c, double scale){
  uint8_t *rgb = getRGB(c);
  int r = rgb[0];
  int g = rgb[1];
  int b = rgb[2];
  int newR=r*scale;
  int newG=g*scale;
  int newB=b*scale;
  if (newR>127 || newG>127 || newB>127) return c;
  return strip.Color(newR, newG, newB);
}

uint32_t addRGB(uint32_t c, int r, int g, int b)
{
  uint8_t *rgb = getRGB(c);
  int newR = max(min(rgb[0]+r,127),0);
  int newG = max(min(rgb[1]+g,127),0);
  int newB = max(min(rgb[2]+b,127),0);
  return strip.Color(newR, newG, newB);
}
//Has issues with rounding down to 0, so generally avoid using with large steps
uint32_t nextInFadeSequence(uint32_t start, uint32_t finish, int steps)
{
  if (steps == 0){
    return finish;
  }
  uint8_t *rgbStart = getRGB(start);
  uint8_t *rgbFinish = getRGB(finish);
  int deltaR = (rgbFinish[0] - rgbStart[0])/steps;
  int deltaG = (rgbFinish[1] - rgbStart[1])/steps;
  int deltaB = (rgbFinish[2] - rgbStart[2])/steps;
  return addRGB(start, deltaR, deltaG, deltaB);
}

void setup() {
  Serial.begin(9600);
  
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
}

uint32_t blue=strip.Color(0,0,127);
uint32_t teal=strip.Color(0,127,127);
uint32_t green=strip.Color(0,127,0);
uint32_t yellow=strip.Color(127,127,0);
uint32_t red=strip.Color(127,0,0);
uint32_t purple=strip.Color(127,0,127);

uint32_t colors[]={blue, teal, green, yellow, red, purple};

int currentIdx = 0;
void loop() {

    fadeFill(colors[currentIdx], colors[(currentIdx+2)%6],10);
    currentIdx= (currentIdx+1)%6;
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

void allFade(uint32_t c1, uint32_t c2, uint8_t wait) {
  int i;
  
  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);
  uint32_t current = c1;
   for(i=0; i<strip.numPixels(); i++) {
     int numSteps = strip.numPixels() - (i+1);
     strip.setPixelColor(i, current);
     current = nextInFadeSequence(current, c2, numSteps);
     
   }
   
   delay(wait);
   strip.show(); 
}

void lightRight(uint32_t c, uint8_t wait) {
  int i;
  
  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

   for(i=0; i<strip.numPixels(); i++) {
     
     if (i<46) strip.setPixelColor(i, c);
   }
   
   delay(wait);
   strip.show(); 
}

void lightLeft(uint32_t c, uint8_t wait) {
  int i;
  
  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

   for(i=0; i<strip.numPixels(); i++) {
     
     if (i>113) strip.setPixelColor(i, c);
   }
   
   delay(wait);
   strip.show(); 
}

void lightTop(uint32_t c, uint8_t wait) {
  int i;
  
  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

   for(i=0; i<strip.numPixels(); i++) {
     
     if (i >=46 && i <= 113) strip.setPixelColor(i, c);
   }
   
   delay(wait);
   strip.show(); 
}
// Chase one dot down the full strip.
int trailLen=6;
int trailMultiplier=4;
void colorChase(uint32_t c, uint8_t wait) {
  int i;

  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels()+trailLen*trailMultiplier; i++) {
    int j;
    for (j=i; j>=0 && j>=i-trailMultiplier*trailLen; j--){
      if (j<strip.numPixels())
        strip.setPixelColor(j, darker(c, pow(2, (i-j)/trailMultiplier)));
    }
    strip.show();              // Refresh LED states
    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
    for (j=i; j>=0 && j>=i-trailMultiplier*trailLen; j--){
      strip.setPixelColor(j, 0);
    }
    delay(wait/(pow(trailMultiplier,2)));
  }

  strip.show(); // Refresh to turn off last pixel
  trailMultiplier++;
}

void chaseFill(uint32_t c, uint8_t wait) {
  int i;

  // Start by turning all pixels off:

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels()+trailLen; i++) {
    int j;
    for (j=i; j>=0 && j>=i-trailLen; j--){
      if (j<strip.numPixels())
        strip.setPixelColor(j, darker(c, pow(2, i-j)));
    }
    strip.show();              // Refresh LED states
    

    delay(wait);
  }

  strip.show(); // Refresh to turn off last pixel
}

void fadeChase(uint32_t c1, uint32_t c2, uint8_t wait) {
  int i;

  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels()+trailLen*trailMultiplier; i++) {
    int j;
    uint32_t current = c1;
    for (j=i; j>=0 && j>=i-trailMultiplier*trailLen; j--){
      if (j<strip.numPixels()){
        int numSteps = trailMultiplier*trailLen - (i - j);
        strip.setPixelColor(j, current);
        current = nextInFadeSequence(current, c2, numSteps);
      }
    }
    strip.show();              // Refresh LED states
    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
    for (j=i; j>=0 && j>=i-trailMultiplier*trailLen; j--){
      strip.setPixelColor(j, 0);
    }
    delay(wait/(pow(trailMultiplier,2)));
  }

  strip.show(); // Refresh to turn off last pixel
}

void fadeFill(uint32_t c1, uint32_t c2, uint8_t wait) {
  int i;

  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels()+trailLen*trailMultiplier; i++) {
    int j;
    uint32_t current = c1;
    for (j=i; j>=0 && j>=i-trailMultiplier*trailLen; j--){
      if (j<strip.numPixels()){
        int numSteps = trailMultiplier*trailLen - (i - j);
        strip.setPixelColor(j, current);
        current = nextInFadeSequence(current, c2, numSteps);
      }
    }
    strip.show();              // Refresh LED states

    delay(wait);
  }

  strip.show(); // Refresh to turn off last pixel
}

// Chase one dot down the full strip.  Good for testing purposes.
void movingLights(uint32_t c, uint8_t wait) {
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

