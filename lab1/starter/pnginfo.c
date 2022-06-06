#include <stdio.h>	/* printf needs to include this header file */
#include <ctype.h>
#include <stdlib.h>
#include "png_util/crc.h"
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
    U32 readed_file[100000];
    char chunk_type;
    // int tracker = 0;
    FILE *files;
    int *png_bytes = -1991225785;
    struct data_IHDR ihdr_in;
    struct chunk chunk;
    U32 crc_calculated;
    ///////////////////////////////

    ///////file operations//////////////
    files = fopen(argv[1], "rb");
    if(files == NULL){
        printf("failed to open file");
        return 3;
    }
    fread(readed_file, sizeof(U32), 100000, files);

    ////////////Check if png///////////////
    int i = ntohl(readed_file[0]);
    // printf("%d\n", i);
    if(i != png_bytes){
    //     printf("%s: It is a PNG file\n", argv[1]);
    // }else{
        printf("%s: Not a PNG file", argv[1]);
        return 0;
    }
    ///////////////////////////////

    /////////Print width and height///////////
    (ihdr_in).width = ntohl(readed_file[4]);
    (ihdr_in).height = ntohl(readed_file[5]);
    printf("%s: %d x %d\n", argv[1], get_png_width(ihdr_in), get_png_height(ihdr_in));

    ///////CRC//////////
    simple_PNG_p png = get_image(readed_file);
    U8 *ihdr_data_crc_ptr;
    U8 ihdr_data_crc[120];
    U8 *idat_data_crc_ptr;
    U8 idat_data_crc[40000];
    U8 *iend_data_crc_ptr;
    U8 iend_data_crc[120];
    /* IHDR */
    for(i=0;i<4;++i){
        ihdr_data_crc[i] = png.p_IHDR->type[i];
    }
    k=0;
    for(i=4;i<(4+(png.p_IHDR->length));++i){
        ihdr_data_crc[i] = png.p_IHDR->p_data[k];
        ++k;
    }
    ihdr_data_crc_ptr = ihdr_data_crc; 

    /* IDAT */
    for(i=0;i<4;++i){
        idat_data_crc[i] = png.p_IDAT->type[i];
    }
    k=0;
    for(i=4;i<(4+(png.p_IDAT->length));++i){
        idat_data_crc[i] = png.p_IDAT->p_data[k];
        ++k;
    }
    idat_data_crc_ptr = idat_data_crc;

    // /* IEND */
    for(i=0;i<4;++i){
        iend_data_crc[i] = png.p_IEND->type[i];
    }
    iend_data_crc_ptr = iend_data_crc;

    U32 crc_ihdr = crc(ihdr_data_crc_ptr, (png.p_IHDR->length)+4);
    U32 crc_idat = crc(idat_data_crc_ptr, (png.p_IDAT->length)+4);
    U32 crc_iend = crc(iend_data_crc_ptr, (png.p_IEND->length)+4);
    // //////////////////////////////////////////////////////////



    if(crc_ihdr != png.p_IHDR->crc){
        printf("IHDR chunk CRC error: computed %x, expected %x\n", crc_ihdr, png.p_IHDR->crc);
    }
    if(crc_idat != png.p_IDAT->crc){
        printf("IDAT chunk CRC error: computed %x, expected %x\n", crc_idat, png.p_IDAT->crc);
    }
    if(crc_iend != png.p_IEND->crc){
        printf("IEND chunk CRC error: computed %x, expected %x\n", crc_iend, png.p_IEND->crc);
    }
    // get_image(readed_file);


    return 0;
}