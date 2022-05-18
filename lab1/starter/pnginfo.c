#include <stdio.h>	/* printf needs to include this header file */
#include <ctype.h>
#include <stdlib.h>

/**
 *@brief: main function that lists all command line arguments
 *@param: int argc is the number of command line arguments. 
 *        The executable itself is the first argument.
 *@param: char *argv is an array to hold each command line argument as an element.
 *        argv[0] contains the string representation of executable's name
 *        argv[1] contains the string representation of the 1st input argument
 *        ...
 *        argv[n] contains the string representation of the n'th input argument
 */
int main(int argc, char *argv[]) 
{
    int i;
    char *readed_file;
    int tracker = 0;
    FILE *files;
    char *png_bytes[3] = {"50", "4E", "47"};

    readed_file = (char*)malloc(1*8);
    files = fopen(argv[1], "r");
    fread(readed_file, 1, 8, files);
    printf(readed_file);

    for(i = 0; i < 2; i++){
        if(readed_file[i+1] == png_bytes[i]){
            tracker = 0;
        }else{
            tracker = 1;
            break;
        }
    }

    if(tracker == 1){
        printf("its not a png file");
    }else{
        printf("yes it is");
    }


    // printf("A complete list of command line arguments:\n");
    // for (i = 0; i < argc; i++) {
    //     printf("argv[%d]=%s\n", i, argv[i]);
    // }
    return 0;
}