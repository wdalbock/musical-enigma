#include <TFT_eSPI.h>  // Include the TFT display library
#include "Connect_Four.h"
 
extern TFT_eSPI tft; 
extern TFT_eSprite sprite;
 
uint16_t bg = TFT_BLACK;  // white
uint16_t fg = TFT_WHITE;  // black
uint16_t rg = TFT_RED;
uint16_t yg = TFT_YELLOW;
 
static int x = 150;
int mat[6][7] = {{0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}};  // Game board
static int player = 1;  // Player A starts
bool win = false;
 
int scoreA = 0;  // Score for Player A
int scoreB = 0;  // Score for Player B
 
const int buttonLeft = 1;   // Button pin for moving left
const int buttonRight = 2;  // Button pin for moving right
const int buttonSelect = 3; // Button pin for selecting
 
void displayScore() {
  tft.setTextSize(2);
 
  // Display Player A's score
  tft.setTextColor(rg);
  tft.fillRect(10, 10, 100, 20, bg);  // Clear previous score
  tft.setCursor(10, 10);
  tft.printf("A: %d", scoreA);
 
  // Display Player B's score
  tft.setTextColor(yg);
  tft.fillRect(10, 30, 100, 20, bg);  // Clear previous score
  tft.setCursor(10, 30);
  tft.printf("B: %d", scoreB);
}
 
void displayWinMessage(const char* msg, uint16_t color) {
  tft.setCursor(30, 70);
  tft.setTextColor(color, TFT_WHITE);
  tft.setTextSize(4);
  tft.println(msg);
}
 
void displayDrawMessage() {
  tft.setCursor(30, 70);
  tft.setTextColor(fg, bg);
  tft.setTextSize(4);
  tft.println("It's a draw!");
}
 
bool isColumnFull(int x) {
  int col = (x - 132) / 18 - 1;
  return mat[0][col] != 0;
}
 
void drawBoard() {
  tft.drawRect(150, 18, 126, 108, TFT_BLUE);  // Draw game grid
  for (int i = 1; i < 6; i++) {
    tft.drawLine(150, 18 + i * 18, 274, 18 + i * 18, TFT_BLUE);
  }
  for (int i = 1; i < 7; i++) {
    tft.drawLine(150 + i * 18, 18, 150 + i * 18, 126, TFT_BLUE);
  }
}
 
void drawIndicator() {
  tft.fillRect(150, 0, 126, 15, bg);  // Clear previous indicator
  tft.fillTriangle(x, 4, x + 18, 4, x + 9, 13, TFT_BLUE);  // Draw new indicator
}
 
void drawPlayer() {
  tft.fillRect(270, 135, 140, 98, bg);  // Clear the area
  tft.setCursor(270, 135);
 
  tft.setTextSize(4);
  if (player == 1) {
    tft.setTextColor(rg);
    tft.println("A");
  } else {
    tft.setTextColor(yg);
    tft.println("B");
  }
}

void displayStartScreen() {
  tft.setTextSize(2.5);
  tft.setTextColor(fg);
  tft.drawString("Connect", 10, 70);
  tft.setTextSize(5);
  tft.setTextColor(rg);
  tft.drawString("4", 100, 55);
  tft.setTextSize(1);
  tft.setTextColor(fg);
  tft.drawString("Made by group 24", 10, 95);
  tft.setTextSize(3);
  tft.drawString("Player:", 145, 140);
  drawBoard();
  drawPlayer();
  displayScore();
}
 
void drawPiece(int row, int col, int player) {
  int x = 159 + col * 18;
  int y = 27 + row * 18;
  if (player == 1) {
    tft.fillCircle(x, y, 5, rg);  // Player A piece
  } else {
    tft.fillRect(x - 2, y - 7, 4, 14, yg);  // Player B cross
    tft.fillRect(x - 7, y - 2, 14, 4, yg);
  }
}

void place(int player, int x) {
  int col = (x - 132) / 18 - 1;
  for (int i = 5; i >= 0; i--) {
    if (mat[i][col] == 0) {
      mat[i][col] = player;
      drawPiece(i, col, player);
      break;
    }
  }
}
 
