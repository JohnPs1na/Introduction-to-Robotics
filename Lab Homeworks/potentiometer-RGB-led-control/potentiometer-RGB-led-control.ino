
const int redPotPin = A0;
const int greenPotPin = A1;
const int bluePotPin = A2;

int redPotValue = 0;
int greenPotValue = 0;
int bluePotValue = 0;

const int redLedPin = 9;
const int greenLedPin = 10; 
const int blueLedPin = 11;

int brightnessRedLed = 0;	
int brightnessGreenLed = 0;	
int brightnessBlueLed = 0;	

const int MAX_LED_VALUE = 255;
const float MAX_POT_VALUE = 1023.0;

void setup() {
  pinMode(redLedPin,OUTPUT);
  pinMode(greenLedPin,OUTPUT);
  pinMode(blueLedPin,OUTPUT);
  pinMode(bluePotPin,INPUT);
  pinMode(greenPotPin,INPUT);
  pinMode(redPotPin,INPUT);
  Serial.begin(9600);
}

void loop() {

  // reading pots value
  redPotValue = analogRead(redPotPin);
  greenPotValue = analogRead(greenPotPin);
  bluePotValue = analogRead(bluePotPin);

  brightnessRedLed = redPotValue * (MAX_LED_VALUE / MAX_POT_VALUE);
  brightnessGreenLed = greenPotValue * (MAX_LED_VALUE / MAX_POT_VALUE);  
  brightnessBlueLed = bluePotValue * (MAX_LED_VALUE / MAX_POT_VALUE);

  analogWrite(redLedPin,brightnessRedLed);
  analogWrite(greenLedPin,brightnessGreenLed);
  analogWrite(blueLedPin,brightnessBlueLed);

  delay(2);
}

