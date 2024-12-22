/*
* 
* Author: Safwan Hossain
* COE718-F2024 Embedded Systems
* Project: Multimedia Center Embedded in LPC1768 Microprocessor on MCB1700 Dev Board
* Module: Tic Tac Toe
* 
*/

#include <LPC17xx.H>                    
#include "GLCD.h"
#include "KBD.h"
#include "display.h"
#include <string.h>
#include <stdio.h>

#define __FI 1

typedef struct {
	int state;
	int display_row;
	int display_col;
	int grid_row; // -1 if not applicable
	int grid_col; // -1 if not applicable
	char cell_type; // 'C' for cell, 'B' for button
} CursorPosition;

#define NUM_STATES 12
#define NUM_INPUTS 4

CursorPosition cursor_positions[NUM_STATES] = {
	{0, 2, 1, 0, 0, 'C'},   // State 0
	{1, 2, 5, 0, 1, 'C'},   // State 1
	{2, 2, 9, 0, 2, 'C'},   // State 2
	{3, 2,14, -1,-1,'B'},   // State 3 (RESET)
	{4, 5, 1, 1, 0, 'C'},   // State 4
	{5, 5, 5, 1, 1, 'C'},   // State 5
	{6, 5, 9, 1, 2, 'C'},   // State 6
	{7, 5,14, -1,-1,'B'},   // State 7 (RETURN)
	{8, 8, 1, 2, 0, 'C'},   // State 8
	{9, 8, 5, 2, 1, 'C'},   // State 9
	{10,8, 9, 2, 2, 'C'},    // State 10
	{11,8,14, -1,-1,'B'}    // State 11 (CURRENT)
};

// Input indices: 0 = UP, 1 = DOWN, 2 = LEFT, 3 = RIGHT
int state_transitions[NUM_STATES][NUM_INPUTS];

int get_input_index(uint32_t kbd_input) {
	switch(kbd_input) {
		case KBD_UP: return 0;
		case KBD_DOWN: return 1;
		case KBD_LEFT: return 2;
		case KBD_RIGHT: return 3;
		default: return -1;
	}
}

void initialize_state_transitions(void) {
	int state, input;
	// Initialize to current state
	for (state = 0; state < NUM_STATES; state++) {
		for (input = 0; input < NUM_INPUTS; input++) {
			state_transitions[state][input] = state;
		}
	}
	
	// Input indices: 0 = UP, 1 = DOWN, 2 = LEFT, 3 = RIGHT
	
	// Define specific transitions
	state_transitions[0][1] = 4; // State 0, DOWN to state 4
	state_transitions[0][3] = 1; // State 0, RIGHT to state 1

	state_transitions[1][1] = 5; // State 1, down to State 5
	state_transitions[1][2] = 0; // State 1, left to State 0
	state_transitions[1][3] = 2; // State 1, right to State 2

	state_transitions[2][1] = 6; // State 2, down to State 6
	state_transitions[2][2] = 1; // State 2, left to State 1
	state_transitions[2][3] = 3; // State 2, right to State 3

	state_transitions[3][1] = 7; // State 3, down to State 7
	state_transitions[3][2] = 2; // State 3, left to State 2

	state_transitions[4][0] = 0;
	state_transitions[4][1] = 8;
	state_transitions[4][3] = 5;

	state_transitions[5][0] = 1;
	state_transitions[5][1] = 9;
	state_transitions[5][2] = 4;
	state_transitions[5][3] = 6;

	state_transitions[6][0] = 2;
	state_transitions[6][1] =10;
	state_transitions[6][2] = 5;
	state_transitions[6][3] = 7;

	state_transitions[7][0] = 3;
	state_transitions[7][1] =11;  // State 7, DOWN to state 11
	state_transitions[7][2] = 6;

	state_transitions[8][0] = 4;
	state_transitions[8][3] = 9;

	state_transitions[9][0] = 5;
	state_transitions[9][2] = 8;
	state_transitions[9][3] =10;

	state_transitions[10][0] = 6;
	state_transitions[10][2] = 9;
	state_transitions[10][3] =11; // State 10, RIGHT to state 11
	
	state_transitions[11][0] = 7; // State 11, UP to state 7
	state_transitions[11][2] =10; // State 11, LEFT to state 10
}

