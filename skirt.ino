#include <FastLED.h>
#include "Input.h"
#include "Animation.h"

#include "animations/TestAnimation.cpp"
#include "animations/SafetyLights.cpp"

#define LED_NUM 300

CRGB leds[LED_NUM];

PixelRow rows[] = {
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5),
  PixelRow(8.5)
};

constexpr uint8_t rowCount = sizeof(rows) / sizeof(PixelRow);

Display dpy(rows, rowCount, leds);
SafetyLights safetyLights;

Input input;
Animator animations;

void setup() {
  FastLED.addLeds<WS2812B, 3, GRB>(leds, LED_NUM);
  for (int i = 0; i < LED_NUM; i++) {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();

  animations.setAnimation(new TestAnimation());
}

void loop() {

  auto evt = input.read();
  animations.handleEvent(evt);
  animations.update();
  animations.render(&dpy);

  // Render the safety lights as a special case
  safetyLights.handleEvent(evt);
  safetyLights.update();
  safetyLights.render(&dpy);
  FastLED.show();
}
