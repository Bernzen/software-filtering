#include "filter.h"

#define INPUT_PIN 0

void setup() {
  Serial.begin(9600);
}

void loop() {
  uint32_t intensity_10khz = detect_10khz(INPUT_PIN);
  uint32_t intensity_1khz = detect_1khz(INPUT_PIN);
  
  Serial.print(intensity_10khz);
  Serial.print(' ');
  Serial.println(intensity_1khz);

  delay(50);
}
