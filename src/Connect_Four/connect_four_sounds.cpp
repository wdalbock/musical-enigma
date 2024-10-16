#include "../Setup/pitches.h"
#include <Arduino.h>
#include <stdlib.h>
#include <esp32-hal.h>

int song_melody[] = {

    NOTE_C4,4, NOTE_E4,4, NOTE_G4,4, NOTE_E4,4, 
    NOTE_C4,4, NOTE_E4,8, NOTE_G4,-4, NOTE_E4,4,
    NOTE_A3,4, NOTE_C4,4, NOTE_E4,4, NOTE_C4,4,
    NOTE_A3,4, NOTE_C4,8, NOTE_E4,-4, NOTE_C4,4,
    NOTE_G3,4, NOTE_B3,4, NOTE_D4,4, NOTE_B3,4,
    NOTE_G3,4, NOTE_B3,8, NOTE_D4,-4, NOTE_B3,4,

    NOTE_G3,4, NOTE_G3,8, NOTE_G3,-4, NOTE_G3,8, NOTE_G3,4, 
    NOTE_G3,4, NOTE_G3,4, NOTE_G3,8, NOTE_G3,4,
    NOTE_C4,4, NOTE_E4,4, NOTE_G4,4, NOTE_E4,4, 
    NOTE_C4,4, NOTE_E4,8, NOTE_G4,-4, NOTE_E4,4,
    NOTE_A3,4, NOTE_C4,4, NOTE_E4,4, NOTE_C4,4,
    NOTE_A3,4, NOTE_C4,8, NOTE_E4,-4, NOTE_C4,4,
    NOTE_G3,4, NOTE_B3,4, NOTE_D4,4, NOTE_B3,4,
    NOTE_G3,4, NOTE_B3,8, NOTE_D4,-4, NOTE_B3,4,

    NOTE_G3,-1,
  
};

static unsigned long previousMillis = 0;  
static int currentNote = 0;               
static bool songPlaying = false;          
static int buzzer = 43;                   
static int tempo = 160;                   
static int wholenote = (60000 * 4) / tempo;  
static int noteDuration = 0;              

void startKeyboardCat() {
    songPlaying = true;           
    currentNote = 0;              
    previousMillis = millis();    
}

void stopKeyboardCat() {
    noTone(buzzer);                
    songPlaying = false;    
}       

void updateKeyboardCat() {
    if (!songPlaying || currentNote >= sizeof(song_melody) / sizeof(song_melody[0])) {
        stopKeyboardCat();  
        return;
    }

    unsigned long currentMillis = millis();
    int divider = song_melody[currentNote + 1];

    if (divider > 0) {
        noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
        noteDuration = (wholenote) / abs(divider) * 1.5;  // Handle dotted notes
    }

    if (currentMillis - previousMillis >= noteDuration) {
        noTone(buzzer);
        currentNote += 2;

        if (currentNote < sizeof(song_melody) / sizeof(song_melody[0])) {
            tone(buzzer, song_melody[currentNote], noteDuration * 0.9);
        }

        previousMillis = currentMillis;
    }
}

void playPlaceSound() {
  tone(buzzer,NOTE_B5,50);
  tone(buzzer,NOTE_E6,400);
  noTone(buzzer);
}