#define STATE1 1
#define STATE2 2
#define STATE3 3
#define STATE4 4

#define NOTE1 523
#define NOTE2 2093

// pin setup
const int pedRedLed = 5;
const int pedGreenLed = 6;

const int carRedLed = 11;
const int carYellowLed = 10;
const int carGreenLed = 9;

const int buttonPin = 2;
const int buzzerPin = 3;


// time variables
unsigned long blinkTime = 0;
unsigned long prevTime = 0;
unsigned long beepTime = 0;

// states
int currentState = 0;
int blinkingLedState = LOW;
byte buttonState = 0;

bool switchedLeds = false;

// intervals
const int blinking_interval = 333;
const int stateTwo_interval1 = 5000;
const int stateTwo_interval2 = 3000;
const int stateThree_interval = 8000;
const int stateFour_interval = 4000;
const int buzzerBeepBeep_interval = 500;

void initState() {
  pinMode(pedRedLed,OUTPUT);
  pinMode(pedGreenLed,OUTPUT);
  pinMode(carRedLed,OUTPUT);
  pinMode(carYellowLed,OUTPUT);
  pinMode(carGreenLed,OUTPUT);
  pinMode(buttonPin,INPUT_PULLUP);

  prevTime = millis();
  currentState = STATE1;
}

void resetSemaphore() {
  digitalWrite(carGreenLed,HIGH);
  digitalWrite(pedRedLed,HIGH);
  digitalWrite(carRedLed,LOW);
  digitalWrite(carYellowLed,LOW);
  digitalWrite(pedGreenLed,LOW);
}

void State1() {
  resetSemaphore();
  prevTime = millis();
  noTone(buzzerPin);

  if(buttonPressed()){
    currentState = STATE2;
    switchedLeds = false;
  }
}

void State2() {

  if(!switchedLeds) {
    if(millis() - prevTime >= stateTwo_interval1) {

      digitalWrite(carYellowLed,HIGH);
      digitalWrite(carGreenLed,LOW);
      switchedLeds = true;
      prevTime = millis();

    }
  }

  if(switchedLeds) {
    if(millis() - prevTime >= stateTwo_interval2) {

      digitalWrite(carYellowLed,LOW);
      digitalWrite(carRedLed,HIGH);
      digitalWrite(pedRedLed,LOW);
      digitalWrite(pedGreenLed,HIGH);

      switchedLeds = false;
      prevTime = millis();
      currentState = STATE3;
    }
  }
}

void State3() {
  if(millis() - prevTime >= stateThree_interval) {
    prevTime = millis();
    currentState = STATE4;
  }

  buzzerBeep(buzzerBeepBeep_interval);
}

void State4() {
  if(millis() - prevTime >= stateFour_interval) {
    prevTime = millis();
    currentState = STATE1;
  }

  buzzerBeep(buzzerBeepBeep_interval / 2);
  blinkingLed();
}


void blinkingLed() {

  auto changeState = [](int currentState){
    if (currentState == HIGH)
      return LOW;
    else
      return HIGH;
  };

  if(millis() - blinkTime >= blinking_interval) {
    blinkTime = millis();
    blinkingLedState = changeState(blinkingLedState);
  }

  digitalWrite(pedGreenLed,blinkingLedState);
}

void buzzerBeep(int interval) {
  if(millis() - beepTime >= interval){
    tone(buzzerPin,NOTE1);
    beepTime = millis();
  } else {
    noTone(buzzerPin);
  }
}


void setup() {
  initState();
  Serial.begin(9600);
}

bool buttonPressed() {
  return !digitalRead(buttonPin);
}

void loop() {

  switch(currentState){
    case STATE1:
      State1();
      break;
    case STATE2:
      State2();
      break;
    case STATE3:
      State3();
      break;
    case STATE4:
      State4();
      break;

  }
}


