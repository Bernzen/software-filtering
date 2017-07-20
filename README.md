# software-filtering

Running out of time to finish your robot? Don't have enough space for electronics? Why not give yourself one less circuit board to build and troubleshoot by using software to detect the 10 kHz and 1 kHZ IR beacons? This code takes readings from a sensor connected directly to an analog input on the microcontroller, and determines the intensity of a given frequency component.

To use, simply add `filter.cpp` and `filter.h` to your Arduino project, `#include "filter.h"`, and then call `detect_10khz` or `detect_1khz` with the number of the pin the sensor is connected to. These functions will return a value in arbitrary units corresponding to how strong the signal is at each frequency.

Circuit diagram:

![Circuit Diagram](schematic.png?raw=true)

Example usage:

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
    
If you are interested in detecting other frequencies, or sampling once and using that to detect multiple frequencies, look at `filter.cpp` to see how `detect_10khz` and `detect_1khz` make use of `sample_signal` and `detect_frequency`.
