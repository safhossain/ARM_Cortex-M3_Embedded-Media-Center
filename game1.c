/*
*
* Author: Safwan Hossain
* COE718-F2024 Embedded Systems
* Project: Multimedia Center Embedded in LPC1768 Microprocessor on MCB1700 Dev Board
* Module: Maze
*
*/

#include <LPC17xx.H>
#include "GLCD.h"
#include "KBD.h"
#include <string.h>
#include <stdio.h>
#include "display.h"
#include "scary.c"

#define __FI 1

//those kbd defines are too fucking long to type every time so making some aliases:
#define UP    KBD_UP
#define DOWN  KBD_DOWN
#define LEFT  KBD_LEFT
#define RIGHT KBD_RIGHT
#define SELECT KBD_SELECT

#define MAZE_ROWS 9
#define MAZE_COLS 20

char maze[MAZE_ROWS][MAZE_COLS + 1];
int player_row = 1;
int player_col = 1;
int level;

const char maze_level1[MAZE_ROWS][MAZE_COLS + 1] = {
	"####################",
	"#P   #         #   #",
	"# ### # ##### ###  E",
	"#     #     #     ##",
	"##### ##### # ######",
	"#     #   #        #",
	"# ### # # ##### ## #",
	"#   #   #        # #",
	"####################"
};

const char maze_level2[MAZE_ROWS][MAZE_COLS + 1] = {
	"####################",
	"#    #        ##  E#",
	"### ####### # ## ###",
	"#   #     # # #    #",
	"# #   ### # # #### #",
	"# ### #   # #    # #",
	"#     # ### # ## # #",
	"#P#####     #      #",
	"####################"
};

const char maze_level3[MAZE_ROWS][MAZE_COLS + 1] = {
	"####################",
	"#              #####",
	"### ###### # #    E#",
	"#   #    # # #######",
	"# #   ## # # #    P#",
	"# ### #   # # ###  #",
	"#     # ###   #   ##",
	"# #####     #   ####",
	"####################"
};

/* Function to find the player's starting position */
void find_player_position(void) {
	int row, col;
	for (row = 0; row < MAZE_ROWS; row++) {
		for (col = 0; col < MAZE_COLS; col++) {
			if (maze[row][col] == 'P') {
				player_row = row;
				player_col = col;
				return;
			}
		}
	}
}

void draw_maze(void) {
	int row, col;
	for (row = 1; row <= MAZE_ROWS; row++) {
		for (col = 0; col < MAZE_COLS; col++) {
			char cell = maze[row - 1][col];
			unsigned short color = Black;
			unsigned short text_color = White;
			
			// blue walls, green player, red exit, black path
			if (cell == '#') {
				color = Blue;
				text_color = Blue;
			} else if (cell == 'P') {
				color = Green;
				text_color = Green;
			} else if (cell == 'E') {
				color = Red;
				text_color = Red;
			} else {
				color = Black;
				text_color = White;
			}

			GLCD_SetBackColor(color);
			GLCD_SetTextColor(text_color);
			GLCD_DisplayChar(row, col, __FI, cell);
		}
	}
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(White);
}

void load_level(void) {
	char title[21];
	switch (level) {
		case 1:
			memcpy(maze, maze_level1, sizeof(maze));
			break;
		case 2:
			memcpy(maze, maze_level2, sizeof(maze));
			break;
		case 3:
			memcpy(maze, maze_level3, sizeof(maze));
			break;
		default:
			return;
	}
	find_player_position();
	draw_maze();	
	sprintf(title, "LVL %d: SEL to Exit", level);
	displayTypeOfText(0, NAV_BAR, title);
}

void display_win_message(void) {
	int joystickVal;
	GLCD_Clear(Black);
	/*
	displayTypeOfText(4, DEFAULT_MIDDLE, "You Escaped!");
	displayTypeOfText(6, DEFAULT_MIDDLE, "Press SELECT");
	displayTypeOfText(7, DEFAULT_MIDDLE, "to Return");
	*/
	
	GLCD_Bitmap(10, 25, 260, 180, (unsigned char *)GIMP_SCARY_pixel_data);
	displayTypeOfText(8, DEFAULT_MIDDLE, "Press SELECT");
	displayTypeOfText(9, DEFAULT_MIDDLE, "to Return");
	
	while (1) {
		joystickVal = get_button();
		if (joystickVal == KBD_SELECT) {
			GLCD_Clear(Black);
			displayTypeOfText(8, DEFAULT_MIDDLE, "Press SELECT");
			displayTypeOfText(9, DEFAULT_MIDDLE, "to Return");
			return;
		}
	}
}

void move_player(int delta_row, int delta_col) {
	int new_row = player_row + delta_row;
	int new_col = player_col + delta_col;
	char next_cell;

	// Check outer boundaries
	if (new_row < 0 || new_row >= MAZE_ROWS || new_col < 0 || new_col >= MAZE_COLS) {
		return;
	}

	next_cell = maze[new_row][new_col];

	// Check for walls
	if (next_cell == '#') {
		return; // Wall detected
	}

	// Check for exit
	if (next_cell == 'E') {
		// Update maze with new player position
		maze[player_row][player_col] = ' '; // Clear old position
		player_row = new_row;
		player_col = new_col;
		maze[player_row][player_col] = 'P'; // Set new position internally
		draw_maze();

		if (level < 3) {
			level++;
			load_level(); // Load the next level
			return;
		} else {
			display_win_message(); // Display win message after level 3
			return;
		}
	}

	// regular path movement (no exit, no out of bounds, no wall hit)
	maze[player_row][player_col] = ' '; // Clear old position
	player_row = new_row;
	player_col = new_col;
	maze[player_row][player_col] = 'P'; // Set new position
	
	draw_maze();
}

volatile int delayN;
int game1(void) {
	int joystickVal;
	int prev_joystickVal = 0;

	level = 1;
	load_level();
	
	while (1) {
		joystickVal = get_button();

		//if (joystickVal != prev_joystickVal) { // joystick control too slow with this. better to give continious drift rather than discrete user inputs
			if (joystickVal != 0) {
				if (joystickVal & UP) {
					move_player(-1, 0);
				}
				if (joystickVal & DOWN) {
					move_player(1, 0);
				}
				if (joystickVal & LEFT) {
					move_player(0, -1);
				}
				if (joystickVal & RIGHT) {
					move_player(0, 1);
				}
				if (joystickVal & SELECT) {					
					GLCD_Clear(Black);					
					return 0;
				}				
				for (delayN = 0; delayN < 100000; delayN++); // delay
			}
			//prev_joystickVal = joystickVal;
		//}
	}
}