bool check_if_4() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 7; j++) {
      if (mat[i][j] == mat[i + 1][j] && mat[i + 1][j] == mat[i + 2][j] && mat[i + 2][j] == mat[i + 3][j] && mat[i][j] != 0) {
        return true;
      }
    }
  }
 
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 4; j++) {
      if (mat[i][j] == mat[i][j + 1] && mat[i][j + 1] == mat[i][j + 2] && mat[i][j + 2] == mat[i][j + 3] && mat[i][j] != 0) {
        return true;
      }
    }
  }
 
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      if (mat[i][j] == mat[i + 1][j + 1] && mat[i + 1][j + 1] == mat[i + 2][j + 2] && mat[i + 2][j + 2] == mat[i + 3][j + 3] && mat[i][j] != 0) {
        return true;
      }
    }
  }
 
  for (int i = 0; i < 3; i++) {
    for (int j = 3; j < 7; j++) {
      if (mat[i][j] == mat[i + 1][j - 1] && mat[i + 1][j - 1] == mat[i + 2][j - 2] && mat[i + 2][j - 2] == mat[i + 3][j - 3] && mat[i][j] != 0) {
        return true;
      }
    }
  }
 
  return false;
}
 
bool isBoardFull() {
  for (int i = 0; i < 7; i++) {
    if (mat[0][i] == 0) {
      return false;
    }
  }
  return true;
}
 
void resetGame() {
  // Clear the matrix for a new game
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 7; j++) {
      mat[i][j] = 0;
    }
  }
 
  tft.fillRect(0, 0, 320, 240, bg);  // Clear the display
  tft.setTextSize(2.5);
  tft.setTextColor(fg);
  tft.drawString("Connect", 10,70);
  tft.setTextSize(5);
  tft.setTextColor(rg);
  tft.drawString("4", 100,55);
  tft.setTextSize(1);
  tft.setTextColor(fg);
  tft.drawString("Made by group 24", 10,95);
  tft.setTextSize(3);
  tft.drawString("Player:",145,140);
  drawBoard();
  player = 1;  // Reset to Player A
  drawPlayer();
  displayScore();  // Display updated scores
}

void ConnectFourLoop() {
  drawIndicator();  // Draw the player’s current selection indicator
 
  if (digitalRead(buttonSelect) == LOW) {
    delay(100);  // Debounce delay
   
    // Check if the column is full before allowing a piece to be placed
    if (!isColumnFull(x)) {
      place(player, x);
 
      // Check if a player has won
      if (check_if_4()) {
        if (player == 1) {
          scoreA++;  // Increment Player A's score
          displayWinMessage("Player A won", rg);  // Display win message for Player A
        } else {
          scoreB++;  // Increment Player B's score
          displayWinMessage("Player B won", yg);  // Display win message for Player B
        }
        delay(3000);
        resetGame();  // Reset game after a win
      }
     
      // Check if the board is full, in which case it's a draw
      else if (isBoardFull()) {
        displayDrawMessage();
        delay(3000);
        resetGame();  // Reset game after a draw
      } else {
        // Switch players only if a valid move was made and game is not over
        player = (player == 1) ? 2 : 1;
        drawPlayer();
      }
    } else {
      delay(1000);  // Pause for a second before letting the player try again
    }
  } else if (digitalRead(buttonLeft) == LOW) {
    delay(100);  // Debounce delay
    x -= 18;
    if (x < 150) x = 258;  // Wrap around if moving out of bounds on the left
  } else if (digitalRead(buttonRight) == LOW) {
    delay(100);  // Debounce delay
    x += 18;
    if (x > 258) x = 150;  // Wrap around if moving out of bounds on the right
  }
  delay(100);  // Main loop delay
}

void ConnectFourMain() {
  pinMode(buttonLeft, INPUT_PULLUP);  // Set button pins
  pinMode(buttonRight, INPUT_PULLUP);
  pinMode(buttonSelect, INPUT_PULLUP);
 
  tft.init();
  tft.setRotation(1);  // Rotate screen 90 degrees
  tft.fillScreen(bg);
 
  displayStartScreen();
  while(true) {
    ConnectFourLoop();
  }
}