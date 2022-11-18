#define STATE1 1
#define STATE2 2

const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;
byte swState = LOW;
int xValue = 0;
int yValue = 0;

const int dataPin = 12;
const int latchPin = 11;
const int clockPin = 10;

const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;

const int minThreshold = 400;
const int maxThreshold = 600;
bool joyMoved = false;
bool joyPressed = false;

const byte regSize = 8;

int displayDigits[] = {
  segD1,segD2,segD3,segD4
};

const int displayCount = 4;

byte registers[regSize];
const int encodingsNumber = 16;

//variables used to make a led blink
int blinkingLed = 0;
unsigned long blinkTime = 0;
const int blinkingInterval = 550;
const int shortButtonPressInterval = 50;
const int longButtonPressInterval = 1000;

int shortPress = -1;
int longPress = -1;

int activeDp = 1;
byte dpState=LOW;
int encodingsToDisplay[displayCount] {
  0,0,0,0
};

int lastSwState = 0;
unsigned long lastDebounceTime = 0;
int reading = 0;

byte byteEncodings[encodingsNumber] = {
//A B C D E F G DP 
  B11111100, // 0 
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110, // 9
  B11101110, // A
  B00111110, // b
  B10011100, // C
  B01111010, // d
  B10011110, // E
  B10001110  // F
};


void setup() {
  pinMode(latchPin,OUTPUT);
  pinMode(clockPin,OUTPUT);
  pinMode(dataPin,OUTPUT);

  pinMode(pinSW,INPUT_PULLUP);
  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i],OUTPUT);
    digitalWrite(displayDigits[i],LOW);
  }
  Serial.begin(9600);
}

int main_state = STATE1;

void loop() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  displayEncodings();

  if(main_state == STATE1){
      blinkDp();
      controlJoystick();
  }
  if(main_state == STATE2 ){
      changeDigit();
  }

}

void writeReg(byte encoding) {
  digitalWrite(latchPin,LOW);
  shiftOut(dataPin,clockPin,MSBFIRST,encoding);
  digitalWrite(latchPin,HIGH);
}

void reset() {
  for(int i = 0;i<displayCount;i++){
    encodingsToDisplay[i] = 0;
  }
}

void displayEncodings() {
  for (int i = displayCount-1; i>=0; i--) {
    byte encoding = byteEncodings[encodingsToDisplay[i]] + ((i == (activeDp - 1)) && dpState);
    writeReg(encoding);
    digitalWrite(displayDigits[i],LOW);  
    delay(1);
    digitalWrite(displayDigits[i],HIGH);  
  }
}

void blinkDp() {
  if(millis() - blinkTime >= blinkingInterval) {
    blinkTime = millis();
    dpState = !dpState;
  }
}

void controlJoystick() {
  if (joyMoved == false) {
    if(xValue < minThreshold){
      if(activeDp < 4) activeDp++;
    } 
    if(xValue > maxThreshold){
      if(activeDp > 1) activeDp--;
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

  if(millis() - lastDebounceTime > shortButtonPressInterval){
    if(reading!=swState) {
      swState = reading;
      if(!swState){
        shortPress = 1;
      }
    }
  }

  if(millis() - lastDebounceTime > longButtonPressInterval) {
    if(reading == swState && shortPress == 1){
      if(!swState){
        longPress = 1;
      } else {
        longPress = 0;
      }
    }
  }

  if (shortPress && longPress==0){
    shortPress = -1;
    longPress = -1;
    main_state = STATE2;
  }

  if (shortPress == 1 && longPress == 1) {
    shortPress = -1;
    longPress = -1;
    reset();
  }

  lastSwState = reading;
}

void changeDigit() {
  int indexOfDigit = activeDp - 1;

  if (joyMoved == false) {
    if(yValue > maxThreshold){
      if(encodingsToDisplay[indexOfDigit] < 15) encodingsToDisplay[indexOfDigit]++;
    } 
    if(yValue < minThreshold){
      if(encodingsToDisplay[indexOfDigit] > 0) encodingsToDisplay[indexOfDigit]--;
    }
    joyMoved = true;
  }

  if (yValue > minThreshold && yValue < maxThreshold) {
    joyMoved = false;
  }

  reading = digitalRead(pinSW);
  if(reading != lastSwState) {
    lastDebounceTime = millis();
  }

  if(millis() - lastDebounceTime > shortButtonPressInterval){
    if(reading!=swState) {
      swState = reading;
      if(!swState){
        main_state = STATE1;
      }
    }
  }

  lastSwState = reading;
}