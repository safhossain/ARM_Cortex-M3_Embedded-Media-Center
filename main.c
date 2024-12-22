/*
*
* Author: Safwan Hossain
* COE718-F2024 Embedded Systems
* Project: Multimedia Center Embedded in LPC1768 Microprocessor on MCB1700 Dev Board
* Module: Main
* 
*/

#include <LPC17xx.h>
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"
#include <string.h>
#include <stdio.h>
#include "display.h"

#define __FI 1

typedef struct {
    char text[21];	
    int (*function)(void);
} MenuOption;
// external applications
extern int showPhotos(void);
extern int music_time(void);
extern int game1(void);
extern int game2(void);
extern int game3(void);
// Menu Options Array
MenuOption menuOptions[] = {
    {"Photos Gallery", showPhotos},
    {"Audio Player", music_time},
    {"Maze Game", game1},
	{"Tic Tac Toe", game2},
	{"Dragon Warrior Scroll", game3}
};
#define NUM_MENU_OPTIONS (sizeof(menuOptions) / sizeof(MenuOption))

void menu(void);

int main(void) {
	LED_Init();
	KBD_Init();
	GLCD_Init();
	
  GLCD_Clear(White);	
	
	displayTypeOfText(0, NAV_BAR, "COE718-F2024");
	displayTypeOfText(1, NAV_BAR, "FINAL PROJECT DEMO");
  
	displayTypeOfText(2, DEFAULT_MIDDLE, "");
	displayTypeOfText(3, DEFAULT_MIDDLE, "");
	displayTypeOfText(4, DEFAULT_MIDDLE, "");
	displayTypeOfText(5, DEFAULT_MIDDLE, "Press SEL to Start");
	displayTypeOfText(6, DEFAULT_MIDDLE, "");
	displayTypeOfText(7, DEFAULT_MIDDLE, "");
	
	displayTypeOfText(8, NAV_BAR, "");
	printLCD(9, Black, Yellow, "SAFWAN HOSSAIN", JUSTIFY_MIDDLE);	
	
	while(1) {
		if(get_button() == KBD_SELECT) {
				menu();
		}
	}
}

void menu() {	
	int i;
	int j;
  volatile int delay;
	int currentOption = 0;
	int joystickVal;
	
	GLCD_Clear(White);	
	while (1) {
		
		displayTypeOfText(0, NAV_BAR, "");
		displayTypeOfText(1, NAV_BAR, "Choose App");		
		displayTypeOfText(2, DEFAULT_MIDDLE, "");
		
		/* Display the menu options */
		for (i = 0; i < NUM_MENU_OPTIONS; i++) {
			if (i == currentOption) { // Highlight the active option				
				printLCD(3 + i, Blue, Yellow, menuOptions[i].text, JUSTIFY_MIDDLE);
			} else { // Non-active options				
				printLCD(3 + i, Maroon, Green, menuOptions[i].text, JUSTIFY_MIDDLE);
			}
		}
		
		for (j=7; j>=NUM_MENU_OPTIONS+3; j--){
			displayTypeOfText(j, DEFAULT_MIDDLE, "");
		}
		
		displayTypeOfText(8, NAV_BAR, "");
		displayTypeOfText(9, NAV_BAR, "UP/DOWN + SELECT");
		displayTypeOfText(10, NAV_BAR, "");
		
		while (1) {
			joystickVal = get_button();
			if (joystickVal != 0) {
					break;
			}			
		}
		
		if (joystickVal == KBD_DOWN) { // Move to the next option, loop back to the top if at the end			
			currentOption = (currentOption + 1) % NUM_MENU_OPTIONS;
		} else if (joystickVal == KBD_UP) { // Move to the previous option, loop to the bottom if at the top			
			currentOption = (currentOption - 1 + NUM_MENU_OPTIONS) % NUM_MENU_OPTIONS;
		} else if (joystickVal == KBD_SELECT) { // Execute the selected function			
			GLCD_Clear(White);
			menuOptions[currentOption].function();
			
			currentOption = 0; // for after function() return
		}		
		
		for (delay = 0; delay < 10000; delay++); // adding a small delay for button presses if necessary
	}
}
