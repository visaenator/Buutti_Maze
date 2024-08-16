#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Possible maze size can be infinite and not even a square, but lets set the limit somewhere in this exercise
//Because max movement is 200, starting from the middle and going in straight line would get you a 401x401 maze
#define MAX_SIZE 402

//void read_maze(char maze[MAX_SIZE][MAX_SIZE], FILE* ptr);
void read_maze(char **maze, int x, int y, FILE* ptr);
void check_size(int *max_x, int *max_y, FILE* ptr);
void fast_check();
void maze_alg();

int main(){

    char file_name[20];
    printf("Give the name of the maze file: ");
    scanf("%s", file_name);
    printf("%s\n", file_name);

    FILE* file_ptr = fopen(file_name, "r");
    if(file_ptr == NULL){
        printf("That file does not exist :(");
        return 1;
    }
    
    int x, y = 0;
    check_size(&x, &y, file_ptr);

    char** maze = (char**)malloc(y * sizeof(char*));
    for(int i = 0; i < y; ++i){
        maze[i] = (char*)malloc(x * sizeof(char));
    }

    read_maze(maze, x, y, file_ptr);


    bool end = false;
    int start_x, start_y = 0;
    for(int i = 0; i < y; ++i){
        for(int j = 0; j < x; ++j){
            char c = maze[i][j];
            printf("%c", c);
            if(c == '^'){
                start_x = j;
                start_y = i;
            }
        }
        printf("\n");
    }

    printf("%d, %d", start_x, start_y);

    for(int i = 0; i < y; ++i){
        free(maze[i]);
    }
    free(maze);
    fclose(file_ptr);
    return 0;
}
/*
void read_maze(char maze[MAX_SIZE][MAX_SIZE], FILE* ptr){
    char ch;
    int x, y = 0;
    int read_amount = 0;
    int max_x, max_y = 0;

    do{
        ch = fgetc(ptr);
        if(ch == '\n'){
            maze[y][x] = 'X';
            ++y;
            x = 0;
        }
        else{
            maze[y][x] = ch;
            ++x;
        }
        ++read_amount;
    }
    while(ch != EOF);
    maze[y][x] = 'Z';
}

*/

void read_maze(char **maze, int x, int y, FILE* ptr){
    char ch;
    for(int i = 0; i < y; ++i){
        int j;
        for(j = 0; j < x; ++j){
            ch = fgetc(ptr);
            //Maze is not a rectangle
            if(ch == '\n'){
                break;
            }
            maze[i][j] = ch;
        }
        //Check if the newline needs to read out of the way
        if(j == x){
            fgetc(ptr);
        }
        
    }
}

void check_size(int *max_x, int *max_y, FILE* ptr){
    int x, y, big_x = 0;
    char ch;
    do{
        ch = fgetc(ptr);
        if(ch == '\n'){
            ++y;
            //The maze might not be a rectangle
            if(big_x < x){
                big_x = x;
            }
            x = 0;
        }
        else{
            ++x;
        }
    }
    while(ch != EOF);
    *max_x = big_x;
    *max_y = y;
    rewind(ptr);
}
