#include <TFT_eSPI.h>
#include "../Space_wars/space_wars.h"
#include "fonts/pixel.h"
#include "../Snake/Snake.h"
#include "ESP32S3VGA.h"
#include "GfxWrapper.h"

#define Pin_Button_Next 0
#define Pin_Button_Select 14

//                   r,  r, r, r,r,   g, g, g, g, g,g,   b, b, b, b,b,  h, v
const PinConfig pins(-1,-1,-1,-1,1,  -1,-1,-1,-1,-1,2,  -1,-1,-1,-1,3,  10,11);

const char*games[] = {"Space Wars", "Snake"}; 
int currentGameIndex = 0; 
int totalGames = sizeof(games)/ sizeof(games[0]);

VGA vga;
Mode mode = Mode::MODE_640x480x60;
int width = 640;
int height = 480;
GfxWrapper<VGA> gfx(vga, mode.hRes, mode.vRes);

enum State {
    MENU, 
    LOADING, 
    PLAYING
};

State currentState = MENU;

void showMainMenu() {
    vga.clear(vga.rgb(0, 0, 0)); // black
    gfx.setTextColor(0xFFFF); // white

    gfx.setTextSize(3);  
    gfx.setCursor(150, 75);
    gfx.print("ARCADE MENU"); 
    
    gfx.setTextSize(2); 

    for (int i = 0; i < totalGames; i++) {
        gfx.setCursor(150, 125 + 32 * i);

        if (i == currentGameIndex){
            gfx.setTextColor(0xFFE0);
            gfx.print(">>");
        }
        else {
            gfx.setTextColor(0xFFFF); 
        }
        gfx.setCursor(175, 125 + 32 * i);
        gfx.print(games[i]);
    }
    vga.show();
} 

void loadingAnimation(int x, int y, int w, int h) {
    float widthFraction = w / 100.0;
    for (int i = 1; i <= 100; i++) {
        gfx.drawRect(x, y, w, h, 0xFFFF);
        vga.clear(vga.rgb(0, 0, 0));
        gfx.fillRect(x, y, widthFraction * i, h, 0xFFFF);
        vga.show();
        delay(2);
    }
}

void setup() {
    vga.bufferCount = 2;
	if(!vga.init(pins, mode, 8)) {
        while(1) {
            delay(1);
        }
    }
	vga.start();

    pinMode(Pin_Button_Next, INPUT_PULLUP); 
    pinMode(Pin_Button_Select, INPUT_PULLUP); 
}

void loop() {
    if (currentState == MENU) {
        static int prevNextState = HIGH;
        static int prevButSel = HIGH;
        int currentNextState = digitalRead(Pin_Button_Next);
        int currentButState = digitalRead(Pin_Button_Select);


        if (currentNextState == LOW && currentNextState != prevNextState) {
            currentGameIndex = (currentGameIndex + 1) % totalGames; 
        }

        if (currentButState == LOW && currentButState != prevButSel) {
            currentState = LOADING;
        }

        showMainMenu();
        prevNextState = currentNextState;
        prevButSel = currentButState;

    } else if (currentState == LOADING) {
        currentState = PLAYING;
        loadingAnimation(35, 240, 520, 30);
        vga.clear(vga.rgb(0, 0, 0));
        vga.show();

    } else if (currentState == PLAYING) {
        if (currentGameIndex == 0) {
            space_warsMain(); 
        }
        else if (currentGameIndex == 1) { 
            SnakeMain(); 
        }
    }
}