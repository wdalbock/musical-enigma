#include "../Space_wars/space_wars.h"
#include "../Connect_Four/Connect_Four.h"
#include "../Snake/Snake.h"
#include "common.h"

#include "ESP32S3VGA.h"
#include "GfxWrapper.h"

#include <esp_now.h>
#include <WiFi.h>

//                   r,  r, r, r,r,   g, g, g, g, g,g,   b, b, b, b,b,  h, v
const PinConfig pins(-1,-1,-1,-1,1,  -1,-1,-1,-1,-1,2,  -1,-1,-1,-1,3,  10,11);

const char*games[] = {"Space Wars", "Snake", "Connect Four"}; 
int currentGameIndex = 0; 
int totalGames = sizeof(games)/ sizeof(games[0]);

VGA vga;
Mode mode = Mode::MODE_640x400x70;
int width = 640;
int height = 400;
GfxWrapper<VGA> gfx(vga, mode.hRes, mode.vRes);

struct_message buttonState;

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

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  memcpy(&buttonState, incomingData, sizeof(buttonState));
  Serial.printf("l: %d, r: %d, u: %d, d: %d, s: %d, b: %d\n", buttonState.left, buttonState.right, buttonState.up, buttonState.down, buttonState.start, buttonState.back);
}

void setup() {
    // vga init
    vga.bufferCount = 2;
	if(!vga.init(pins, mode, 8)) {
        while(1) {
            delay(1);
        }
    }
	vga.start();

    // espnow init
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      return;
    }
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {
    if (currentState == MENU) {
        static int prevNextState = 1;
        static int prevBackState = 1;
        static int prevSelState = 1;
        int currentNextState = buttonState.down;
        int currentBackState = buttonState.up;
        int currentSelState = buttonState.start;

        if (currentBackState == 0 && currentBackState != prevBackState) {
            if (currentGameIndex == 0) {
                currentGameIndex = totalGames - 1;
            } else {
                currentGameIndex -= 1;
            }
        }

        if (currentNextState == 0 && currentNextState != prevNextState) {
            currentGameIndex = (currentGameIndex + 1) % totalGames; 
        }

        if (currentSelState == 0 && currentSelState != prevSelState) {
            currentState = LOADING;
        }

        showMainMenu();
        prevNextState = currentNextState;
        prevBackState = currentBackState;
        prevSelState = currentSelState;

    } else if (currentState == LOADING) { // Used to have a loading bar hence need for this state (it is now not needed but there is no need to change atm).
        currentState = PLAYING;
        vga.clear(vga.rgb(0, 0, 0));
        vga.show();

    } else if (currentState == PLAYING) {
        if (currentGameIndex == 0) {
            //space_warsMain();
            currentState = MENU;
        }
        else if (currentGameIndex == 1) { 
            SnakeMain();
            currentState = MENU;
        } else if (currentGameIndex == 2) {
            ConnectFourMain();
            currentState = MENU;
        }
    }
}