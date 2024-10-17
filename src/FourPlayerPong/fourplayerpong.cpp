#include <TFT_eSPI.h>
#include "../Setup/common.h"
#include "ESP32S3VGA.h"
#include "GfxWrapper.h"
#include <esp_now.h>
#include <WiFi.h>
#include <stdlib.h>
#include <time.h>
#include "../Setup/pitches.h"
#include <Arduino.h>
#include <stdlib.h>
#include <esp32-hal.h>

extern VGA vga;
extern Mode mode;
extern int width;
extern int height;
extern GfxWrapper<VGA> gfx;
extern struct_message buttonState;
extern void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len);

struct Paddle {
    int x, y;           // Position of the paddle
    int width, height;  // Size of the paddle
    int speed;          // Paddle movement speed
};

struct Ball {
    int x, y;           // Ball position
    int dx, dy;         // Ball movement direction
    int speed;          // Ball speed
};

// Define paddles for 4 players (top, bottom, left, right)
Paddle paddles[4] = {
    {200, 0, 150, 10, 10},  // Top paddle (Player 1)
    {200, 390, 150, 10, 10}, // Bottom paddle (Player 2)
    {90, 200, 10, 150, 10},  // Left paddle (Player 3)
    {480, 200, 10, 150, 10}, // Right paddle (Player 4)
};

int paddleDirection[4] = {1, 1, 1, 1}; // Used to determine which way the paddles are currently moving.

int isMoving[4] = {0, 0, 0, 0}; // Used to determine whether paddles are moving or not.

int collisions = 0; // Total number of collisions.

Ball ball = {0, 0, 0, 0, 2}; // Start at the center with a direction

void drawPaddles() {
    for (int i = 0; i < 4; i++) {
        gfx.fillRect(paddles[i].x, paddles[i].y, paddles[i].width, paddles[i].height, TFT_WHITE);
    }
}

void drawBall() {
    gfx.fillCircle(ball.x, ball.y, 5, TFT_WHITE);
}

void drawCollisions() {
    gfx.setCursor(10, 10);
    gfx.setTextColor(TFT_WHITE);
    gfx.setTextSize(2);
    gfx.printf("Collisions: %d", collisions);
}

void randomiseBall() {
    srand(time(0));
    ball.x = (rand() % 180) + 200;
    ball.y = (rand() % 180) + 110;

    int dx = (rand() % 5) - 2;
    int dy = (rand() % 5) - 2;

    if (dx == 0) {
        dx = 1;
    }
    if (dy == 0) {
        dy = 1;
    }

    ball.dx = dx;
    ball.dy = dy;
}

void displayGameOver() {
    vga.clear(vga.rgb(0, 0, 0));
    gfx.setCursor(100, 150);
    gfx.setTextSize(3);
    gfx.setTextColor(TFT_WHITE);
    gfx.print("GAME OVER");

    gfx.setCursor(100, 200);
    gfx.setTextSize(2);
    gfx.printf("You made %d collisions.", collisions);
    vga.show();
}

void playCollisionSound() {
  tone(43, NOTE_B0, 150);
  noTone(43);
}

