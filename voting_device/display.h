#ifndef DISPLAY_H
#define DISPLAY_H
#include <SPI.h>
#include "src/epd1in54_V2.h"
#include "src/epdpaint.h"
#include <stdio.h>

extern Epd epd;

#define COLORED     0
#define UNCOLORED   1

//Display functions called in voting_device.ino
void initDisplay();

void paintVoteScreen(const char *question, int batteryLevel);

void paintConfirmScreen(const char *response, int batteryLevel);

void paintClosingScreen();

//Unused in voting_device.ino, could be removed from this .h file
void display_question(const char *question, char *line, int *position);

void drawImage(int x, int y, int width, int height, const unsigned char *image);

#endif