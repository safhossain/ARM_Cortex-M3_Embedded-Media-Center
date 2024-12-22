#ifndef DISPLAY_H
	#define DISPLAY_H

	#define __FI 1
	#define JUSTIFY_MIDDLE 0
	#define JUSTIFY_LEFT 1
	#define NAV_BAR 0
	#define DEFAULT_MIDDLE 1
	#define DEFAULT_LEFT 2

	#include "GLCD.h"

	void displayTypeOfText(int row, int type, char text[21]);
	void printLCD(int row, unsigned short backgroundColor, unsigned short textColor, const char *text, int justify);

#endif