int updateBall() {
    static int increase = 0;

    // Clear previous ball position
    gfx.fillCircle(ball.x, ball.y, 5, TFT_BLACK);

    // Update ball position
    ball.x += ball.dx * ball.speed;
    ball.y += ball.dy * ball.speed;

    // Check for collisions with the paddles
    if (ball.y <= 15 && ball.x >= paddles[0].x && ball.x <= paddles[0].x + paddles[0].width) {
        ball.dy = -ball.dy; // Bounce off the top paddle (player 1) 
        ball.dx = ball.dx + 2 * isMoving[0] * ((paddles[0].speed * paddleDirection[0])/10); // The speed of the ball is influenced by the current speed/direction of the paddle.
        // If isMoving[0] = 0, the ball speed remains the same. 
        collisions++;
        increase = 1;
        playCollisionSound();
    }

    if (ball.y >= 385 && ball.x >= paddles[1].x && ball.x <= paddles[1].x + paddles[1].width) {
        ball.dy = -ball.dy; 
        ball.dx = ball.dx + 2 * isMoving[1] * ((paddles[1].speed * paddleDirection[1])/10);
        collisions++;
        increase = 1;
        playCollisionSound();
    }

    if (ball.x <= 105 && ball.y >= paddles[2].y && ball.y <= paddles[2].y + paddles[2].height) {
        ball.dx = -ball.dx; 
        ball.dy = ball.dy + 2 * isMoving[2] * ((paddles[2].speed * paddleDirection[2])/10);
        collisions++;
        increase = 1;
        playCollisionSound();
    }

    if (ball.x >= 475 && ball.y >= paddles[3].y && ball.y <= paddles[3].y + paddles[3].height) {
        ball.dx = -ball.dx;
        ball.dy = ball.dy + 2 * isMoving[3] * ((paddles[3].speed * paddleDirection[3])/10);
        collisions++;
        increase = 1;
        playCollisionSound();
    }

    // Check for collisions with the screen boundaries
    if (ball.x <= 100 || ball.x >= 480) {
        return 1;
    }
    if (ball.y <= 10 || ball.y >= 390) {
        return 1;
    }

    // Increase speed of ball every five collisions.
    if (increase && collisions % 5 == 0) {
        ball.speed++;
        increase = 0;
    }

    drawBall();
    return 0;
}

void updatePaddles() {

    // Player 1: Top paddle moves left/right
    if (!buttonState.up) {
        isMoving[0] = 1;
        paddles[0].x += paddles[0].speed * paddleDirection[0]; 

        // Reverse direction if it hits the left or right boundary
        if (paddles[0].x <= 100 || paddles[0].x + paddles[0].width >= 480) {
            paddleDirection[0] = -paddleDirection[0]; 
        }
    } else {
        isMoving[0] = 0;
    }

    // Player 2: Bottom paddle moves left/right
    if (!buttonState.down) {
        isMoving[1] = 1;
        paddles[1].x += paddles[1].speed * paddleDirection[1]; 

        // Reverse direction if it hits the left or right boundary
        if (paddles[1].x <= 100 || paddles[1].x + paddles[1].width >= 480) {
            paddleDirection[1] = -paddleDirection[1]; 
        }
    } else {
        isMoving[1] = 0;
    }

    // Player 3: Left paddle moves up/down
    if (!buttonState.left) {
        isMoving[2] = 1;
        paddles[2].y += paddles[2].speed * paddleDirection[2];

        // Reverse direction if it hits the top or bottom boundary
        if (paddles[2].y <= 10 || paddles[2].y + paddles[2].height >= 390) {
            paddleDirection[2] = -paddleDirection[2];
        }
    } else {
        isMoving[2] = 0;
    }

    // Player 4: Right paddle moves up/down
    if (!buttonState.right) {
        isMoving[3] = 1;
        paddles[3].y += paddles[3].speed * paddleDirection[3];

        // Reverse direction if it hits the top or bottom boundary
        if (paddles[3].y <= 10 || paddles[3].y + paddles[3].height >= 390) {
            paddleDirection[3] = -paddleDirection[3]; 
        }
    } else {
        isMoving[3] = 0;
    }
}


void displayBounds() {
    vga.clear(vga.rgb(0, 0, 0));
    gfx.drawRect(100, 10, 380, 380, TFT_WHITE);
    drawPaddles();  
    drawBall();  
    drawCollisions();   
    vga.show();
}

int pongloop() {
    // Main game loop to update paddles and ball
    updatePaddles();
    if (updateBall()) {
        return 1;
    }
    displayBounds();
    return 0;
}

int fourplayerpongmain() {
    randomiseBall();
    displayBounds();
    while (true) {
        if(pongloop()) {
            break;
        }
    }
    displayGameOver();
    delay(3000);

    ball.speed = 2;
    int temp = collisions;
    collisions = 0;
    return temp;
}