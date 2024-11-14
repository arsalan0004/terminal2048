#include <iostream> 
#include <thread>
#include <termios.h>
#include <unistd.h>


// grid is GRID_SIZE tall and wide
#define GRID_SIZE 4

// settings for the terminal (set in set_getChar(), restoreTerminal())
struct termios newt, oldt;


void setupInput(){

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
	
	newt = oldt;
	
    newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
	
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Set new terminal settings
	
}


void restoreTerminal(){
	
	// Restore old terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	
}



int** instantiateGrid(){
	
	/* Create a pointer to a pointer to an int. This pointer to an int is 
	   the first element of an array of pointers to ints */
	int** grid = new int*[GRID_SIZE];
	
	/* instantiate each pointer to an int so that they they point to 
	   an array of ints*/
	for(int i=0; i<GRID_SIZE; i++)
		grid[i] = new int[GRID_SIZE];
	
	/* initiliaze each member of the grid to '0' */
	for(int i=0; i<GRID_SIZE; i++) {
		for(int j=0; j<GRID_SIZE; j++){
			grid[i][j] = 0;
		}	
	}
	
	return grid;
	
	
}

void printGrid(int** grid){
	
	
	// draw top wall 
	for(int i=0; i< GRID_SIZE * 2 + 1; i++){
		std::cout << '-';
	}
	
	// move to next line 
	std::cout << std::endl;
	
	// add the numbers in the grid 
	for(int i=0; i<GRID_SIZE; i++) {
		
		// add left wall 
		std::cout << '|';
		
		// add numbers, then walls 
		for(int j=0; j<GRID_SIZE; j++){
			std::cout << grid[i][j];
			std::cout << '|';
		}
		
		// go to next line 
		std::cout << std::endl;
	}
	
	// add bottom wall
	for(int i=0; i< GRID_SIZE * 2 + 1; i++){
		std::cout << '-';
	}
	std::cout << std::endl;
	std::cout << "press q to quit at anytime. Use WASD keys to play" << std::endl;
	
}


bool slideUp(int** grid){
	/* move all numbers from bottom to top, until they hit the wall 
	   or they hit another number. We do this startig from the numbers at the 
	   top, and then working our way down to the numbers at the bottom
	*/
	
	// keeps track of if the movement worked or not 
	bool movement = false;
	
	// this grid keeps track of which numbers have already been fused. 
	// we can only fuse a number one time per turn 
	int** fusionGrid = instantiateGrid();
	
	// row_size = 1 because we don't move the top most numbers in the grid 
	for(int c=0; c < GRID_SIZE; c++){
		for(int r=1; r< GRID_SIZE; r++){
			
			if(grid[r][c] == 0) continue;
			
			std::cout << r << c << std::endl;
			
			int p = r;
			
			
			// find furthest point that the number can move 
			while(p > 0) {
				if (grid[p-1][c] == 0)
					p--;
				else 
					break;
			}
			
			std:: cout << "p is " << p << std::endl;
			
			// move the value to the new open position 
			if(p != r) {
				movement = true;
				
				// move the number 
				grid[p][c] = grid[r][c];
				
				// erase the old position of the number 
				grid[r][c] = 0;
			}
			
			/*
				check if the number it is next to is the same number.
				If so, fuse the two numbers together 
			*/
			
			
			// if the new position is at the top of the grid then don't worry about it 
			// since it doesnt have anything but the wall to bump (and fuse) with. 
			if(p == 0) continue;
			
			std::cout << "fusion grid is " << fusionGrid[r][p - 1] << std::endl;;
			
			// if the left and number hasn't already been fused
			if(fusionGrid[p-1][c] == 0){
				// if the left hand number is the same as current number 
				if(grid[p][c] == grid[p-1][c]) {
					
					// 'fuse' the numbers 
					grid[p-1][c] = grid[p-1][c] * 2;
					
					// remove old entry 
					grid[p][c]   = 0;
					
					// record that this number has been fused 
					fusionGrid[p-1][c] = 1;
				}
			}
		}
	}
	
	return movement;
}


bool slideDown(int** grid){
	/* move all numbers from top to bottom, until they hit the wall 
	   or they hit another number. We do this startig from the numbers at the 
	   bottom, and then working our way up to the numbers at the bottom
	*/
	
	// keeps track of if the movement worked or not 
	bool movement = false;
	
	// this grid keeps track of which numbers have already been fused. 
	// we can only fuse a number one time per turn 
	int** fusionGrid = instantiateGrid();
	
	// row_size -2  because we don't move the bottom most numbers in the grid 
	for(int c=0; c < GRID_SIZE; c++){
		for(int r=GRID_SIZE-2; r >= 0; r--){
			
			if(grid[r][c] == 0) continue;
			
			
			int p = r;
			
			
			// find furthest point that the number can move 
			while(p < GRID_SIZE-1) {
				if (grid[p+1][c] == 0)
					p++;
				else 
					break;
			}
			
			
			// move the value to the new open position 
			if(p != r) {
				movement = true;
				
				// move the number 
				grid[p][c] = grid[r][c];
				
				// erase the old position of the number 
				grid[r][c] = 0;
			}
			
			/*
				check if the number it is next to is the same number.
				If so, fuse the two numbers together 
			*/
			
			
			// if the new position is at the bottom of the grid then don't worry about it 
			// since it doesnt have anything but the wall to bump (and fuse) with. 
			if(p == GRID_SIZE-1) continue;
			
			
			// if the left and number hasn't already been fused
			if(fusionGrid[p-1][c] == 0){
				// if the left hand number is the same as current number 
				if(grid[p][c] == grid[p+1][c]) {
					
					// 'fuse' the numbers 
					grid[p+1][c] = grid[p+1][c] * 2;
					
					// remove old entry 
					grid[p][c]   = 0;
					
					// record that this number has been fused 
					fusionGrid[p+1][c] = 1;
				}
			}
		}
	}
	
	return movement;
	
	
}


