#include <SD_MMC.h>
#include "FS.h"

bool fileExists(const char* filepath) {
  Serial.println(SD_MMC.exists(filepath));
  return SD_MMC.exists(filepath);
}

void writeScores(const char* filename, int* scoreArray) {
  fs::File file = SD_MMC.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    Serial.println(filename);
    return;
  }

  for (int i = 0; i < 5; i++) {
    file.println(scoreArray[i]);
    Serial.println(scoreArray[i]);
  }
  file.close();
  Serial.println("Integers written to file successfully");
}

void clearFile(const char* path) {
    if (SD_MMC.remove(path)) {
        Serial.println("File cleared successfully.");
    } else {
        Serial.println("Failed to clear the file.");
    }
}

void readFile(const char* filename, int* numbers) {
    fs::File file = SD_MMC.open(filename, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
    }

    size_t count = 0;
    while (file.available() && count < 5) {
        numbers[count++] = file.parseInt();
        file.read(); // Read the newline character
    }

    file.close();
    Serial.println("Integers read from file successfully");
}

void addSnakeScore(int score, int* leaderboard) {
  int currentScore = score;
  int previousScore = leaderboard[0];
  for (int i = 0; i < 5; i++) {
    if (leaderboard[i] < currentScore) {
      previousScore = leaderboard[i];
      leaderboard[i] = currentScore;
      currentScore = previousScore;
    } else if (leaderboard[i] == currentScore) { //if it's a repeat score, skip
      break;
    }
  }
}

void addConnectFourScore(int score, int* leaderboard) {
  int currentScore = score;
  int previousScore = leaderboard[0];

  for (int i = 0; i < 5; i++) {
    if (leaderboard[i] == 0) {
      leaderboard[i] = currentScore;
      break;
    }

    if (leaderboard[i] > currentScore) {
      previousScore = leaderboard[i];
      leaderboard[i] = currentScore;
      currentScore = previousScore; 
    } else if (leaderboard[i] == currentScore) {
      break;
    }
  }
}

// void leaderboards() {
//     tft.fillScreen(TFT_BLACK);
//     tft.setTextSize(3);
//     tft.setTextColor(0xFEE7);
//     tft.drawString("LEADERBOARD", tft.width() / 2, 20);
//     tft.setTextDatum(MC_DATUM);
//     tft.setFreeFont(&TomThumb);
//     tft.setTextSize(2);
//     tft.setTextColor(TFT_WHITE);
//     tft.fillRect(tft.width() / 4, 40, 160, 100, TFT_BLACK);
//     Serial.println("what's in the file?");
//     // Snake
//     if (fileExists(filenameSnake) == 0) { // if file does not exist, then make the file with zeroes
//         Serial.println("The file doesn't exist!");
//         writeScores(filenameSnake, snakeLeaderboard);
//     } else {
//         Serial.println("the file does exist!");
//         readFile(filenameSnake, snakeLeaderboard);
//     }
//     // Connect 4
//     if (fileExists(filenameConnect) == 0) {
//         Serial.println("The file doesn't exist!");
//         writeScores(filenameConnect, connectLeaderboard);
//     } else {
//         Serial.println("the file does exist!");
//         readFile(filenameConnect, connectLeaderboard);
//     }
//     for (int i = 0; i < 5; i++) {
//         tft.drawString(String(i+1) + "..............................." + String(snakeLeaderboard[i]), tft.width() / 3, 100 + space);
//         tft.drawString(String(i+1) + "..............................." + String(connectLeaderboard[i], 2*tft.width() / 3, 100 + space))
//         space = space + 20;
//     }
// }