void draw_grid(char current_player) {
	int row, col;
	
	int i, j;
	for (i=1; i<=7; i+=3){
		for (j=1; j<=9; j+=4){
			GLCD_DisplayChar(i, j, __FI, ' ');
		}
	}
	
	// Draw vertical lines
	for (row = 1; row <= 8; row++) {
		GLCD_DisplayChar(row, 3, __FI, '|');
		GLCD_DisplayChar(row, 7, __FI, '|');
		GLCD_DisplayChar(row,11, __FI, '|');
		GLCD_DisplayChar(row,18, __FI, '|');
	}
	
	// Draw horizontal lines
	for (col = 0; col <= 18; col++) {
		GLCD_DisplayChar(3, col, __FI, '-');
		GLCD_DisplayChar(6, col, __FI, '-');
	}
	
	// Draw right-side buttons
	GLCD_SetBackColor(Green);
	GLCD_SetTextColor(White);
	GLCD_DisplayString(1, 12, __FI, (unsigned char *)"RESET ");
	GLCD_DisplayString(2, 12, __FI, (unsigned char *)"      ");
	//GLCD_DisplayString(2, 12, __FI, (unsigned char *)"ET ");
	
	GLCD_SetBackColor(Navy);
	GLCD_SetTextColor(White);
	GLCD_DisplayString(4, 12, __FI, (unsigned char *)"RETURN");
	GLCD_DisplayString(5, 12, __FI, (unsigned char *)"      ");
	//GLCD_DisplayString(5, 12, __FI, (unsigned char *)"URN");		
	
	// Reset to default colors		
	GLCD_SetBackColor(White);
	GLCD_SetTextColor(Black);
	GLCD_DisplayString(7, 12, __FI, (unsigned char *)"PLAYER");
	GLCD_DisplayChar(8, 14, __FI, current_player);	
}

void display_cursor(int state) {
	int row = cursor_positions[state].display_row;
	int col = cursor_positions[state].display_col;
	// Display "_" at the cursor position
	GLCD_DisplayString(row, col, __FI, (unsigned char *)"_");
}

void clear_cursor(int state, char game_grid[3][3], char current_player) {
	int row = cursor_positions[state].display_row;
	int col = cursor_positions[state].display_col;		

	if (cursor_positions[state].cell_type == 'C') {
		// For game cells, clear cursor position
		GLCD_DisplayChar(row, col, __FI, ' ');
	} 
	else if (state == 11) { // "CURRENT" cell
		// Display the current player
		GLCD_DisplayChar(row, col, __FI, current_player);
	} 
	else {		
		if (state == 3) { // RESET button
			GLCD_SetBackColor(Green);
			GLCD_SetTextColor(White);
			GLCD_DisplayString(2, 12, __FI, (unsigned char *)"      ");
			GLCD_SetBackColor(White);
			GLCD_SetTextColor(Black);
		}
		else if (state == 7) { // RETURN button
			GLCD_SetBackColor(Navy);
			GLCD_SetTextColor(White);
			GLCD_DisplayString(5, 12, __FI, (unsigned char *)"      ");
			GLCD_SetBackColor(White);
			GLCD_SetTextColor(Black);
		}
	}		
}

int check_winner(char grid[3][3]) {
	int i;
	// Check rows
	for (i = 0; i < 3; i++) {
		if (grid[i][0] != ' ' &&
			grid[i][0] == grid[i][1] &&
			grid[i][1] == grid[i][2]) {
			return 1;
		}
	}
	// Check columns
	for (i = 0; i < 3; i++) {
		if (grid[0][i] != ' ' &&
			grid[0][i] == grid[1][i] &&
			grid[1][i] == grid[2][i]) {
			return 1;
		}
	}
	// Check diagonals (two types)
	if (grid[0][0] != ' ' &&
		grid[0][0] == grid[1][1] &&
		grid[1][1] == grid[2][2]) {
		return 1;
	}
	if (grid[0][2] != ' ' &&
		grid[0][2] == grid[1][1] &&
		grid[1][1] == grid[2][0]) {
		return 1;
	}
	return 0;
}

int check_tie(char grid[3][3]) {
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			if (grid[i][j] == ' ') {
					return 0; // Not a tie
			}
		}
	}
	return 1; // Tie
}

