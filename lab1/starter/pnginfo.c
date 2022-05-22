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
    U32 readed_file[100];
    U8 readed_file_e[200];
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
    fread(readed_file, sizeof(readed_file), 100, files);
    // fread(readed_file_e, sizeof(readed_file_e), 200, files);

    // for(k=0; k<10; k++){
    //     printf("%x\n", ntohl(readed_file[k]));
    //     // readed_file[k] = ntohl(readed_file[k]);
    // }
    ////////////////////////////////////

    ////////////Check if png///////////////
    int i = ntohl(readed_file[0]);
    // printf("%d\n", i);
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
    // printf("%0x\n", ntohl(readed_file[7]));
    // printf("%x\n", ((ntohl(readed_file[7])<<8)));
    ///////////////////////////////////////

    // chunk = get_chunk(readed_file, chunk_type = "ihdr");
    // chunk = get_chunk(readed_file, chunk_type = "ihdr");
    int n = 0;
    // printf("%d\n", chunk.length);
    U32 temp_chunk[24]={0};
    // for(n;n<chunk.length;++n){
    //     if(n==0){
    //         if((chunk.p_data[n]==73) && (chunk.p_data[n+1]==72) && (chunk.p_data[n+2]==68) && (chunk.p_data[n+3]==82)){
    //             temp_chunk[n] = chunk.p_data[n];
    //         }
    //     }
        
        
    //     printf("%d\n%d\n", chunk.p_data[n], temp_chunk[n]);
    // }
    // crc_calculated = crc(chunk.p_data, chunk.length);
    // printf("crc calculated: %x\n", crc_calculated);
    struct chunk chunk_ihdr = get_chunk(readed_file, chunk_type = "ihdr");
    U32 crc_ihdr = crc(chunk_ihdr.p_data, chunk_ihdr.length);
    struct chunk chunk_idat = get_chunk(readed_file, chunk_type = "idat");
    U32 crc_idat = crc(chunk_idat.p_data, chunk_idat.length);
    struct chunk chunk_iend = get_chunk(readed_file, chunk_type = "iend");
    U32 crc_iend = crc(chunk_iend.p_data, chunk_iend.length);
    // U32 crc_ihdr = crc(chunk_ihdr.p_data, chunk_ihdr.length);
    // U32 crc_idat = crc(chunk_idat.p_data, chunk_idat.length);
    // U32 crc_iend = crc(chunk_iend.p_data, chunk_iend.length);
    // struct chunk chunk_ihdr = get_chunk(readed_file, chunk_type = "ihdr");
    // struct chunk chunk_idat = get_chunk(readed_file, chunk_type = "idat");
    // struct chunk chunk_iend = get_chunk(readed_file, chunk_type = "iend");

    if(crc_ihdr != chunk_ihdr.crc){
        printf("IHDR chunk CRC error: computed %x, expected %x\n", crc_ihdr, chunk_ihdr.crc);
    }
    if(crc_idat != chunk_idat.crc){
        printf("IDAT chunk CRC error: computed %x, expected %x\n", crc_idat, chunk_idat.crc);
    }
    if(crc_iend != chunk_iend.crc){
        printf("IEND chunk CRC error: computed %x, expected %x\n", crc_iend, chunk_iend.crc);
    }


    return 0;
}