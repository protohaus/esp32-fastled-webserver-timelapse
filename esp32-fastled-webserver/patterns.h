/*
   ESP32 FastLED WebServer: https://github.com/jasoncoon/esp32-fastled-webserver
   Copyright (C) 2017 Jason Coon

   Built upon the amazing FastLED work of Daniel Garcia and Mark Kriegsman:
   https://github.com/FastLED/FastLED

   ESP32 support provided by the hard work of Sam Guyer:
   https://github.com/samguyer/FastLED

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "twinkleFox.h"

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, speed);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += ColorFromPalette(palettes[currentPaletteIndex], gHue, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(speed, 0, NUM_LEDS - 1);
  static int prevpos = 0;
  CRGB color = ColorFromPalette(palettes[currentPaletteIndex], gHue, 255);
  if ( pos < prevpos ) {
    fill_solid( leds + pos, (prevpos - pos) + 1, color);
  } else {
    fill_solid( leds + prevpos, (pos - prevpos) + 1, color);
  }
  prevpos = pos;
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t beat = beatsin8( speed, 64, 255);
  CRGBPalette16 palette = palettes[currentPaletteIndex];
  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  const byte dotCount = 3;
  const byte hueInc = 240 / dotCount;
  for ( int i = 0; i < dotCount; i++) {
    leds[beatsin16( i + speed, 0, NUM_LEDS - 1 )] |= ColorFromPalette(palettes[currentPaletteIndex], dothue, 255);
    dothue += hueInc;
  }
}

void showSolidColor()
{
  fill_solid(leds, NUM_LEDS, solidColor);
}

// based on FastLED example Fire2012WithPalette: https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
void heatMap(CRGBPalette16 palette, bool up)
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy(random(256));

  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  byte colorindex;

  // Step 1.  Cool down every cell a little
  for ( uint16_t i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((cooling * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( uint16_t k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < sparking ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( uint16_t j = 0; j < NUM_LEDS; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    colorindex = scale8(heat[j], 190);

    CRGB color = ColorFromPalette(palette, colorindex);

    if (up) {
      leds[j] = color;
    }
    else {
      leds[(NUM_LEDS - 1) - j] = color;
    }
  }
}

void fire()
{
  heatMap(HeatColors_p, true);
}

void water()
{
  heatMap(IceColors_p, false);
}

// Pride2015 by Mark Kriegsman: https://gist.github.com/kriegsman/964de772d64c502760e5
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void fillWithPride(bool useFibonacciOrder)
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  // uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t sat8 = beatsin88( 43.5, 220, 250);
  // uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint8_t brightdepth = beatsin88(171, 96, 224);
  // uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint16_t brightnessthetainc16 = beatsin88( 102, (25 * 256), (40 * 256));
  // uint8_t msmultiplier = beatsin88(147, 23, 60);
  uint8_t msmultiplier = beatsin88(74, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  // uint16_t hueinc16 = beatsin88(113, 1, 3000);
  uint16_t hueinc16 = beatsin88(57, 1, 128);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  // sHue16 += deltams * beatsin88( 400, 5, 9);
  sHue16 += deltams * beatsin88( 200, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV( hue8, sat8, bri8);

    uint16_t pixelnumber = useFibonacciOrder ? fibonacciToPhysical[i] : i;

    // nblend( leds[pixelnumber], newcolor, 64);
    nblend( leds[pixelnumber], newcolor, 8);
  }
}

void pride() {
  fillWithPride(false);
}

void prideFibonacci() {
  fillWithPride(true);
}

// ColorWavesWithPalettes by Mark Kriegsman: https://gist.github.com/kriegsman/8281905786e8b2632aeb
// This function draws color waves with an ever-changing,
// widely-varying set of parameters, using a color palette.
void colorwaves( CRGB* ledarray, uint16_t numleds, CRGBPalette16& palette, bool useFibonacciOrder)
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  // uint8_t sat8 = beatsin88( 87, 220, 250);
  // uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint8_t brightdepth = beatsin88(171, 96, 224);
  // uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint16_t brightnessthetainc16 = beatsin88( 102, (25 * 256), (40 * 256));
  // uint8_t msmultiplier = beatsin88(147, 23, 60);
  uint8_t msmultiplier = beatsin88(74, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  // uint16_t hueinc16 = beatsin88(113, 300, 1500);
  uint16_t hueinc16 = beatsin88(57, 1, 128);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  // sHue16 += deltams * beatsin88( 400, 5, 9);
  sHue16 += deltams * beatsin88( 200, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t i = 0 ; i < numleds; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if ( h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    uint8_t index = hue8;
    //index = triwave8( index);
    index = scale8( index, 240);

    CRGB newcolor = ColorFromPalette( palette, index, bri8);

    uint16_t pixelnumber = useFibonacciOrder ? fibonacciToPhysical[i] : i;

    nblend( ledarray[pixelnumber], newcolor, 8);
  }
}

void colorWaves()
{
  colorwaves(leds, NUM_LEDS, currentPalette, false);
}

void colorWavesFibonacci()
{
  colorwaves(leds, NUM_LEDS, currentPalette, true);
}

void fibonacciStarsWithOffset(uint16_t stars[], uint8_t starCount, uint8_t offset = 21, bool setup = false, bool move = false) {
  // use a number from the Fibonacci sequence for offset to follow a spiral out from the center

  const uint8_t hues = NUM_LEDS / 256;

  for (uint8_t i = 0; i < starCount; i++) {
    if (setup || stars[i] >= NUM_LEDS) { 
      // reset star
      stars[i] = random8(offset - 1);
    }

    uint16_t index = fibonacciToPhysical[stars[i]];

    // draw the star
    CRGB newcolor = ColorFromPalette(currentPalette, (stars[i] / hues) + gHue); // i * (240 / starCount)

    nblend( leds[index], newcolor, 64);
  }

  // move the stars
  if(move) {
    for (uint8_t i = 0; i < starCount; i++) {
      stars[i] = stars[i] + offset;
    }
  }
}

const uint8_t starCount = 16;

// void fibonacciStars13(bool setup = false, bool move = false) {
//   static uint16_t stars[starCount];
//   fibonacciStarsWithOffset(stars, starCount, 13, setup, move);
// }

// void fibonacciStars21(bool setup = false, bool move = false) {
//   static uint16_t stars[starCount];
//   fibonacciStarsWithOffset(stars, starCount, 21, setup, move);
// }

void fibonacciStars34(bool setup = false, bool move = false) {
  static uint16_t stars[starCount];
  fibonacciStarsWithOffset(stars, starCount, 34, setup, move);
}

void fibonacciStars55(bool setup = false, bool move = false) {
  static uint16_t stars[starCount];
  fibonacciStarsWithOffset(stars, starCount, 55, setup, move);
}

void fibonacciStars89(bool setup = false, bool move = false) {
  static uint16_t stars[starCount];
  fibonacciStarsWithOffset(stars, starCount, 89, setup, move);
}

void fibonacciStars() {
  bool move = false;
  static bool setup = true;
  fadeToBlackBy( leds, NUM_LEDS, 8);

  EVERY_N_MILLIS(90) {
    move = true;
  }

  // fibonacciStars13(setup, move);
  // fibonacciStars21(setup, move);
  fibonacciStars34(setup, move);
  fibonacciStars55(setup, move);
  fibonacciStars89(setup, move);
  setup = false;
}

void fireFibonacci() {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    uint16_t x = coordsX[i];
    uint16_t y = coordsY[i];

    uint8_t n = qsub8(inoise8((y << 2) - beat88(speed << 2), (x << 2)), y);

    leds[i] = ColorFromPalette(HeatColors_p, n);
  }
}

void waterFibonacci() {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    uint16_t x = coordsX[i];
    uint16_t y = coordsY[i];

    uint8_t n = inoise8((y << 2) + beat88(speed << 2), (x << 4));

    leds[i] = ColorFromPalette(IceColors_p, n);
  }
}

void firePalette() {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    uint16_t x = coordsX[i];
    uint16_t y = coordsY[i];

    uint8_t n = qsub8(inoise8((y << 2) - beat88(speed << 2), (x << 2)), y);

    leds[i] = ColorFromPalette(currentPalette, n);
  }
}

void waterPalette() {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    uint16_t x = coordsX[i];
    uint16_t y = coordsY[i];

    uint8_t n = inoise8((y << 2) + beat88(speed << 2), (x << 4));

    leds[i] = ColorFromPalette(currentPalette, n);
  }
}

void emitterFibonacci() {
  static CRGB ledBuffer[NUM_LEDS]; // buffer for better fade behavior
  const uint8_t dAngle = 32; // angular span of the traces
  const uint8_t dRadius = 12; // radial width of the traces
  const uint8_t vSpeed = 16; // max speed variation


  static const uint8_t eCount = 7; // Number of simultanious traces
  static uint8_t angle[eCount]; // individual trace angles
  static uint16_t timeOffset[eCount]; // individual offsets from beat8() function
  static uint8_t speedOffset[eCount]; // individual speed offsets limited by vSpeed
  static uint8_t sparkIdx = 0; // randomizer cycles through traces to spark new ones

  // spark new trace
  EVERY_N_MILLIS(20) {
    if (random8(17) <= (speed >> 4)) { // increase change rate for higher speeds
      angle[sparkIdx] = random8();
      speedOffset[sparkIdx] = random8(vSpeed); // individual speed variation
      timeOffset[sparkIdx] = beat8(qadd8(speed,speedOffset[sparkIdx]));
      sparkIdx = addmod8(sparkIdx, 1, eCount); // continue randomizer at next spark
    }
  }

  // fade traces
  fadeToBlackBy( ledBuffer, NUM_LEDS, 6 + (speed >> 3));

  // draw traces
  for (uint8_t e = 0; e < eCount; e++) {
    uint8_t startRadius = sub8(beat8(qadd8(speed, speedOffset[e])), timeOffset[e]);
    uint8_t endRadius = add8(startRadius, dRadius + (speed>>5)); // increase radial width for higher speeds
    antialiasPixelAR(angle[e], dAngle, startRadius, endRadius, ColorFromPalette(currentPalette, startRadius), ledBuffer, NUM_LEDS);
  }

  // copy buffer to actual strip
  memcpy(leds, ledBuffer, sizeof(ledBuffer[0])*NUM_LEDS);
}

CRGBPalette16 swirlPalette(palettes[1]); // es_rivendell_15_gp

void swirlFibonacci() {
  const float z = 2.5; // zoom (2.0)
  const float w = 3.0; // number of wings (3)
  const float p_min = 0.1; const float p_max = 2.0; // puff up (default: 1.0)
  const float d_min = 0.1; const float d_max = 2.0; // dent (default: 0.5)
  const float s_min = -3.0; const float s_max = 2.0; // swirl (default: -2.0)
  const float g_min = 0.1; const float g_max = 0.5; // glow (default: 0.2)
  const float b = 240; // inverse brightness (240)

  const float p = p_min + beatsin88(13*speed) / (float)UINT16_MAX * (p_max - p_min);
  const float d = d_min + beatsin88(17*speed) / (float)UINT16_MAX * (d_max - d_min);
  const float s = s_min + beatsin88(7*speed) / (float)UINT16_MAX * (s_max - s_min);
  const float g = g_min + beatsin88(27*speed) / (float)UINT16_MAX * (g_max - g_min);


  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    float r = physicalToFibonacci[i] / 256.0 * z;
    float a = (angles[i] + (beat88(3*speed)>>3)) / 256.0 * TWO_PI;
    float v = r - p + d * sin(w * a + s * r * r);
    float c = 255 - b * pow(fabs(v), g);
    if (c < 0) c = 0;
    else if (c > 255) c = 255;

    CRGB newcolor = ColorFromPalette(swirlPalette, (uint8_t)c);
    nblend(leds[i], newcolor, 128);
  }
}

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct {
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

PatternAndNameList patterns = {
  { colorWavesFibonacci, "Color Palette Waves" },
  { prideFibonacci, "Rainbow Waves" },


  { paletteNoise, "Palette Noise" },
  { polarNoise, "Polar Palette Noise" },

  { fireFibonacci, "Fire" },
  { waterFibonacci, "Water" },

  { firePalette, "Palette Fire" },
  { waterPalette, "Palette Water" },

  { fibonacciStars, "Palette Stars" },
  { emitterFibonacci, "Palette Emitters" },

  { swirlFibonacci, "Swirl" },

  // { colorWaves,          "Color Waves" },
  // { pride,               "Pride" },

  // // TwinkleFOX patterns
  // { drawTwinkles, "Twinkles" },

  // // Fire & Water
  // { fire, "Fire" },
  // { water, "Water" },

  // // DemoReel100 patterns
  // { rainbow, "Rainbow" },
  // { rainbowWithGlitter, "Rainbow With Glitter" },
  // { confetti, "Confetti" },
  // { sinelon, "Sinelon" },
  // { juggle, "Juggle" },
  // { bpm, "BPM" },

  // { showSolidColor, "Solid Color" },
};

const uint8_t patternCount = ARRAY_SIZE(patterns);
