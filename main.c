//
//  main.c
//  The Game of Life
//
//  Created by Gabriel Kapach on 17/09/2017.
//  Copyright © 2017 Gabriel Kapach. All rights reserved.
//035724221

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BOARD_SIZE 14

#define WRONG_ARGUMENTS             (-1);
#define CANNOT_OPEN_SCREEN_FILE     (-2);
#define CANNOT_OPEN_BOARD_FILE      (-3);
#define CANNOT_CLOSE_SCREEN_FILE    (-4);
#define CANNOT_CLOSE_BOARD_FILE     (-5);

void load(FILE * fp);
void generate(void);
int print(const char * screen_path);
int count(int x, int y);
void apply(void);

//Create the board and add empty frame to simplfy calculations
int board[BOARD_SIZE+2][BOARD_SIZE+2] = {{0}};

int main(int argc, const char * argv[]) {
    srand(time(0));
    int status = 0;
    FILE * fp_board = NULL;
    
    //Check if file for exportation is available
    if (argc < 2) {
        printf("Please enter a screen file");
        return WRONG_ARGUMENTS;
    }
    
    //Check if a board was given
    if (argc > 2) {
        fp_board = fopen(argv[2], "r");
        if (fp_board == NULL) {
            printf("Couldn't open file at path \"%s\"\nGenerating random board...\n", argv[2]);
            status = CANNOT_OPEN_BOARD_FILE;
            generate();
        } else {
            load(fp_board);
        }
    } else {
        generate();
    }
    
    //Make one move
    apply();
    
    //Print & Check if the printing was sucessfull
    status = print(argv[1]);
    
    return status;
}

/*
 * Function: generate
 * ------------------
 * Generates a random board
 *
 */

void generate(void) {
    int x, y;
    for (y = 1; y <= BOARD_SIZE; y++) {
        for (x = 1; x <= BOARD_SIZE; x++) {
            board[y][x] = rand() % 2;
        }
    }
}

/*
 * Function: load
 * --------------
 * Load a board from a given file
 *
 */

void load(FILE * fp) {
    int x = 0, y = 0, c, flag = 0;
    while (EOF != (c = fgetc(fp))) {
        if (x > BOARD_SIZE || y > BOARD_SIZE) {
            printf("Board max width or height is %d\n", BOARD_SIZE);
            exit(0);
        }
        
        //Ignore similar chars in the unicode characters used the view the map
        if (c == 226 || c == 150) {
            continue;
        }
        
        //Move to the next row in the matrix
        if (c == '\n') {
            y += 1;
            x = 0;
            continue;
        }
        
        //Ignore one character because any char being used twice to create visualy a square
        if (flag == 1) {
            board[1 + y][++x] = c == 136 ? 1 : 0;
            flag = 0;
        } else {
            flag = 1;
        }
    }
}

/*
 * Function: print
 * ---------------
 * Prints the current state of the board into a given file
 *
 */

int print(const char * screen_path) {
    FILE * fp_screen = fopen(screen_path, "w");
    if (fp_screen == NULL) {
        printf("Couldn't create / open file at path \"%s\"\n", screen_path);
        return CANNOT_OPEN_SCREEN_FILE;
    }
    
    int x, y;
    for (y = 1; y <= BOARD_SIZE; y++) {
        for (x = 1; x <= BOARD_SIZE; x++) {
            char * key = board[y][x] == 1 ? "\u2588\u2588" : "\u2591\u2591";
            fputs(key, fp_screen);
        }
        fputc('\n', fp_screen);
    }
    
    if (0 != fclose(fp_screen)) {
        printf("Couldn't close file \"%s\"\n", screen_path);
        return CANNOT_CLOSE_SCREEN_FILE;
    }
    
    return 0;
}

/*
 * Function: count
 * ---------------
 * Count how many living neighbors there are around a cell
 *
 */

int count(int x, int y) {
    int count = 0;
    
    int n, m;
    for (n = -1; n <= 1; n++) {
        for (m = -1; m <= 1; m++) {
            //Ignore the cell himself
            if (n == 0 && m == 0) {
                continue;
            }

            count += board[y + n][x + m];
        }
    }

    return count;
}

/*
 * Function: apply
 * ---------------
 * Make one move in the game by his rules
 *
 */

void apply(void) {
    int neighbors[BOARD_SIZE][BOARD_SIZE] = {{0}};
    
    //Map the board for each cell by his neighbors count
    int x, y;
    for (y = 1; y <= BOARD_SIZE; y++) {
        for (x = 1; x <= BOARD_SIZE; x++) {
            neighbors[y-1][x-1] = count(x, y);
        }
    }
    
    for (y = 1; y <= BOARD_SIZE; y++) {
        for (x = 1; x <= BOARD_SIZE; x++) {
            if (board[y][x] == 0) {
                if (neighbors[y-1][x-1] == 3) {
                    board[y][x] = 1;
                }
            } else {
                if (neighbors[y-1][x-1] < 2 || neighbors[y-1][x-1] > 3) {
                    board[y][x] = 0;
                }
            }
        }
    }
}
