#include "LedControl.h"
#include <LiquidCrystal.h>

using namespace std;

#define LEFT 1
#define DOWN 2
#define RIGHT 3
#define UP 4


#define MENU 1
#define GAME 2
#define OVER 3


typedef void(*Action)();  


const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;


const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;
byte swState = LOW;
int xValue = 0;
int yValue = 0;

const int minThreshold = 400;
const int maxThreshold = 600;
bool joyMoved = false;

bool val = true;

LedControl lc = LedControl(dinPin,clockPin,loadPin,1);

byte matrixBrightness = 10;


int GAME_WIDTH = 8;
int GAME_HEIGHT = 8;
int SPEED = 250;
int SPACE_SIZE = 1;
int BODY_PARTS = 2;

int score = 0;
int direction = DOWN;

unsigned long timePassed = 0;

bool gameOver = false;

int mainState = MENU;

int lastSwState = 0;
unsigned long lastDebounceTime = 0;
int reading = 0;

int buttonPressInterval = 50;


int snakeCoords[64][2] = {{-1,-1}};

// food related
int foodCoords[2] = {0};
bool foodIn = false;

void createFood(){
  foodCoords[0] = (int)random(0,8);
  foodCoords[1] = (int)random(0,8);
  lc.setLed(0, foodCoords[0], foodCoords[1], true);
} 
void removeFood() {
  lc.setLed(0, foodCoords[0], foodCoords[1], false);
  foodIn = false;
}

//game related
void next_turn(){ 
  int x = snakeCoords[0][0];
  int y = snakeCoords[0][1];

  if (direction == UP) {
    y-=1;
  } else if (direction == DOWN) {
    y+=1;
  } else if (direction == RIGHT) {
    x+=1;
  } else if (direction == LEFT){
    x-=1;
  }


  int idx = 0;

  while (true) {
    if (snakeCoords[idx][0] == -1){
      break;
    }
    idx++;
  }

  for (int i = idx;i>0;i--){
    snakeCoords[i][0] = snakeCoords[i-1][0];
    snakeCoords[i][1] = snakeCoords[i-1][1];
  }

  snakeCoords[0][0] = x;
  snakeCoords[0][1] = y;


  if(x == foodCoords[0] && y == foodCoords[1]){
    removeFood();
  } else {
    lc.setLed(0, snakeCoords[idx][0], snakeCoords[idx][1], false);

    snakeCoords[idx][0] = -1;
    snakeCoords[idx][1] = -1;
  }
  
  lc.setLed(0, x, y, true);

  if (checkCollisions()){
    foodIn = false;
    mainState = OVER;
  }
}
bool checkCollisions(){
  int x = snakeCoords[0][0];
  int y = snakeCoords[0][1];

  if (x < 0 || x >= 8){
    return true;
  }
  if (y < 0 || y >= 8) {
    return true;
  }

  int idx = 0;

  while (true) {
    if (snakeCoords[idx][0] == -1){
      break;
    }
    idx++;
  }

  for (int i = 1;i<idx;i++){
    if (x == snakeCoords[i][0] && y == snakeCoords[i][1]){
      return true;
    }
  }

  return false;
}
void control_joystick() {
  if (joyMoved == false) {
    if(xValue < minThreshold){
      if (direction != UP)
        direction = DOWN;
    } 
    if(xValue > maxThreshold){
      if (direction != DOWN)
        direction = UP;
    }
    if(yValue < minThreshold){
      if(direction != RIGHT)
        direction = LEFT;
    }
    if(yValue > maxThreshold){
      if(direction != LEFT)
        direction = RIGHT;
    }
    joyMoved = true;
  }

  if (xValue > minThreshold && xValue < maxThreshold) {
    joyMoved = false;
  }
}
void createSnake() {
  direction = DOWN;

    // setup snake 
  for(int i = 0;i<64;i++){
    snakeCoords[i][0] = -1;
    snakeCoords[i][1] = -1;
  }

  
  for(int i = 0;i<BODY_PARTS;i++){
    snakeCoords[i][0] = 0;
    snakeCoords[i][1] = 0;
  }

  for (int i = 0;i<64;i++){
    if (snakeCoords[i][0] != -1){
      lc.setLed(0, snakeCoords[i][0], snakeCoords[i][1],true);
    }
  }
}


