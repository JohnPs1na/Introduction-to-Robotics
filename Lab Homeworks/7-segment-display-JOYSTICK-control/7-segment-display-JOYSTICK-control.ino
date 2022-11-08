// ///////////////////////////////

#define RIGHT 0
#define DOWN 1
#define LEFT 2
#define UP 3

// declare all the joystick pins
const int pinSW = 2; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output

// declare all the segments pins
const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;

const int segSize = 8;
int blinkingLedState = 0;

bool commonAnode = false; 

byte state = LOW;
byte dpState = LOW;
byte swState = LOW;
int xValue = 0;
int yValue = 0;

bool joyMoved = false;
bool joyPressed = false;
int digit = 0;
int minThreshold = 400;
int maxThreshold = 600;

bool changingLedIntensity = false;
const int blinking_interval = 550;

const int shortButtonPress_interval = 50;
const int longButtonPress_interval = 1500;

unsigned long blinkTime = 0;
int blinkingLed = 0;

int lastSwState = 0;
unsigned long lastDebounceTime = 0;
int reading = 0;

int segments[segSize] = { 
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

int pinStates[segSize] = {
  LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW
};

int neighbours[segSize][4] = {
  //      right,down,left,up
  /*pinA*/{pinB,pinG,pinF,NULL},
  /*pinB*/{NULL,pinC,pinG,pinA},
  /*pinC*/{NULL,pinD,pinG,pinB},
  /*pinD*/{pinC,NULL,pinE,pinG},
  /*pinE*/{pinG,pinD,NULL,pinF},
  /*pinF*/{pinG,pinE,NULL,pinA},
  /*pinG*/{pinB,pinD,pinF,pinA},
  /*pinDP*/{NULL,NULL,pinC,NULL}
};

void setup() {
  // initialize all the pins
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);
  if (commonAnode == true) {
    state = !state;
  }

  blinkingLed = pinDP;
  blinkTime = millis();
  Serial.begin(9600);
}



void loop() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);


  if(swState && changingLedIntensity && blinkingLed!=pinDP){
    changeLedIntensity(blinkingLed);
  } else {
    blinkLed(blinkingLed);
    controlJoystick();
  }
}

// gets the pin for neighbours array 
int getPin(int ledPin){
  return ledPin-4;
}


int changeState (int currentState){
    if (currentState == HIGH)
      return LOW;
    else
      return HIGH;
  };

void blinkLed(int ledPin) {

  if(millis() - blinkTime >= blinking_interval) {
    blinkTime = millis();
    blinkingLedState = changeState(blinkingLedState);
  }

  digitalWrite(ledPin,blinkingLedState);
}

void controlJoystick() {
  if( joyMoved == false){
    if (xValue < minThreshold) {
      changeBlinkingLed(DOWN);
    }
    if (xValue > maxThreshold) {
      changeBlinkingLed(UP);
    }
    if (yValue < minThreshold) {
      changeBlinkingLed(LEFT);
    }
    if (yValue > maxThreshold) {
      changeBlinkingLed(RIGHT);
    }
    joyMoved = true;
  }

	if (xValue > minThreshold && xValue < maxThreshold && yValue > minThreshold && yValue < maxThreshold) {
    joyMoved = false;
  }

  reading = digitalRead(pinSW);
  if(reading != lastSwState) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > shortButtonPress_interval){
    if(reading != swState){
      swState = reading;
      if(!swState){
        changingLedIntensity = true;
      }
    }
  }

  if(millis() - lastDebounceTime > longButtonPress_interval) {
    if(reading == swState && changingLedIntensity){
      if(!swState){
        resetDisplay();
      } 
    }
  }

  lastSwState = reading;

}

void changeBlinkingLed(int direction){
    int ledToChange = neighbours[getPin(blinkingLed)][direction];
    if(ledToChange != NULL){
      digitalWrite(blinkingLed,pinStates[getPin(blinkingLed)]);
      blinkingLed = ledToChange;
    }
}

void resetDisplay() {
  for (int i = 0;i<segSize;i++){
    pinStates[i] = LOW;
    digitalWrite(segments[i],LOW);
    blinkingLed = pinDP;
    changingLedIntensity = false;
  };
}

void changeLedIntensity(int currentLed) {
  int pinToChange = getPin(currentLed);

  if( joyMoved == false){
    if (xValue < minThreshold) {
      pinStates[pinToChange] = LOW;
      digitalWrite(currentLed,LOW);
    }
    if (xValue > maxThreshold) {
      pinStates[pinToChange] = HIGH;
      digitalWrite(currentLed,HIGH);
    }
    joyMoved = true;
  }
	if (xValue > minThreshold && xValue < maxThreshold) {
    joyMoved = false;
  }

  reading = digitalRead(pinSW);
  if(reading != lastSwState) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > shortButtonPress_interval){
    if(reading != swState){
      swState = reading;
      if(!swState){
        changingLedIntensity = false;
      }
    }
  }

  lastSwState = reading;
}




