#include "../Space_wars/space_wars.h"
#include "../Connect_Four/Connect_Four.h"
#include "../Snake/Snake.h"

#include "ESP32S3VGA.h"
#include "GfxWrapper.h"

#include <esp_now.h>
#include <WiFi.h>

//                   r,  r, r, r,r,   g, g, g, g, g,g,   b, b, b, b,b,  h, v
const PinConfig pins(-1,-1,-1,-1,1,  -1,-1,-1,-1,-1,2,  -1,-1,-1,-1,3,  10,11);

const char*games[] = {"Space Wars", "Snake", "Connect"}; 
int currentGameIndex = 0; 
int totalGames = sizeof(games)/ sizeof(games[0]);

VGA vga;
Mode mode = Mode::MODE_640x480x60;
int width = 640;
int height = 480;
GfxWrapper<VGA> gfx(vga, mode.hRes, mode.vRes);

typedef struct struct_message {
    int left;
    int right;
    int up;
    int down;
    int start;
    int back;
} struct_message;

static struct_message input;

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

// void loadingAnimation(int x, int y, int w, int h) {
//     float widthFraction = w / 100.0;
//     for (int i = 1; i <= 100; i++) {
//         gfx.drawRect(x, y, w, h, 0xFFFF);
//         vga.clear(vga.rgb(0, 0, 0));
//         gfx.fillRect(x, y, widthFraction * i, h, 0xFFFF);
//         vga.show();
//     }
// }

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  Serial.println("data received");
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  memcpy(&input, incomingData, sizeof(input));
}

void setup() {
    vga.bufferCount = 2;
	if(!vga.init(pins, mode, 8)) {
        while(1) {
            delay(1);
        }
    }
	vga.start();

    // temp ----------------------------------------------
    pinMode(0, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
    // ---------------------------------------------------

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
        static int prevNextState = LOW;
        static int prevBackState = LOW;
        static int prevSelState = LOW;
        int currentNextState = input.down;
        int currentBackState = input.back;
        int currentSelState = input.start;

        // temp --------------------------------
        int d = digitalRead(0);
        int s = digitalRead(14);
        // -------------------------------------

        if (currentBackState == HIGH && currentBackState != prevBackState) {
            if (currentGameIndex == 0) {
                currentGameIndex = totalGames - 1;
            } else {
                currentGameIndex -= 1;
            }
        }

        if (currentNextState == HIGH && currentNextState != prevNextState || d == 0) {
            currentGameIndex = (currentGameIndex + 1) % totalGames; 
        }

        if (currentSelState == HIGH && currentSelState != prevSelState || s == 0) {
            currentState = LOADING;
        }

        showMainMenu();
        prevNextState = currentNextState;
        prevBackState = currentBackState;
        prevSelState = currentSelState;

    } else if (currentState == LOADING) {
        currentState = PLAYING;
        // loadingAnimation(35, 240, 520, 30);
        vga.clear(vga.rgb(0, 0, 0));
        vga.show();

    } else if (currentState == PLAYING) {
        if (currentGameIndex == 0) {
            space_warsMain();
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