/*
* 
* Author: Safwan Hossain
* COE718-F2024 Embedded Systems
* Project: Multimedia Center Embedded in LPC1768 Microprocessor on MCB1700 Dev Board
* Module: Dragon Warrior Scroll
* 
*/

#include "display.h"
#include <LPC17xx.H>                    
#include "GLCD.h"
#include "KBD.h"
#include "display.h"

int game3(void){
	int joystickVal;	
	
	while(1){			
		displayTypeOfText(0, NAV_BAR, "Dragon Warrior Scroll");
		displayTypeOfText(9, NAV_BAR, "SELECT to Exit");
		
		joystickVal = get_button();			
		
		if(joystickVal == KBD_SELECT){
			GLCD_Clear(Black);
			return 0;
		}
	}
}
