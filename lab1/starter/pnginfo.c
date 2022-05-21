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
    ///////Variable Declaration//////
    int k;
    int width;
    int height;
    U32 readed_file[100];
    char chunk_type;
    // int tracker = 0;
    FILE *files;
    int *png_bytes = -1991225785;
    struct data_IHDR ihdr_in;
    ///////////////////////////////

    ///////file operations//////////////
    files = fopen(argv[1], "rb");
    if(files == NULL){
        printf("failed to open file");
        return 3;
    }
    fread(readed_file, sizeof(readed_file), 100, files);

    for(k=0; k<24; k++){
        printf("%0x\n", ntohl(readed_file[k]));
        // readed_file[k] = ntohl(readed_file[k]);
    }
    ////////////////////////////////////

    ////////////Check if png///////////////
    int i = ntohl(readed_file[0]);
    printf("%d\n", i);
    if(i == png_bytes){
        printf("%s: It is a PNG file\n", argv[1]);
    }else{
        printf("%s: Not a PNG file", argv[1]);
        return 0;
    }
    ///////////////////////////////

    /////////Print width and height///////////
    (ihdr_in).width = ntohl(readed_file[4]);
    (ihdr_in).height = ntohl(readed_file[5]);
    printf("%s: %d x %d\n", argv[1], get_png_height(ihdr_in), get_png_width(ihdr_in));
    // printf("%0x\n", ntohl(readed_file[22]));
    // printf("%0x\n", (ntohl(readed_file[22])>>24));
    ///////////////////////////////////////

    get_chunk(readed_file, chunk_type = "iend");

    return 0;
}