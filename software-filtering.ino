const size_t num_samples = 512;
uint8_t samples[num_samples];

// Define various ADC prescaler
const unsigned char PS_2 = (1 << ADPS0);
const unsigned char PS_4 = (1 << ADPS1);
const unsigned char PS_8 = (1 << ADPS1) | (1 << ADPS0);
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

// Setup the serial port and pin 2
void setup() {
  Serial.begin(115200);

  // set up the ADC
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library

  // you can choose a prescaler from above.
  // PS_16, PS_32, PS_64 or PS_128
  ADCSRA |= PS_16;    // set our own prescaler to 64 
}

typedef uint16_t angle_t;

class IntTrigTable {
public:
  int16_t sin(angle_t angle) {
    int8_t index = (angle >> 7) & 0xFF;
    int16_t value = sample(index);
    if (angle & 0x8000) value = -value;
    return value;
  }
  int16_t cos(angle_t angle) {
    return sin(angle + 0x4000);
  }
private:
  int16_t sample(uint8_t index) {
    return data[index & 0xFF];
  }
  const int16_t data[256] = {
    0,3,6,9,13,16,19,22,25,28,31,34,38,41,44,47,
    50,53,56,59,62,65,68,71,74,77,80,83,86,89,92,95,
    98,101,104,107,109,112,115,118,121,123,126,129,132,134,137,140,
    142,145,147,150,152,155,157,160,162,165,167,170,172,174,177,179,
    181,183,185,188,190,192,194,196,198,200,202,204,206,207,209,211,
    213,215,216,218,220,221,223,224,226,227,229,230,231,233,234,235,
    237,238,239,240,241,242,243,244,245,246,247,248,248,249,250,250,
    251,252,252,253,253,254,254,254,255,255,255,256,256,256,256,256,
    256,256,256,256,256,256,255,255,255,254,254,254,253,253,252,252,
    251,250,250,249,248,248,247,246,245,244,243,242,241,240,239,238,
    237,235,234,233,231,230,229,227,226,224,223,221,220,218,216,215,
    213,211,209,207,206,204,202,200,198,196,194,192,190,188,185,183,
    181,179,177,174,172,170,167,165,162,160,157,155,152,150,147,145,
    142,140,137,134,132,129,126,123,121,118,115,112,109,107,104,101,
    98,95,92,89,86,83,80,77,74,71,68,65,62,59,56,53,
    50,47,44,41,38,34,31,28,25,22,19,16,13,9,6,3
  };
};

IntTrigTable trig;

uint32_t detectFrequency(int32_t detection_frequency, int32_t sample_rate, uint16_t num_samples, uint8_t* samples) {
  angle_t theta = 0;
  angle_t d_theta = (detection_frequency << 16) / sample_rate;
  int32_t real = 0;
  int32_t cplx = 0;
  int32_t signal_sum = 0;
  for (uint16_t n = 0; n < num_samples; n++) {
    signal_sum += samples[n];
  }
  int16_t signal_offset = signal_sum / num_samples;
  for (uint16_t n = 0; n < num_samples; n++) {
    real += ((samples[n] - signal_offset) * trig.cos(theta)) >> 4;
    cplx += ((samples[n] - signal_offset) * trig.sin(theta)) >> 4;
    theta += d_theta;
  }
  real = real >> 4;
  cplx = cplx >> 4;
  return ((real * real) + (cplx * cplx));
}

void loop() {
  unsigned int i;
  uint32_t sample_rate = 66253;

  unsigned long start_time = micros();
  for(i=0;i<num_samples;i++) {
    samples[i] = analogRead(0) >> 2;
  }
  unsigned long acquisition_time = micros() - start_time;

  int32_t intensity_10k = detectFrequency(10000, sample_rate, 64, samples) * 32;
  int32_t intensity_1k = detectFrequency(1000, sample_rate, 128, samples);
  
  /*Serial.print("Acquisition time: ");
  Serial.print(acquisition_time);
  Serial.print("us, ");
  Serial.print("10KHz: ");
  Serial.print(intensity_10k);
  Serial.print(", 1KHz: ");
  Serial.print(intensity_1k);
  Serial.println("\n");*/
  Serial.print(intensity_10k);
  Serial.print(" ");
  Serial.print(intensity_1k);
  Serial.println(" ");
  Serial.flush();
}
