// a loose clone of a clone
// original: MTM Turning Machine
// clone: 2hp TM
// clone: this thing


int pot1; // A6
int pot2; // A3
int pot3; // A7
int jack1; // A2
int jack2; // A1
int jack3; // A0
// jack4
#define JACK4_PIN 9

// maximum amplitude of output, in volts
#define AMPLITUDE_MAX 3000
bool trigger = false;

int pattern[32] = {
  000, 100, 200, 300,
  400, 500, 600, 700,
  800, 900, 1000, 1100,
  1200, 1300, 1400, 1500,
  1600, 1700, 1800, 1900,
  2000, 2100, 2200, 2300,
  2400, 2500, 2600, 2700,
  2800, 2900, 3000, 3100
};
byte patternIndex = 0;


void setup() {
  pinMode(JACK4_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void sampleAnalogInputs() {
  pot1 = analogRead(A6); // probability
  pot2 = analogRead(A3); // num steps
  pot3 = analogRead(A7); // amplitude
  jack1 = analogRead(A2); // (unused)
  jack2 = analogRead(A1); // (unused)
  jack3 = analogRead(A0); // trigger
}

void updateLED() {
  digitalWrite(LED_BUILTIN, trigger);
}



void loop() {
  sampleAnalogInputs();
  int probability = map(pot1, 0, 1023, 0, 1000);
  int steps = map(pot2, 0, 1023, 1, 16);
  int amplitude = map(pot3, 0, 1023, 0, 5000);

  if(trigger && jack3 < 512) { // falling edge
    trigger = false;
    updateLED();
  } else if(!trigger && jack3 >= 512) { // rising edge
    trigger = true;
    updateLED();

    // if a random number is under our PROB threshold,
    //  change the current value in the pattern
    if(random(0, 1000) < probability) {
      pattern[patternIndex] = random(0, 5000);
    }

    // pick the current value out of the pattern
    //  and map it to the amplitude maximum
    //  then map it to the DAC output range
    int out = map(pattern[patternIndex], 0, 5000, 0,
              map(amplitude, 0, AMPLITUDE_MAX, 0, 255));
    analogWrite(JACK4_PIN, out);
    patternIndex = (patternIndex + 1) % steps;
  }

  
}
