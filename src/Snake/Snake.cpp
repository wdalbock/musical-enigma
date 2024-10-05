#include <TFT_eSPI.h>
#include "Snake.h"
#include "back.h"
#include "gameOver.h"
#include "newGame.h"
#include "../Sound/snake_sound.h"

#include <esp_now.h>
#include <WiFi.h>

// extern TFT_eSPI tft; 
// extern TFT_eSprite sprite;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

bool isPlayingMelody1 = false; // Track if melody is currently playing
typedef struct struct_message {
    int player;
    int left;
    int right;
    int up;
    int down;
    int start;
} struct_message;

struct_message input;

static int size=1;
static int y[120]={0};
static int x[120]={0};

unsigned long currentTime=0;
unsigned long lastButtonPressTime = 0; // Last time a button was pressed
unsigned long readyTime = 0; // Last time ready state was checked
int period=200;
int deb,deb2=0;
int dirX=1;
int dirY=0;
bool taken=0;
unsigned short colors[2]={0x48ED,0x590F}; //terain colors
unsigned short snakeColor[2]={0x9FD3,0x38C9};
bool chosen=0;
bool gOver=0;
int moves=0;
int playTime=0;
int foodX=0;
int foodY=0;
int howHard=0;
String diff[3]={"EASY","NORMAL","HARD"};
bool ready=1;

enum GameState { MENU, GAME };
GameState snake_currentState = MENU;


void getFood()//.....................getFood -get new position of food
{
    foodX=random(0,17);
    foodY=random(0,17);
    taken=0;
    for(int i=0;i<size;i++)
    if(foodX==x[i] && foodY==y[i])
    taken=1;
    if(taken==1)
    getFood();
}

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  Serial.println("data received");
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  memcpy(&input, incomingData, sizeof(input));
}

void Snake_setup() {  //.......................setup
    tft.init();
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(0); 
    tft.setSwapBytes(true);
    tft.pushImage(0,0,170,320,back);
    tft.pushImage(0,30,170,170,newGame);
    
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      return;
    }
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  
    tft.setTextColor(TFT_PURPLE,0x7DFD);
    tft.fillSmoothCircle(28,102+(howHard*24),5,TFT_RED,TFT_BLACK); 
    tft.drawString("DIFFICULTY:   "+ diff[howHard]+"   ",26,267); 
   
    sprite.createSprite(170,170);
    sprite.setSwapBytes(true);

    size = 1;
    dirX = 1;
    dirY = 0;
    gOver = 0;  
    taken = 0;
    moves = 0;
    getFood();

    tft.setTextSize(3);
    tft.setTextDatum(4);
    tft.drawString(String(size), 44, 250); 
    tft.drawString(String(500 - period), 124, 250);

    delay(400);

    snakesound_setup();  
    if (!isPlayingMelody1) {
        snake_playMelody();  // Play melody part 1 (menu music)
        isPlayingMelody1 = true;  // Set the flag to indicate melody is playing
    }


    while(snake_currentState==MENU){
      snakesound_loop(); 

      if(millis() - lastButtonPressTime > 200){
        if(input.down == 0){
          tft.fillSmoothCircle(28, 102 + (howHard * 24), 5, TFT_BLACK);
          howHard = (howHard + 1) % 3;
          tft.fillSmoothCircle(28, 102 + (howHard * 24), 5, TFT_RED, TFT_BLACK);
          tft.drawString("DIFFICULTY: " + diff[howHard] + "   ", 26, 267);
          period = 200 - howHard * 20;
          lastButtonPressTime = millis();
        }

        if (input.start == 0) {
          snake_currentState = GAME;  
          break;  
        }
      }
    }
}

void checkGameOver()//..,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,check game over
{
  if(x[0]<0 || x[0]>=17 || y[0]<0 || y[0]>=17 )
    gOver=true;
  for(int i=1;i<size;i++)
    if(x[i]==x[0] && y[i]==y[0])
      gOver=true;
}

void run() {
    for (int i = size; i > 0; i--) {
        x[i] = x[i - 1];    
        y[i] = y[i - 1];     
    }    

    x[0] += dirX;
    y[0] += dirY;

    if (x[0] == foodX && y[0] == foodY) {
        size++;
        getFood();
        tft.drawString(String(size), 44, 250); 
        period = period - 1; 
        tft.drawString(String(500 - period), 124, 250);
    }

    sprite.fillSprite(TFT_BLACK);
    checkGameOver();
    if (gOver == 0) {
        sprite.drawRect(0, 0, 170, 170, 0x02F3);     
        for (int i = 0; i < size; i++) {
            sprite.fillRoundRect(x[i] * 10, y[i] * 10, 10, 10, 2, snakeColor[0]); 
            sprite.fillRoundRect(2 + x[i] * 10, 2 + y[i] * 10, 6, 6, 2, snakeColor[1]); 
        }
        sprite.fillRoundRect(foodX * 10 + 1, foodY * 10 + 1, 8, 8, 1, TFT_RED); 
        sprite.fillRoundRect(foodX * 10 + 3, foodY * 10 + 3, 4, 4, 1, 0xFE18); 
    } else {
        sprite.pushImage(0, 0, 170, 170, gameOver);    
        stopMelody(); // Stop the melody on game over
    }    
    sprite.pushSprite(0, 30);
}   


int change=0;

void Snake_loop() { //...............................................................loop
    snakesound_loop(); // Continuously check sound

    if (millis() > currentTime + period) {
        run(); 
        currentTime = millis(); 
    } 

    if (millis() > readyTime + 100 && ready == 0) 
        ready = 1; 

    if (ready == 1) {
        // Non-blocking button checks with debounce logic for snake movement
        if (input.left == 0) {
            if (millis() - lastButtonPressTime > 200) {
                if (dirX == 1) { dirY = dirX * -1; dirX = 0; }
                else if (dirX == -1) { dirY = dirX * -1; dirX = 0; }
                else if (dirY == 1) { dirX = dirY * 1; dirY = 0; }
                else if (dirY == -1) { dirX = dirY * 1; dirY = 0; }
                lastButtonPressTime = millis(); // Update the last button press time
                ready = 0;
            }
        } 

        if (input.right == 0) {
            if (millis() - lastButtonPressTime > 200) {
                if (dirX == 1) { dirY = dirX * 1; dirX = 0; }
                else if (dirX == -1) { dirY = dirX * 1; dirX = 0; }
                else if (dirY == 1) { dirX = dirY * -1; dirY = 0; }
                else if (dirY == -1) { dirX = dirY * -1; dirY = 0; }
                lastButtonPressTime = millis(); // Update the last button press time
                ready = 0;
            }
        } 
    }
        // Check for game over state and reset if select button is pressed
    if (gOver) {
        if (input.start == 0) {
            snake_currentState = MENU; // Switch to menu state
             // Stop the melody when restarting
            Snake_setup(); // Re-initialize game setup
        }
    }
  }


void SnakeMain() {
    Snake_setup(); 
    while (snake_currentState == GAME) {
        Snake_loop(); 
    }
}

