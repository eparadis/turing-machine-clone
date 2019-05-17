// TimeOne from https://github.com/PaulStoffregen/TimerOne
#include <TimerOne.h>

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

// max number of steps
#define STEPS_MAX 16

// number of notes in the output voltage LUT
#define NOTES_MAX 12

bool trigger = false;

int pattern[STEPS_MAX];
int notes[NOTES_MAX];
byte patternIndex = 0;


void setup() {
  pinMode(JACK4_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Timer1.initialize(1000); // microseconds
  Timer1.pwm(JACK4_PIN, 0);

  // fill the notes lookup table with half-note steps
  for(byte i=0; i<NOTES_MAX; i+=1) {
    notes[i] = (i * 1000.0) / 12.0;
  }

  // fill the pattern buffer with the indices of the note LUT
  for(byte i=0; i<STEPS_MAX; i+=1) {
    pattern[i] = i%NOTES_MAX;
  }
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

int voltageToPWM(int millivolts) {
  return map(millivolts, 0, 5000, 0, 1023);
}

void loop() {
  sampleAnalogInputs();
  int probability = map(pot1, 0, 1023, 0, 1000);
  int steps = map(pot2, 0, 1023, 1, STEPS_MAX);
  // what to use pot3 for? maybe bank-switching different scales?
  // int amplitude = map(pot3, 0, 1023, 0, 5000);

  if(trigger && jack3 < 512) { // falling edge
    trigger = false;
    updateLED();
  } else if(!trigger && jack3 >= 512) { // rising edge
    trigger = true;
    updateLED();

    // if a random number is under our PROB threshold,
    //  change the current LUT index in the pattern
    // leave a small gap at the bottom to have a solid 'lock' area
    if(random(25, 1000) < probability) {
      pattern[patternIndex] = random(0, NOTES_MAX);
    }

    // pick the current value out of the pattern
    //  and look up in the note LUT what voltage to output
    int out = notes[ pattern[ patternIndex] ];
    out = voltageToPWM(out);
    Timer1.setPwmDuty(JACK4_PIN, out);
    Serial.println(out);

    // reset pattern index to zero when it reaches our step count
    patternIndex = (patternIndex + 1) % steps;
  }
}
