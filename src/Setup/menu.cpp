#include <TFT_eSPI.h>
#include "../Space_wars/space_wars.h"
#include "fonts/pixel.h"
#include "../Snake/Snake.h"

#define Pin_Button_Next 0
#define Pin_Button_Select 14

const char*games[] = {"Space Wars", "Snake"}; 
int currentGameIndex = 0; 
int totalGames = sizeof(games)/ sizeof(games[0]); 

TFT_eSPI tft= TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

void showMainMenu() {
    tft.fillScreen(TFT_BLACK); 
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.setFreeFont(&TomThumb); 
    tft.setTextSize(3); 
    tft.setTextDatum(MC_DATUM); 
    tft.drawString("ARCADE MENU", tft.width() / 2, 30); 

    tft.setFreeFont(&TomThumb); 
    tft.setTextSize(2); 

    int textHeight = tft.fontHeight(); 
    int screenWidth = tft.width(); 
    int screenHeight = tft.height(); 
    int yOffset = (screenHeight - (totalGames * (textHeight + 10))) / 2;

    for (int i = 0; i < totalGames; i++) {
        int textWidth = tft.textWidth(games[i]); 

        if (i == currentGameIndex){

            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.drawString(">>", (screenWidth - textWidth)/2 - 25, yOffset + (i * (textHeight + 10))); 
        }
        else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK); 
        }
        tft.drawString(games[i], screenWidth/2, yOffset + (i * (textHeight + 10))); 
    }
} 

void loadingTextAnimation(int duration) {
    tft.fillScreen(TFT_BLACK); 
    int barWidth = tft.width() - 40;
    int barHeight = 10;
    int barX = (tft.width() - barWidth) / 2;
    int barY = tft.height() / 2 + 20;  // Position the bar below the text
    int textX = tft.width() / 2;
    int textY = tft.height() / 2 - 20;  // Position the text above the bar
    unsigned long startTime = millis();
    unsigned long currentTime = 0;
    int numDots = 0;

    while (millis() - startTime < duration) {
        currentTime = millis() - startTime;
        int progress = (currentTime * barWidth) / duration;  // Calculate progress based on time

        // Clear previous frame
        tft.fillScreen(TFT_BLACK);

        // Draw the "LOADING..." text
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.setFreeFont(&TomThumb); 
        tft.setTextSize(2);
        tft.drawString("LOADING", textX, textY);

        // Draw dots animation
        for (int i = 0; i < numDots; i++) {
            tft.drawString(".", textX + 60 + (i * 6), textY);
        }
        
        numDots = (numDots + 1) % 4;  // Cycle between 0 to 3 dots

        // Draw the progress bar border
        tft.drawRect(barX, barY, barWidth, barHeight, TFT_WHITE);

        // Fill the progress bar based on the current progress
        tft.fillRect(barX + 1, barY + 1, progress - 2, barHeight - 2, TFT_WHITE);

        delay(100);  // Adjust speed of animation
    }

    // Clear screen after loading
    tft.fillScreen(TFT_BLACK);
}

void setup() {
    tft.init(); 
    tft.setRotation(1); 
    tft.setFreeFont(&TomThumb); 


    pinMode(Pin_Button_Next, INPUT_PULLUP); 
    pinMode(Pin_Button_Select, INPUT_PULLUP); 

    showMainMenu(); 
}

void loop() {

    if (digitalRead(Pin_Button_Next) == LOW) {
        currentGameIndex = (currentGameIndex + 1) % totalGames; 
        showMainMenu(); 
        delay(200); 
    }

    if (digitalRead(Pin_Button_Select) == LOW) {
        if (currentGameIndex == 0) {
            loadingTextAnimation(3000); 
            space_warsMain(); 
        }
        else if (currentGameIndex == 1) {
            loadingTextAnimation(3000); 
            SnakeMain(); 
        }
        delay(200); 
    }

}
 

