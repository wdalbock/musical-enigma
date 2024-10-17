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

void addDescendingScore(int score, int* leaderboard) {
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

void addAscendingScore(int score, int* leaderboard) {
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