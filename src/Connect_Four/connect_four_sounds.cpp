#include "../Setup/pitches.h"
#include <Arduino.h>
#include <stdlib.h>
#include <esp32-hal.h>

int song_melody[] = {

  // Keyboard cat
  // Score available at https://musescore.com/user/142788/scores/147371

    NOTE_C4,4, NOTE_E4,4, NOTE_G4,4, NOTE_E4,4, 
    NOTE_C4,4, NOTE_E4,8, NOTE_G4,-4, NOTE_E4,4,
    NOTE_A3,4, NOTE_C4,4, NOTE_E4,4, NOTE_C4,4,
    NOTE_A3,4, NOTE_C4,8, NOTE_E4,-4, NOTE_C4,4,
    NOTE_G3,4, NOTE_B3,4, NOTE_D4,4, NOTE_B3,4,
    NOTE_G3,4, NOTE_B3,8, NOTE_D4,-4, NOTE_B3,4,

    NOTE_G3,4, NOTE_G3,8, NOTE_G3,-4, NOTE_G3,8, NOTE_G3,4, 
    // NOTE_G3,4, NOTE_G3,4, NOTE_G3,8, NOTE_G3,4,
    // NOTE_C4,4, NOTE_E4,4, NOTE_G4,4, NOTE_E4,4, 
    // NOTE_C4,4, NOTE_E4,8, NOTE_G4,-4, NOTE_E4,4,
    // NOTE_A3,4, NOTE_C4,4, NOTE_E4,4, NOTE_C4,4,
    // NOTE_A3,4, NOTE_C4,8, NOTE_E4,-4, NOTE_C4,4,
    // NOTE_G3,4, NOTE_B3,4, NOTE_D4,4, NOTE_B3,4,
    // NOTE_G3,4, NOTE_B3,8, NOTE_D4,-4, NOTE_B3,4,

    // NOTE_G3,-1,
  
};

unsigned long previousMillis = 0;  // Store the last time a note was played
int currentNote = 0;               // Track the current note being played
bool songPlaying = false;          // Flag to indicate if a song is currently playing
int buzzer = 43;                   // Your buzzer pin
int tempo = 160;                   // Song tempo
int wholenote = (60000 * 4) / tempo;  // Duration of a whole note in ms
int noteDuration = 0;              // Duration of the current note

void startSong() {
    songPlaying = true;            // Start the song
    currentNote = 0;               // Start from the first note
    previousMillis = millis();     // Store the current time
}

void stopSong() {
    noTone(buzzer);                // Stop the buzzer
    songPlaying = false;           // Reset the flag
}

void updateSong() {
    if (!songPlaying || currentNote >= sizeof(song_melody) / sizeof(song_melody[0])) {
        stopSong();  // Stop if the song is finished
        return;
    }

    unsigned long currentMillis = millis();
    int divider = song_melody[currentNote + 1];

    // Calculate the duration of the note
    if (divider > 0) {
        noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
        noteDuration = (wholenote) / abs(divider) * 1.5;  // Handle dotted notes
    }

    // If enough time has passed since the last note, play the next note
    if (currentMillis - previousMillis >= noteDuration) {
        // Stop the previous note
        noTone(buzzer);

        // Move to the next note
        currentNote += 2;

        // Play the new note, if there are more notes to play
        if (currentNote < sizeof(song_melody) / sizeof(song_melody[0])) {
            tone(buzzer, song_melody[currentNote], noteDuration * 0.9);
        }

        // Update the timer for the next note
        previousMillis = currentMillis;
    }
}

void playPlaceSound() {
  tone(buzzer,NOTE_B5,50);
  tone(buzzer,NOTE_E6,400);
  noTone(buzzer);
}