int game2(void) {
	int joystickVal;
	int prev_joystickVal = 0;
	int current_state = 0;
	char current_player = 'X'; //init will always be X
	int game_over = 0;
	int winner = 0;
	char game_grid[3][3];
	int i, j;
	int volatile n;
	int input_index;
	int display_row; 
	int display_col;
	int grid_row, grid_col;
	int next_state;

	// Initialize game grid
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
				game_grid[i][j] = ' ';
		}
	}

	// Initialize state transitions
	initialize_state_transitions();

	// Initialize display
	GLCD_Clear(White);
	displayTypeOfText(0, NAV_BAR, "Tic Tac Toe");
	displayTypeOfText(9, NAV_BAR, "");	
	GLCD_SetBackColor(White);
	GLCD_SetTextColor(Black);

	// Draw grid lines and buttons
	draw_grid(current_player);

	// Display initial cursor
	display_cursor(current_state);

	while (!game_over) {
		joystickVal = get_button();
		
		if (joystickVal != prev_joystickVal) { //prevents joystick SELECT drift
			// Clear the cursor from the current position
			clear_cursor(current_state, game_grid, current_player);
			
			if (joystickVal != 0) {				
				input_index = get_input_index(joystickVal);
			
				if (input_index != -1) {
					next_state = state_transitions[current_state][input_index];
					current_state = next_state;				
				} 
				else if (joystickVal == KBD_SELECT) {						
					grid_row = cursor_positions[current_state].grid_row;
					grid_col = cursor_positions[current_state].grid_col;
				
					// Handle RESET button
					if (current_state == 3) {
						// Clear internal grid
						for (i = 0; i < 3; i++) {
							for (j = 0; j < 3; j++) {
								game_grid[i][j] = ' ';
							}
						}
						// Redraw the grid and reset the game state
						current_player = 'X';
						game_over = 0;
						winner = 0;
						draw_grid(current_player);
						display_cursor(current_state);
						continue; // Skip to the next loop iteration
					}
					
					// Handle RETURN button
					if (current_state == 7) {
						GLCD_Clear(White);
						return 0;
					}
					
					// Handle regular cell (tictactoe section) selection
					if (grid_row != -1 && grid_col != -1) {
						if (game_grid[grid_row][grid_col] == ' ') {
							// Place the marker in the game grid
							game_grid[grid_row][grid_col] = current_player;
							// Update the display (top row of the cell)
							display_row = cursor_positions[current_state].display_row - 1;
							display_col = cursor_positions[current_state].display_col;
							GLCD_DisplayChar(display_row, display_col, __FI, current_player);
							// Check for winner or tie
							if (check_winner(game_grid)) {
									game_over = 1;
									winner = 1;
							} else if (check_tie(game_grid)) {
									game_over = 1;
							}
							// Switch player only after a successful move
							current_player = (current_player == 'X') ? 'O' : 'X';
							// Update the display of current player								
							GLCD_DisplayChar(8, 14, __FI, current_player);								
						}
						// if cell is already occupied do nada
					}
				}
			}
			// Display the cursor at the new position
			display_cursor(current_state);

			// Update previous joystick value for next iter
			prev_joystickVal = joystickVal;
			
			for (n = 0; n < 100000; n++) {
				// Debounce delay
			}
		}
	}
	for (n = 0; n < 100000; n++) {
		// delay
	}
	while (1){
		// Display game over message	
		printLCD(0, Red, Yellow, "GAME OVER", JUSTIFY_MIDDLE);
		printLCD(9, Red, Yellow, "Any DIRECTION 2 Exit", JUSTIFY_MIDDLE);
		//GLCD_SetBackColor(Red);
		//GLCD_SetTextColor(Yellow);
		if (winner) {		
			GLCD_DisplayString(7, 12, __FI, (unsigned char *)"WINNER");
			GLCD_DisplayString(8, 14, __FI, (unsigned char *)"      ");
			GLCD_DisplayChar(8, 14, __FI, (current_player == 'X') ? 'O' : 'X');		
		} 
		else {		
			GLCD_DisplayString(7, 12, __FI, (unsigned char *)" TIED ");
			GLCD_DisplayString(8, 12, __FI, (unsigned char *)"      ");
		}
		GLCD_SetBackColor(White);
		GLCD_SetTextColor(Black);

		// Wait for joystick input to return to main 
		while(1){
			joystickVal = get_button();
			if(joystickVal != 0){
				break;
			}
		}
		if ((joystickVal == KBD_UP) || (joystickVal == KBD_DOWN) || (joystickVal == KBD_LEFT) || (joystickVal == KBD_RIGHT)) {
			GLCD_Clear(White);
			return 0;
		}
	}
}
