#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Possible maze size can be infinite and not even a square, but lets set the limit somewhere in this exercise
//Because max movement is 200, starting from the middle and going in straight line would get you a 401x401 maze
#define MAX_SIZE 402

typedef
    struct{
        int x, y;
    } coord;

coord set_coordinate(int x, int y){
    coord coordinate = {x, y};
    return coordinate;
}

//void read_maze(char maze[MAX_SIZE][MAX_SIZE], FILE* ptr);
void read_maze(char **maze, int x, int y, FILE* ptr);
bool check_size(int *max_x, int *max_y, FILE* ptr);
void possible_moves(char **maze, coord start, coord* poss_moves, const int max_x, const int max_y);
int go_through_maze(char **maze, coord location, const int max_x, const int max_y, int *steps);
void print_maze(char **maze);

int main(){

    char file_name[20];
    printf("Give the name of the maze file: ");
    scanf("%s", file_name);

    FILE* file_ptr = fopen(file_name, "r");
    if(file_ptr == NULL){
        printf("That file does not exist :(");
        return 1;
    }
    
    int max_x, max_y = 0;
    bool good_size = check_size(&max_x, &max_y, file_ptr);

    if(!good_size){
        fclose(file_ptr);
        return 1;
    }

    char** maze = (char**)malloc(max_y * sizeof(char*));
    for(int i = 0; i < max_y; ++i){
        maze[i] = (char*)malloc(max_x * sizeof(char));
    }

    read_maze(maze, max_x, max_y, file_ptr);
    fclose(file_ptr);

    bool exit_exists = false;
    coord start = {-1, -1};
    for(int i = 0; i < max_y; ++i){
        for(int j = 0; j < max_x; ++j){
            char c = maze[i][j];
            printf("%c", c);
            if(c == '^'){
                start.x = j;
                start.y = i;
            }
            else if(c == 'E'){
                exit_exists = true;
            }
        }
        printf("\n");
    }

    if(start.x == -1 || start.y == -1){
        printf("Couldn't find starting position, aborting...");
    }
    if(!exit_exists){
        printf("Couldn't find exit, aborting...");
    }

    //printf("%d, %d", start.x, start.y);

    int step_difficulties[3] = {50, 100, 200};
    int result = -2;
    int steps_taken = 0;
    int difficulty;

    for(difficulty = 0; difficulty < 3; ++difficulty){
        printf("Trying with %d steps.\n", step_difficulties[difficulty]);
        steps_taken = step_difficulties[difficulty];
        result = go_through_maze(maze, start, max_x, max_y, &steps_taken);
        if(result == 1){
            break;
        }
        printf("Couldn't finish with %d steps...\n", step_difficulties[difficulty]);
    }

    //Restore the starting position;
    maze[start.y][start.x] = '^';

    if(result == 1){
        printf("Solved the maze!\n");
        printf("Steps taken: %d\n", step_difficulties[difficulty]-steps_taken);
    }
    else {
        printf("Didn't solve the maze... \n");
    }

    for(int i = 0; i < max_y; ++i){
        for(int j = 0; j < max_x; ++j){
            char c = maze[i][j];
            printf("%c", c);
        }
        printf("\n");
    }

    for(int i = 0; i < max_y; ++i){
        free(maze[i]);
    }
    free(maze);
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
        //Check if the newline needs to be read out of the way
        if(j == x){
            fgetc(ptr);
        }
        
    }
}

bool check_size(int *max_x, int *max_y, FILE* ptr){
    int x, y, big_x = 0;
    char ch;
    int amount_of_chars = 0;
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
        ++amount_of_chars;
        if(amount_of_chars > (MAX_SIZE*2)){
            //This value can be changed if we come up with realistic solvable mazes that could be larger
            printf("Maze starts to feel a bit too big, aborting...");
            return false;
        }
    }
    while(ch != EOF);
    *max_x = big_x;
    *max_y = y;
    rewind(ptr);
    return true;
}

int go_through_maze(char **maze, coord location, const int max_x, const int max_y, int *steps){

    //0 = Continue
    //1 = Found the exit
    //-1 = Dead end
    //-2 = Failed
    int solution = 0;

    //Found the exit
    if(maze[location.y][location.x] == 'E'){
        solution = 1;
        return solution;
    }

    //We have ran out of steps
    if(*steps <= 0){
        return -2;
    }

    maze[location.y][location.x] = 'X';
    --(*steps);
    
    coord poss_moves[4];
    possible_moves(maze, location, poss_moves, max_x, max_y);

    for(int i = 0; i < 4; ++i){
        //printf("X:%d Y:%d Steps:%d\n", poss_moves[i].x, poss_moves[i].y, *steps);

        if(poss_moves[i].x != -1){
            solution = go_through_maze(maze, poss_moves[i], max_x, max_y, steps);
            if(solution == 1){
                return solution;
            }
            else if(solution == -2){
                break;
            }
        }
    }

    maze[location.y][location.x] = ' ';
    ++(*steps);
    return -1;
}

void possible_moves(char **maze, coord start, coord* poss_moves, const int max_x, const int max_y){
    coord pos = start;
    
    //First set all the moves impossible
    for(int i = 0; i < 4; ++i){
        poss_moves[i] = set_coordinate(-1, -1);
    }

    //UP
    if(pos.y > 0 && (maze[pos.y-1][pos.x] == ' ' || maze[pos.y-1][pos.x] == 'E')){
        poss_moves[0] = set_coordinate(pos.x,pos.y-1);
    }

    //DOWN
    if(pos.y < max_y -1 && (maze[pos.y+1][pos.x] == ' ' || maze[pos.y+1][pos.x] == 'E')){
        poss_moves[1] = set_coordinate(pos.x,pos.y+1);
    }

    //LEFT
    if(pos.x > 0 && (maze[pos.y][pos.x-1] == ' ' || maze[pos.y][pos.x-1] == 'E')){
        poss_moves[2] = set_coordinate(pos.x-1,pos.y);
    }

    //RIGHT
    if(pos.x < max_x -1 && (maze[pos.y][pos.x+1] == ' ' || maze[pos.y][pos.x+1] == 'E')){
        poss_moves[3] = set_coordinate(pos.x+1,pos.y);
    }


}
