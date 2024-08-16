#include <stdio.h>


int main(){

    char file_name[20];
    printf("Give the name of the maze file: ");
    scanf("%s", file_name);
    printf("%s\n", file_name);

    FILE* maze = fopen(file_name, "r");
    if(maze == NULL){
        printf("That file does not exist :(");
        return 1;
    }
    
    fclose(maze);
    return 0;
}
