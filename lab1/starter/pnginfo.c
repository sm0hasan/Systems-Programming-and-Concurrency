#include <stdio.h>	/* printf needs to include this header file */
#include <ctype.h>
#include <stdlib.h>
#include "png_util/lab_png.h"

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
    int k;
    U32 readed_file[100];
    int tracker = 0;
    FILE *files;
    int *png_bytes = -1991225785;

    // readed_file = (char*)malloc(1*8);
    files = fopen(argv[1], "rb");

    if(files == NULL){
        printf("failed to open file");
        return 3;
    }
    fread(readed_file, sizeof(readed_file), 8, files);

    for(k=0; k<8; k++){
        printf("%0x\n", ntohl(readed_file[k]));
    }

    int i = ntohl(readed_file[0]);
    printf("%d", i);
    if(i == png_bytes){
        printf("ye");
    }else{
        printf("Not a PNG file");
        return 0;
    }





    // printf("A complete list of command line arguments:\n");
    // for (i = 0; i < argc; i++) {
    //     printf("argv[%d]=%s\n", i, argv[i]);
    // }
    return 0;
}