bool slideLeft(int** grid){
	/* move all numbers from right to left, until they hit the wall 
	   or they hit another number. We do this starting from the left side
	   and move left */
	
	// keeps track of if the movement worked or not 
	bool movement = false;
	
	// this grid keeps track of which numbers have already been fused. 
	// we can only fuse a number one time per turn 
	int** fusionGrid = instantiateGrid();
	
	// grid_size = 1 because we don't move the left most numbers in the grid 
	for(int c=1; c < GRID_SIZE; c++){
		for(int r=0; r< GRID_SIZE; r++){
			
			if(grid[r][c] == 0) continue;
		
			
			int p = c;
			
			
			// find furthest point that the number can move 
			while(grid[r][p-1] == 0 && p > 0) {
				p--;
			}
			
			
			// move the value to the new open position 
			if(p != c) {
				movement = true;
				
				// move the number 
				grid[r][p] = grid[r][c];
				
				// erase the old position of the number 
				grid[r][c] = 0;
			}
			
			/*
				check if the number it is next to is the same number.
				If so, fuse the two numbers together 
			*/
			
			
			// if the left and number hasn't already been fused
			if(p == 0) continue;
			
			
			if(fusionGrid[r][p - 1] == 0){
				// if the left hand number is the same as current number 
				if(grid[r][p] == grid[r][p-1]) {
					
					// 'fuse' the numbers 
					grid[r][p-1] = grid[r][p-1] * 2;
					
					// remove old entry 
					grid[r][p]   = 0;
					
					// record that this number has been fused 
					fusionGrid[r][p-1] = 1;
				}
			}
		}
	}
	
	return movement;
}




bool slideRight(int** grid){
	/* move all numbers from left to right, until they hit the wall 
	   or they hit another number. We do this starting from the right side
	   and move left */
	
	// keeps track of if the movement worked or not 
	bool movement = false;
	
	// this grid keeps track of which numbers have already been fused. 
	// we can only fuse a number one time per turn 
	int** fusionGrid = instantiateGrid();
	
	// grid_size -2 because we don't move the right most numbers in the grid 
	for(int c=GRID_SIZE -2; c >=0; c--){
		for(int r=0; r< GRID_SIZE; r++){
			
			if(grid[r][c] == 0) continue;
		
			
			int p = c;
			
			
			// find furthest point that the number can move 
			while(grid[r][p+1] == 0 && p < GRID_SIZE -1) {
				p++;
			}
			
			// move the value to the new open position 
			if(p != c) {
				movement = true;
				
				// move the number 
				grid[r][p] = grid[r][c];
				
				// erase the old position of the number 
				grid[r][c] = 0;
			}
			
			/*
				check if the number it is next to is the same number.
				If so, fuse the two numbers together 
			*/
			
			// if the righth and number hasn't already been fused
			if(fusionGrid[r][p + 1] == 0){
				// if the right hand number is the same as current number 
				if(grid[r][p] == grid[r][p+1]) {
					
					// 'fuse' the numbers 
					grid[r][p+1] = grid[r][p+1] * 2;
					
					// remove old entry 
					grid[r][p]   = 0;
					
					// record that this number has been fused 
					fusionGrid[r][p+1] = 1;
				}
			}
		}
	}
	
	return movement;
}

int main(){
	
	// make user able to input 
	setupInput();
	
	bool gameRunning = true;
	char mostRecentInput;
	
	// make a grid with nothing on it 
	int** grid = instantiateGrid();
	
	// add random numbers to the grid 
	grid[1][1] = 2;
	grid[1][3] = 2;
	grid[1][2] = 2;
	grid [2][1] = 4;
	
	// print the initial grid 
	printGrid(grid);
	
	while(gameRunning){
		
		mostRecentInput = getchar(); // get user input 
		
		switch(mostRecentInput){
			case 'q': 
				gameRunning = false;
				restoreTerminal();
				break;
			case 'w': slideUp(grid); break;
			case 's': slideDown(grid); break;
			case 'a': slideLeft(grid) ; break;
			case 'd': slideRight(grid); break;
			default : std::cout << "error!";
			
		}
		
		// update the display 
		printGrid(grid);
		
	}
		
		
	
	
	
	
	
	
	
	
	
	// setup input 
	
	
	
	return 0;
}
