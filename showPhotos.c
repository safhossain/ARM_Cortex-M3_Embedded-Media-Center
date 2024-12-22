/*
* 
* Author: Safwan Hossain
* COE718-F2024 Embedded Systems
* Project: Multimedia Center Embedded in LPC1768 Microprocessor on MCB1700 Dev Board
* Module: Photo Gallery
* 
*/

#include "display.h"
#include <LPC17xx.H>                    
#include "GLCD.h"
#include "KBD.h"
#include "display.h"

#include "lfc.c"
#include "onepiece.c"
#include "stardew.c"
#include "tmu.c"
#include "tmuNot.c"

#define NUM_PHOTOS 5

int showPhotos(void){
	int photoNum = 0;
	int joystickVal = 0;
	int delay;
	int j;
	
	while(1){			
		displayTypeOfText(0, NAV_BAR, "Photos Gallery");
		displayTypeOfText(8, NAV_BAR, "Scroll: UP/DOWN");
		displayTypeOfText(9, NAV_BAR, "Back: SEL");
		
		joystickVal = get_button();
		
		switch (photoNum) {
			case 0:
					GLCD_Bitmap(80, 25, 160, 160, (unsigned char *)GIMP_TMUNOT_pixel_data);
					break;
			case 1:
					GLCD_Bitmap(80, 25, 160, 160, (unsigned char *)GIMP_LFC_pixel_data);
					break;
			case 2:
					GLCD_Bitmap(80, 25, 160, 160, (unsigned char *)GIMP_ONEPIECE_pixel_data);
					break;
			case 3:
					GLCD_Bitmap(80, 25, 167, 160, (unsigned char *)GIMP_STARDEW_pixel_data);
					break;
			case 4:
					GLCD_Bitmap(80, 25, 160, 160, (unsigned char *)GIMP_TMU_pixel_data);
					break;
		}
		
		while(1){
			joystickVal = get_button();
			if(joystickVal != 0){
				break;
			}
		}
		
		if(joystickVal == KBD_DOWN){
			// GLCD_Clear(White); // removed. looks bad
			for(j = 1; j<8; j++){
				printLCD(j, White, White, "", JUSTIFY_MIDDLE);
			}
			
			photoNum = (photoNum + 1) % NUM_PHOTOS;
		}
		else if(joystickVal == KBD_UP){
			//GLCD_Clear(White); //removed. looks bad
			for(j = 1; j<8; j++){
				printLCD(j, White, White, "", JUSTIFY_MIDDLE);
			}			
			photoNum = (photoNum - 1 + NUM_PHOTOS) % NUM_PHOTOS;
		}
		else if(joystickVal == KBD_SELECT){
			GLCD_Clear(Black);
			return 0;
		}
		
		for(delay = 0; delay < 1000; delay++);
	}
}
