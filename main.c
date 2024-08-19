#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//Possible maze size can be infinite and not even a square, but lets set the limit somewhere in this exercise
//Because max movement is 200, starting from the middle and going in straight line would get you a 401x401 maze
#define MAX_SIZE 402

const int step_difficulties[3] = {20, 150, 200};

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
int go_through_maze(char **maze, coord location, const int max_x, const int max_y, int *stepsm, coord* exits_coords, int num_of_exits);

void free_maze_memory(char **maze, int max_y, coord* exits_coords);
bool can_reach_exit(const coord location, const coord* exits, const int num_of_exits, const int steps);
void print_maze(char **maze, const int max_x, const int max_y, coord start);

int main(){

    char file_name[20];
    printf("Give the name of the maze file: ");

    if(fgets(file_name, 20, stdin)){
        file_name[strcspn(file_name, "\n")] = 0;
    }

    FILE* file_ptr = fopen(file_name, "r");
    if(file_ptr == NULL){
        printf("File %s does not exist :(", file_name);
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

    int exits = 0;
    coord start = {-1, -1};
    for(int i = 0; i < max_y; ++i){
        for(int j = 0; j < max_x; ++j){
            char c = maze[i][j];
            if(c == '^'){
                start.x = j;
                start.y = i;
            }
            else if(c == 'E'){
                ++exits;
            }
        }
    }

    coord* exits_coords = (coord*)malloc(exits * sizeof(coord));

    if(start.x == -1 || start.y == -1){
        printf("Couldn't find starting position, aborting...");
        free_maze_memory(maze, max_y, exits_coords);
        return 0;
    }
    if(exits <= 0){
        printf("Couldn't find exit, aborting...");
        free_maze_memory(maze, max_y, exits_coords);
        return 0;
    }
    
    int temp = 0;
    for(int i = 0; i < max_y; ++i){
        for(int j = 0; j < max_x; ++j){
            char c = maze[i][j];
            if(c == 'E'){
                coord test_coord = {j, i};
                exits_coords[temp++] = test_coord;
            }
        }
    }

    int result = -2;
    int steps = 0;
    int steps_taken = 0;

    for(int i = 0; i < 3; ++i){
        if(!can_reach_exit(start, exits_coords, exits, step_difficulties[i])){
            printf("No exit within reach with %d steps...\n", step_difficulties[i]);
            continue;
        }

        printf("Trying with %d steps.\n", step_difficulties[i]);
        steps = step_difficulties[i];
        result = go_through_maze(maze, start, max_x, max_y, &steps, exits_coords, exits);
        if(result == 1){
            printf("Solved the maze!\n");
            steps_taken = step_difficulties[i]-steps;
            printf("Steps taken: %d\n", steps_taken);
            print_maze(maze, max_x, max_y, start);
            break;
        }

        printf("Couldn't finish with %d steps...\n", step_difficulties[i]);
    }

    if(result == 1){
        char optimization = 'N';
        printf("Do you want to try optimization (Y/N)? WARNING! It might take a looooong time: ");
        while((optimization = getchar()) == '\n' || optimization == EOF) {}

        if(optimization == 'Y' || optimization == 'y'){
            char** best_so_far = (char**)malloc(max_y * sizeof(char*));
            for(int i = 0; i < max_y; ++i){
                best_so_far[i] = (char*)malloc(max_x * sizeof(char));
            }

            while(true){
                //Have to empty the old solution for the optimization
                for(int i = 0; i < max_y; ++i){
                    for(int j = 0; j < max_x; ++j){
                        best_so_far[i][j] = maze[i][j];
                        if(maze[i][j] == 'o'){
                            maze[i][j] = ' ';
                        }
                    }
                }
                --steps_taken;
                int new_try = steps_taken;
                printf("Optimizing...\n");
                result = go_through_maze(maze, start, max_x, max_y, &new_try, exits_coords, exits);
                if(result != 1){
                    printf("Found solution with %d steps\n", steps_taken+1);
                    break;
                }
            }

            print_maze(best_so_far, max_x, max_y, start);
            free(best_so_far);
        }
    }
    
    free_maze_memory(maze, max_y, exits_coords);
    return 0;
}

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

int go_through_maze(char **maze, coord location, const int max_x, const int max_y, int *steps, coord* exits_coords, int num_of_exits){

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
    if(!can_reach_exit(location, exits_coords, num_of_exits, *steps) || *steps <= 0){
        return -2;
    }

    maze[location.y][location.x] = 'o';
    --(*steps);
    
    coord poss_moves[4];
    possible_moves(maze, location, poss_moves, max_x, max_y);

    for(int i = 0; i < 4; ++i){
        if(poss_moves[i].x != -1){
            solution = go_through_maze(maze, poss_moves[i], max_x, max_y, steps, exits_coords, num_of_exits);
            if(solution == 1){
                return solution;
            }
            else if(solution == -2){
                break;
            }
        }
    }

    //We have gotten into a dead end, back up
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

void free_maze_memory(char **maze, int max_y, coord* exits_coords){
    for(int i = 0; i < max_y; ++i){
        free(maze[i]);
    }
    free(maze);
    free(exits_coords);
}

bool can_reach_exit(const coord location, const coord* exits, const int num_of_exits, const int steps){
    for(size_t i = 0; i < num_of_exits; ++i){
        //We have atleast one exit within reach
        if((abs(location.x - exits[i].x) + abs(location.y - exits[i].y)) < steps){
            return true;
        }
    }
    //All exits are too far away
    return false;
}

void print_maze(char **maze, const int max_x, const int max_y, coord start){
    //Restore the starting position
    maze[start.y][start.x] = '^';
    for(int i = 0; i < max_y; ++i){
        for(int j = 0; j < max_x; ++j){
            printf("%c", maze[i][j]);
        }
        printf("\n");
    }
}