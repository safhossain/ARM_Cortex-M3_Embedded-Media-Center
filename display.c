#include "display.h"
#include <string.h>

void displayTypeOfText(int row, int type, char text[21]){	
	switch(type){
		case DEFAULT_LEFT:
			GLCD_SetBackColor(White);	
			GLCD_SetTextColor(Blue);  
			printLCD(row, White, Blue, text, JUSTIFY_LEFT);
			break;		
		case NAV_BAR:
			GLCD_SetBackColor(Black);	
			GLCD_SetTextColor(White);  
			printLCD(row, Black, White, text, JUSTIFY_MIDDLE);
			break;
		case DEFAULT_MIDDLE:
		default:
			GLCD_SetBackColor(Maroon);	
			GLCD_SetTextColor(Green);  
			printLCD(row, Maroon, Green, text, JUSTIFY_MIDDLE);
			break;
	}  
}

void printLCD(int row, unsigned short backgroundColor, unsigned short textColor, const char *text, int justify) {
	char displayText[21];
	int textLength = strlen(text);

	switch(justify) {
		case JUSTIFY_LEFT:
			if(textLength > 20) { // Truncate the message
					strncpy(displayText, text, 20);
					displayText[20] = '\0';
			}
			else {				
					memset(displayText, ' ', 20);
					memcpy(displayText, text, textLength);
					displayText[20] = '\0';
			}
			break;

		case JUSTIFY_MIDDLE:
		default:
			if(textLength > 20) { // If text is too long, fallback to left-justify + truncate		
					strncpy(displayText, text, 20);
					displayText[20] = '\0';
					break;
			}			
			{
				// Calculate padding for middle justification (if spaces are uneven, give fewer spaces on the left)
				int totalSpaces = 20 - textLength;
				int leftSpaces = totalSpaces / 2;
				//int rightSpaces = totalSpaces - leftSpaces;
				
				memset(displayText, ' ', 20);
				memcpy(displayText + leftSpaces, text, textLength);
				displayText[20] = '\0';
			}
			break;
	}
    
	GLCD_SetBackColor(backgroundColor);	
	GLCD_SetTextColor(textColor);

	GLCD_DisplayString(row, 0, __FI, (unsigned char *)displayText);
}
