#include <TFT_eSPI.h>
#include "Snake.h"
#include "back.h"
#include "gameOver.h"
#include "newGame.h"
#include "../Setup/common.h"
#include "snake_sounds.h"

#include "ESP32S3VGA.h"
#include "GfxWrapper.h"

#include <esp_now.h>
#include <WiFi.h>

extern VGA vga;
extern Mode mode;
extern int width;
extern int height;
extern GfxWrapper<VGA> gfx;

extern struct_message buttonState;

extern void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len);

static int size=1;
static int y[120]={0};
static int x[120]={0};

unsigned long currentTime=0;
int period=200;
int deb,deb2=0;
int dirX=1;
int dirY=0;
bool taken=0;
unsigned short snakeColor[2]={0x9FD3,0x38C9};
bool chosen=0;
bool gOver=0;
int moves=0;
int foodX=0;
int foodY=0;
int howHard=0;
String diff[3]={"EASY","NORMAL","HARD"};
bool ready=1;
long readyTime=0;

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

void Snake_setup() {  //.......................setup
    vga.clear(vga.rgb(0, 0, 0));
    gfx.drawRGBBitmap(width / 2 - 85, height / 2 - 160, back, 170, 320);  
    gfx.drawRGBBitmap(width / 2 - 85, height / 2 - 130, newGame, 170, 170);

    gfx.fillCircle(width / 2 - 65, height / 2 - 57 + (howHard * 24), 5, 0xFFFF);  // Fill a circle to indicate difficulty level
    vga.show(); 

    while(buttonState.start == 1) {
        if (buttonState.down == 0) {
            if (deb2 == 0) {
                vga.clear(vga.rgb(0, 0, 0));
                deb2 = 1;
                gfx.drawRGBBitmap(width / 2 - 85, height / 2 - 160, back, 170, 320);
                gfx.drawRGBBitmap(width / 2 - 85, height / 2 - 130, newGame, 170, 170);
                gfx.fillCircle(width / 2 - 65, height / 2 - 57 + (howHard * 24), 5, 0x0000);  // Clear the current circle
                howHard++;
                if (howHard == 3) howHard = 0;
                gfx.fillCircle(width / 2 - 65, height / 2 - 57 + (howHard * 24), 5, 0xFFFF);  // Draw new circle for selected difficulty
                period = 200 - howHard * 20;  
                vga.show();
            }
        } else {
            deb2 = 0;
        }
    }

    y[0] = random(5, 15); 
    getFood(); 
    delay(400);
    dirX = 1;
    dirY = 0;
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

    vga.clear(vga.rgb(0, 0, 0)); 
    gfx.drawRGBBitmap(width / 2 - 85, height / 2 - 160, back, 170, 320);

    if (x[0] == foodX && y[0] == foodY) {
        size++;
        getFood();
    }

    gfx.setTextColor(0xf800);
    gfx.setTextSize(2);
    gfx.setCursor(width / 2 - 60, height / 2 + 75);
    gfx.print(size);  // Update snake size display
    period -= 1;
    gfx.setCursor(width / 2 + 20, height / 2 + 75);
    gfx.print(500 - period);  // Update game speed display

    // Draw snake and food
    checkGameOver();
    if (!gOver) {
        gfx.drawRect(width / 2 - 85, height / 2 - 130, 170, 170, 0x02F3);  // Draw game boundary
        for (int i = 0; i < size; i++) {
            gfx.fillRoundRect(x[i] * 10 + width / 2 - 85, y[i] * 10 + height / 2 - 130, 10, 10, 2, snakeColor[0]);  // Snake body
            gfx.fillRoundRect(2 + x[i] * 10 + width / 2 - 85, 2 + y[i] * 10 + height / 2 - 130, 6, 6, 2, snakeColor[1]);  // Snake detail
        }
        gfx.fillRoundRect(foodX * 10 + 1 + width / 2 - 85, foodY * 10 + 1 + height / 2 - 130, 8, 8, 1, 0xF800);  // Draw food
        gfx.fillRoundRect(foodX * 10 + 3 + width / 2 - 85, foodY * 10 + 3 + height / 2 - 130, 4, 4, 1, 0xFE18);  // Food detail
    } else {
        gfx.drawRGBBitmap(width / 2 - 85, height / 2 - 130, gameOver, 170, 170);  // Game over screen
    }
    vga.show();
}


int change=0;

void Snake_loop() { //...............................................................loop
updateFurElise();
  
if(millis()>currentTime+period) 
{run(); currentTime=millis();} 

if(millis()>readyTime+100 && ready==0) 
{ready=1;} 

if(ready==1){
if(buttonState.left==0){

  
  if(deb==0)
  {deb=1;
  if(dirX==1 && change==0) {dirY=dirX*-1; dirX=0; change=1;}
  if(dirX==-1 && change==0) {dirY=dirX*-1; dirX=0;change=1; }
  if(dirY==1 && change==0) {dirX=dirY*1; dirY=0; change=1;}
  if(dirY==-1 && change==0) {dirX=dirY*1; dirY=0; change=1;}
  change=0;
  ready=0;
  readyTime=millis();
  }
}else{ deb=0;}}

if(ready==1){
if(buttonState.right==0)
{
   
  if(deb2==0)
  {deb2=1;
   if(dirX==1 && change==0) {dirY=dirX*1; dirX=0; change=1;}
   if(dirX==-1 && change==0) {dirY=dirX*1; dirX=0;change=1; }
   if(dirY==1 && change==0) {dirX=dirY*-1; dirY=0; change=1;}
   if(dirY==-1 && change==0) {dirX=dirY*-1; dirY=0; change=1;}
  change=0;
  ready=0;
  readyTime=millis();
  }
}else {deb2=0;}}

}

int SnakeMain(){
  delay(200);  
  Snake_setup();
  startFurElise();
  while(!gOver){
   Snake_loop(); 
  }
  // Place logic here to store leaderboard data
  int returnVal = size;

  // Reset all variables.
  size=1;
  memset(x, '\0', sizeof(x));
  memset(y, '\0', sizeof(y));
  currentTime=0;
  period=200;
  deb,deb2=0;
  dirX=1;
  dirY=0;
  taken=0;
  chosen=0;
  gOver=false;
  moves=0;
  foodX=0;
  foodY=0;
  howHard=0;
  ready=1;
  readyTime=0;

  delay(5000);
  return returnVal;
}
