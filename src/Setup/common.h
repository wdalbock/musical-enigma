#ifndef COMMON_H
#define COMMON_H

// Structure declaration
typedef struct struct_message {
    int left;
    int right;
    int up;
    int down;
    int start;
    int back;
} struct_message;

// Declare the global button state variable
extern struct_message buttonState;

#endif
