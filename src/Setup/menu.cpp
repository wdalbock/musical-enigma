#include "../Space_wars/space_wars.h"
#include "../Connect_Four/Connect_Four.h"
#include "../Snake/Snake.h"
#include "common.h"
#include "leaderboard.h"
#include <SD_MMC.h>

#include "ESP32S3VGA.h"
#include "GfxWrapper.h"

#include <esp_now.h>
#include <WiFi.h>

#define PIN_SD_CMD    44
#define PIN_SD_CLK    18
#define PIN_SD_D0     17

const char* filenameSnake = "/Snake.txt";
static int snakeLeaderboard[5];
const char* filenameConnect = "/ConnectFour.txt";
static int connectLeaderboard[5];

//                   r,  r, r, r,r,   g, g, g, g, g,g,   b, b, b, b,b,  h, v
const PinConfig pins(-1,-1,-1,-1,1,  -1,-1,-1,-1,-1,2,  -1,-1,-1,-1,3,  10,11);

const char* games[] = {"Space Wars", "Snake", "Connect Four"}; 
const char* leaderboardMetrics[] = {"", "Size", "Moves to Win"};
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
    PLAYING,
    LEADERBOARD,
};

State currentState = MENU;

void showMainMenu() {
    vga.clear(vga.rgb(0, 0, 0)); // black
    gfx.setTextColor(0xFFFF); // white

    gfx.setTextSize(3);  
    gfx.setCursor(50, 75);
    gfx.print("ARCADE MENU"); 
    
    gfx.setTextSize(2); 

    for (int i = 0; i < totalGames; i++) {
        gfx.setCursor(35, 125 + 32 * i);

        if (i == currentGameIndex){
            gfx.setTextColor(0xFFE0);
            gfx.print("(A)");

            gfx.setCursor(350, 125 + 32 * i);
            gfx.print(">>");
            gfx.setCursor(375, 125 + 32 * i);
            gfx.print("Leaderboard");
        }
        else {
            gfx.setTextColor(0xFFFF); 
        }

        gfx.setCursor(75, 125 + 32 * i);
        gfx.print(games[i]);
    }
    vga.show();
}

void displayLeaderboard(int gameIndex) {
    const char* metric = leaderboardMetrics[gameIndex];
    vga.clear(vga.rgb(0, 0, 0));

    gfx.setTextColor(0xFFFF);

    gfx.setCursor(10, 50);
    gfx.setTextSize(2);  
    gfx.print(games[gameIndex]);

    gfx.setCursor(420, 50);
    gfx.print(metric);

    for (int i = 1; i <= 5; i++) {
        gfx.setCursor(20, 50 + i * 50);
        gfx.printf("%d", i);

        switch (i) {
        case 1:
            gfx.drawRect(15, 45 + i * 50, 500, 40, 0x07E0);
            break;
        case 2:
            gfx.drawRect(15, 45 + i * 50, 500, 40, 0xFFE0);
            break;
        case 3:
            gfx.drawRect(15, 45 + i * 50, 500, 40, 0xF00F);
            break;
        default:
            break;
        }

        gfx.setCursor(480, 50 + i * 50);

        int snakeval = snakeLeaderboard[i-1];
        int connectval = connectLeaderboard[i-1];
        switch (gameIndex) {
        case 0:
            break;
        case 1:
            if (snakeval == 0) {
                gfx.print("-");
            } else {
                gfx.print(snakeval);
            }
            break;
        case 2:
            if (connectval == 0) {
                gfx.print("-");
            } else {
                gfx.print(connectval);
            }
            break;
        default:
            break;
        }
    }

    gfx.setTextColor(0xFFE0);
    gfx.setCursor(0, 370);
    gfx.print("<- (B)");
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

    //sd card init
    SD_MMC.setPins(PIN_SD_CLK, PIN_SD_CMD, PIN_SD_D0);
    SD_MMC.begin("/SDCARD");
    Serial.begin(9600);
    if (!SD_MMC.begin("/SDCARD", true)) {
      Serial.println("SD Card Mount Failed");
      return;
    }

    readFile(filenameSnake, snakeLeaderboard);
    readFile(filenameConnect, connectLeaderboard);

    // espnow init
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
        static int prevRightState = 1;
        int currentNextState = buttonState.down;
        int currentBackState = buttonState.up;
        int currentSelState = buttonState.start;
        int currentRightState = buttonState.right;

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
            currentState = PLAYING;
        }

        if (currentRightState == 0 && currentRightState != prevRightState) {
            currentState = LEADERBOARD;
        }

        showMainMenu();
        prevNextState = currentNextState;
        prevBackState = currentBackState;
        prevSelState = currentSelState;
        prevRightState = currentRightState;

    } else if (currentState == PLAYING) {
        if (currentGameIndex == 0) {
            //space_warsMain();
            currentState = MENU;

        } else if (currentGameIndex == 1) { 
            addSnakeScore(SnakeMain(), snakeLeaderboard);
            writeScores(filenameSnake, snakeLeaderboard);
            delay(10);
            currentState = MENU;

        } else if (currentGameIndex == 2) {
            addConnectFourScore(ConnectFourMain(), connectLeaderboard);
            writeScores(filenameConnect, connectLeaderboard);
            delay(10);
            currentState = MENU;
        }
    } else if (currentState == LEADERBOARD) {
        displayLeaderboard(currentGameIndex);
        if (!buttonState.back) {
            currentState = MENU;
        }
    }
}