// State,display related
bool playButtonDrawn = false;
void menu() {
  byte playButton[] = { B00000000,B00111100,B01000010,B01000010,B01000010,B00100100,B00011000,B00000000};

  if(!playButtonDrawn) {
    for (int i = 0;i<matrixSize;i++){
      for (int j = 0;j<matrixSize;j++){
        lc.setLed(0,i,j,false);
      }
    }  

    for (int i = 0;i<matrixSize;i++){
      lc.setRow(0,i,playButton[i]);
    }

    playButtonDrawn = true;
  }

  reading = digitalRead(pinSW);
  if(reading != lastSwState) {
    lastDebounceTime = millis();
  }

  if(millis() - lastDebounceTime > buttonPressInterval){
    if(reading!=swState) {
      swState = reading;
      if(!swState){
        for (int i = 0;i<matrixSize;i++){
          for (int j = 0;j<matrixSize;j++){
            lc.setLed(0,i,j,false);
          }
        }  
        playButtonDrawn = false;
        mainState = GAME;
        createSnake();
      }
    }
  }

  lastSwState = reading;

}
void game() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  control_joystick();

  if(!foodIn){
    createFood();
    foodIn = true;

    int idx = 0;

    while (true) {
      if (snakeCoords[idx][0] == -1){
        break;
      }
      if(snakeCoords[idx][0] == foodCoords[0] && snakeCoords[idx][1] == foodCoords[1]) {
        removeFood();
        lc.setLed(0, snakeCoords[idx][0], snakeCoords[idx][1], true);
        break;
      }
      idx++;
    }

  }

  reading = digitalRead(pinSW);
  if(reading != lastSwState) {
    lastDebounceTime = millis();
  }

  if(millis() - lastDebounceTime > buttonPressInterval){
    if(reading!=swState) {
      swState = reading;
      if(!swState){
        foodIn = false;
        mainState = MENU;
      }
    }
  }

  lastSwState = reading;


  if(millis() - timePassed > SPEED){
    timePassed = millis();
    next_turn();
  }
}
bool xDrawn = false;
void over() {

  byte X[] = {B00000000,B01000010,B00100100,B00011000,B00011000,B00100100,B01000010,B00000000};

  if(!xDrawn){
    for (int i = 0;i<matrixSize;i++){
      for (int j = 0;j<matrixSize;j++){
        lc.setLed(0,i,j,false);
      }
    }  

    for (int i = 0;i<matrixSize;i++){
      lc.setRow(0,i,X[i]);
    }

  }

  reading = digitalRead(pinSW);
  if(reading != lastSwState) {
    lastDebounceTime = millis();
  }

  if(millis() - lastDebounceTime > buttonPressInterval){
    if(reading!=swState) {
      swState = reading;
      if(!swState){
        for (int i = 0;i<matrixSize;i++){
          for (int j = 0;j<matrixSize;j++){
            lc.setLed(0,i,j,false);
          }
        }  
        xDrawn = false;
        mainState = GAME;
        createSnake();
      }
    }
  }

  lastSwState = reading;
}

void sayHi(){
  Serial.println("hello");
}
class Menu{

  private:

  Menu(){
    for (int i = 0;i<actionsSize;i++){
      actions[i] = &sayHi;
    }
  }
  Menu(const Menu&) = delete;
  Menu& operator=(const Menu&) = delete;

  public:
  
  void run(){

    bool running = true;

    actions[0]();
  }

  public:

  static Menu* get_instance(){
    if (!g_instance)
      g_instance = new Menu();

    return g_instance;
  }

  static void delete_instance(){
    delete g_instance;
    g_instance = nullptr;
  }

  private:
  Action actions[5];
  int actionsSize = 5;

  static Menu* g_instance;
};

Menu* Menu::g_instance = nullptr;

void setup() {
  // put your setup code here, to run once:
  lc.shutdown(0,false);
  lc.setIntensity(0,matrixBrightness);
  lc.clearDisplay(0);
  pinMode(pinSW,INPUT_PULLUP);
  

  Serial.begin(9600);

}



void loop() {
  switch(mainState){
    case MENU:
      menu();
      break;
    case GAME:
      game();
      break;
    case OVER:
      over();
      break;
  }

  // Menu::get_instance()->run();
}
