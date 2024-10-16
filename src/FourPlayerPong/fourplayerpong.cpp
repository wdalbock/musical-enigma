#include <TFT_eSPI.h>
#include "../Setup/common.h"
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
    {200, 10, 140, 10, 10},  // Top paddle (Player 1)
    {200, 380, 140, 10, 10}, // Bottom paddle (Player 2)
    {100, 200, 10, 140, 10},  // Left paddle (Player 3)
    {470, 200, 10, 140, 10}, // Right paddle (Player 4)
};

int paddleDirection[4] = {1, 1, 1, 1};

Ball ball = {240, 200, 2, 2, 2}; // Start at the center with a direction

void drawPaddles() {
    for (int i = 0; i < 4; i++) {
        gfx.fillRect(paddles[i].x, paddles[i].y, paddles[i].width, paddles[i].height, TFT_WHITE);
    }
}

void drawBall() {
    gfx.fillCircle(ball.x, ball.y, 5, TFT_WHITE);
}

int updateBall() {
    // Clear previous ball position
    gfx.fillCircle(ball.x, ball.y, 5, TFT_BLACK);

    // Update ball position
    ball.x += ball.dx * ball.speed;
    ball.y += ball.dy * ball.speed;

    // Check for collisions with the paddles
    if (ball.y <= paddles[0].y + paddles[0].height && ball.x >= paddles[0].x && ball.x <= paddles[0].x + paddles[0].width) {
        ball.dy = -ball.dy; // Bounce off top paddle (Player 1)
    }
    if (ball.y >= paddles[1].y - 5 && ball.x >= paddles[1].x && ball.x <= paddles[1].x + paddles[1].width) {
        ball.dy = -ball.dy; // Bounce off bottom paddle (Player 2)
    }
    if (ball.x <= paddles[2].x + paddles[2].width && ball.y >= paddles[2].y && ball.y <= paddles[2].y + paddles[2].height) {
        ball.dx = -ball.dx; // Bounce off left paddle (Player 3)
    }
    if (ball.x >= paddles[3].x - 5 && ball.y >= paddles[3].y && ball.y <= paddles[3].y + paddles[3].height) {
        ball.dx = -ball.dx; // Bounce off right paddle (Player 4)
    }

    // Check for collisions with the screen boundaries
    if (ball.x <= 100 || ball.x >= 480) {
        return 1;
    }
    if (ball.y <= 10 || ball.y >= 390) {
        return 1;
    }

    drawBall();
    return 0;
}

void updatePaddles() {

    // Player 1: Top paddle moves left/right
    if (!buttonState.up) {
        paddles[0].x += paddles[0].speed * paddleDirection[0]; 

        // Reverse direction if it hits the left or right boundary
        if (paddles[0].x <= 100 || paddles[0].x + paddles[0].width >= 480) {
            paddleDirection[0] = -paddleDirection[0]; 
        }
    }

    // Player 2: Bottom paddle moves left/right
    if (!buttonState.down) {
        paddles[1].x += paddles[1].speed * paddleDirection[1]; 

        // Reverse direction if it hits the left or right boundary
        if (paddles[1].x <= 100 || paddles[1].x + paddles[1].width >= 480) {
            paddleDirection[1] = -paddleDirection[1]; 
        }
    }

    // Player 3: Left paddle moves up/down
    if (!buttonState.left) {
        paddles[2].y += paddles[2].speed * paddleDirection[2];

        // Reverse direction if it hits the top or bottom boundary
        if (paddles[2].y <= 10 || paddles[2].y + paddles[2].height >= 390) {
            paddleDirection[2] = -paddleDirection[2];
        }
    }

    // Player 4: Right paddle moves up/down
    if (!buttonState.right) {
        paddles[3].y += paddles[3].speed * paddleDirection[3];

        // Reverse direction if it hits the top or bottom boundary
        if (paddles[3].y <= 10 || paddles[3].y + paddles[3].height >= 390) {
            paddleDirection[3] = -paddleDirection[3]; 
        }
    }
}


void displayBounds() {
    vga.clear(vga.rgb(0, 0, 0));
    gfx.drawRect(100, 10, 380, 380, TFT_WHITE); 
    drawPaddles();  
    drawBall();     
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

void fourplayerpongmain() {
    displayBounds();
    while (true) {
        if(pongloop()) {
            break;
        }
    }
    // Reset ball position.
    ball = {240, 200, 2, 2, 2};
    return;
}
