#include <Wire.h>
#include "MT6701.h"

MT6701 encoder;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  encoder.initializeI2C();
}

void loop() {

  // read angular position
  float curr_angle = encoder.angleRead();
  Serial.println(curr_angle);


}
