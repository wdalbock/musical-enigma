#include <TFT_eSPI.h>  // Include the TFT display library
#include "Connect_Four.h"
#include "../Setup/common.h"
#include "connect_four_sounds.h"

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
 
uint16_t bg = TFT_BLACK;  // white
uint16_t fg = TFT_WHITE;  // black
uint16_t rg = TFT_RED;
uint16_t yg = TFT_YELLOW;
 
static int x = 150;
int mat[6][7] = {{0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}};  // Game board
static int player = 1;  // Player A starts
bool win = false;
int moveCount = 0;
 
int scoreA = 0;  // Score for Player A
int scoreB = 0;  // Score for Player B
 
void displayScore() {
  gfx.setTextSize(2);
 
  // Display Player A's score
  gfx.setTextColor(0xf800); // red
  gfx.setCursor(10, 10);
  gfx.print("A: ");
  gfx.println(scoreA);
 
  // Display Player B's score
  gfx.setTextColor(0xffe0); // yellow
  gfx.setCursor(10, 30);
  gfx.print("B: ");
  gfx.println(scoreB);
}
 
void displayWinMessage(const char* msg, uint16_t color) {
  gfx.setCursor(30, 70);
  gfx.setTextColor(color, TFT_WHITE);
  gfx.setTextSize(4);
  gfx.println(msg);
}
 
void displayDrawMessage() {
  gfx.setCursor(30, 70);
  gfx.setTextColor(fg, bg);
  gfx.setTextSize(4);
  gfx.println("It's a draw!");
}
 
bool isColumnFull(int x) {
  int col = (x - 132) / 18 - 1;
  return mat[0][col] != 0;
}

void drawPiece(int row, int col, int player) {
  int x = 159 + col * 18;
  int y = 27 + row * 18;
  if (player == 1) {
    gfx.fillCircle(x, y, 5, rg);  // Player A piece
  } else {
    gfx.fillRect(x - 2, y - 7, 4, 14, yg);  // Player B cross
    gfx.fillRect(x - 7, y - 2, 14, 4, yg);
  }
}

void drawPieces() {
  for (int row = 0; row < 6; row++) {
    for (int col = 0; col < 7; col++) {
      if (mat[row][col] == 0) {
        continue;
      }
      drawPiece(row, col, mat[row][col]);
    }
  }
  return;
}

void drawBoard() {
  gfx.drawRect(150, 18, 126, 108, TFT_BLUE);  // Draw game grid
  for (int i = 1; i < 6; i++) {
    gfx.drawLine(150, 18 + i * 18, 274, 18 + i * 18, TFT_BLUE);
  }
  for (int i = 1; i < 7; i++) {
    gfx.drawLine(150 + i * 18, 18, 150 + i * 18, 126, TFT_BLUE);
  }
  drawPieces();
}
 
void drawIndicator() {
  gfx.fillTriangle(x, 4, x + 18, 4, x + 9, 13, TFT_BLUE);  // Draw new indicator
}

void displayMoveCount() {
  gfx.setCursor(145, 180);
  gfx.setTextColor(TFT_WHITE);
  gfx.printf("Moves: %d", moveCount);
}
 
void drawPlayer() {
  gfx.setCursor(270, 135);
 
  gfx.setTextSize(4);
  if (player == 1) {
    gfx.setTextColor(rg);
    gfx.println("A");
  } else {
    gfx.setTextColor(yg);
    gfx.println("B");
  }
}

void displayStartScreen() {
  gfx.setTextSize(2.5);
  gfx.setTextColor(fg);
  gfx.setCursor(10, 70);
  gfx.print("Connect");

  gfx.setTextSize(5);
  gfx.setTextColor(rg);
  gfx.setCursor(100, 55);
  gfx.print("4");

  gfx.setTextSize(1);
  gfx.setTextColor(fg);
  gfx.setCursor(10, 95);
  gfx.print("Made by group 24");

  gfx.setTextSize(3);
  gfx.setCursor(145, 140);
  gfx.print("Player:");

  drawBoard();
  drawPlayer();
  displayScore();
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
 
  gfx.setTextSize(2.5);
  gfx.setTextColor(fg);
  gfx.setCursor(10, 70);
  gfx.print("Connect");

  gfx.setTextSize(5);
  gfx.setTextColor(rg);
  gfx.setCursor(100, 55);
  gfx.print("4");

  gfx.setTextSize(1);
  gfx.setTextColor(fg);
  gfx.setCursor(10, 95);
  gfx.print("Made by group 24");

  gfx.setTextSize(3);
  gfx.setCursor(145, 140);
  gfx.print("Player:");

  drawBoard();
  player = 1;  // Reset to Player A
  drawPlayer();
  displayScore();  // Display updated scores
}

int ConnectFourLoop() {
  static int prevPlace = 1;
  static int prevMoveLeft = 1;
  static int prevMoveRight = 1;
  int currentPlace = buttonState.start;
  int currentMoveLeft = buttonState.left;
  int currentMoveRight = buttonState.right;

  vga.clear(vga.rgb(0, 0, 0)); 
  displayStartScreen();
  drawIndicator();  // Draw the player’s current selection indicator
  displayMoveCount();
  vga.show();

  updateKeyboardCat();
 
  if (currentPlace == 0 && currentPlace != prevPlace) {
  
    // Check if the column is full before allowing a piece to be placed
    if (!isColumnFull(x)) {
      place(player, x);
      moveCount++;
      playPlaceSound();
 
      // Check if a player has won
      if (check_if_4()) {
        if (player == 1) {
          scoreA++;  // Increment Player A's score
          displayWinMessage("Player A won", rg);  // Display win message for Player A
          vga.show();
        } else {
          scoreB++;  // Increment Player B's score
          displayWinMessage("Player B won", yg);  // Display win message for Player B
          vga.show();
        }
        delay(3000);
        resetGame();  // Reset game after a win
        return 1;
      }
     
      // Check if the board is full, in which case it's a draw
      else if (isBoardFull()) {
        displayDrawMessage();
        vga.show();
        delay(3000);
        resetGame();  // Reset game after a draw
        return 1;
      } else {
        // Switch players only if a valid move was made and game is not over
        player = (player == 1) ? 2 : 1;
        drawPlayer();
      }
    }
  } else if (currentMoveLeft == 0 && currentMoveLeft != prevMoveLeft) {
    x -= 18;
    if (x < 150) x = 258;  // Wrap around if moving out of bounds on the left
  } else if (currentMoveRight == 0 && currentMoveRight != prevMoveRight) {
    x += 18;
    if (x > 258) x = 150;  // Wrap around if moving out of bounds on the right
  }

  prevPlace = currentPlace;
  prevMoveLeft = currentMoveLeft;
  prevMoveRight = currentMoveRight;

  return 0;
}

int ConnectFourMain() {
  delay(1000);
  startKeyboardCat();
  while(true) {
    if(ConnectFourLoop()) {
      break;
    }
  }
  int temp = moveCount;
  moveCount = 0;
  return temp;
}