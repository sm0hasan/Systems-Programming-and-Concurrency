#include <stdio.h>	/* printf needs to include this header file */
#include <ctype.h>
#include <stdlib.h>
#include "png_util/crc.h"
#include "png_util/zutil.h"
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
    int width = 0;
    int height = 0;
    U32 readed_file[100000];
    char chunk_type;
    // int tracker = 0;
    // FILE *files;
    int *png_bytes = -1991225785;
    struct data_IHDR ihdr_in;
    struct chunk chunk;
    U32 crc_calculated;
    U32 total_height = 0;
    
    U8 dest_total[2000000]; 
    U8 def_result[2000000];
    U8 *def_result_ptr = def_result;
    U64 def_len = 0;
    U64 dest_len = 0;
    U64 dest_len_total = 0;
    U64 idat_len = 0;
    U32 index_flag = 0;
    ///////////////////////////////

    ///////file operations//////////////
    for(k=1;k<argc;++k){
        U8 dest[2000000];   
        int i = 0;
        FILE *files;
        files = fopen(argv[k], "rb");
        if(files == NULL){
            printf("failed to open file");
            return 3;
        }
        fread(readed_file, sizeof(U32), 100000, files);
        /////////Print width and height///////////
        (ihdr_in).width = ntohl(readed_file[4]);
        (ihdr_in).height = ntohl(readed_file[5]);
        printf("%s: %d x %d\n", argv[k], get_png_height(ihdr_in), get_png_width(ihdr_in));
        total_height += get_png_height(ihdr_in);
        // printf("%0x\n", ntohl(readed_file[7]));
        // printf("%x\n", ((ntohl(readed_file[7])<<8)));
        ///////////////////////////////////////
        // chunk = extract_actual_chunk(readed_file, chunk_type = "idat");
        simple_PNG_p png = get_image(readed_file);
        // for(i=0; i<chunk.length; ++i){
        //     printf("idat: %x\n", chunk.actual_data[i]);
        // }
        dest_len = 0;
        i = mem_inf(&dest, &dest_len, png.p_IDAT->p_data, png.p_IDAT->length);
        printf("length: %d\n", dest_len);
        zerr(i);
        if(i!=0){
            printf("Cat failed %s\n", argv[k]);
            zerr(i);
            break;
        }
        
        dest_len_total += dest_len;
         
    
        int p = 0;
        for(i=index_flag; i<(index_flag+dest_len); ++i){
            dest_total[i] = dest[p];
            ++p;
        }
        index_flag += dest_len;

    }
    printf("TOTAL HEIGHT: %d\n", total_height);
    ///////////Deflate Operation//////////////////
    k = mem_def(def_result_ptr, &def_len, &dest_total, dest_len_total, Z_DEFAULT_COMPRESSION);
    int i=0;
    for(i=0;i<500;i++){
        printf("def result: %x\n",def_result[i]);
    }    
    if(k==0){
        printf("yessir\n");
    }else{
        printf("mem_def failed\n");
    }
    //////////////////////////////////////////

    ////////////STRUCT PNG ASSIGNMENT//////////
    simple_PNG_p png;
    FILE *files;
    files = fopen(argv[1], "rb");
    if(files == NULL){
        printf("failed to open file");
        return 3;
    }
    fread(readed_file, sizeof(U32), 400000, files);
    png = get_image(readed_file);

    
    /*INITIAL ARRAY*/
    k=0;
    U8 initial_arr[12];
    for(i=0;i<4;++i){
        initial_arr[k] = (readed_file[i] >> 24);
        initial_arr[k+1] = ((readed_file[i] << 8)>>24);
        initial_arr[k+2] = ((readed_file[i] <<16)>>24);
        initial_arr[k+3] = ((readed_file[i] << 24)>>24);
        k+=4;
    }
    for(i=0;i<12;++i){
        printf("Initial arr[%d]: %x\n", i, initial_arr[i]);
    }

    fclose(files);
    /*       */
    /* Height */
    U8 height_arr[4];
    height_arr[0] = (total_height >> 24);
    height_arr[1] = ((total_height << 8)>>24);
    height_arr[2] = ((total_height <<16)>>24);
    height_arr[3] = ((total_height << 24)>>24);
    k=0;
    for(i=4;i<8;++i){
        png.p_IHDR->p_data[i] = height_arr[k];
        printf("%x\n", height_arr[k]);
        ++k;
    }
    // for(i=0;i<(png.p_IHDR->length);++i){
    //     printf("The current IHDR: %x\n", png.p_IHDR->p_data[i]);
    // }
    ///////IHDR CRC///////////
    U8 *ihdr_data_crc_ptr;
    U8 ihdr_data_crc[120];

    for(i=0;i<4;++i){
        ihdr_data_crc[i] = png.p_IHDR->type[i];
    }
    k=0;
    for(i=4;i<(4+(png.p_IHDR->length));++i){
        ihdr_data_crc[i] = png.p_IHDR->p_data[k];
        ++k;
    }
    
    ihdr_data_crc_ptr = ihdr_data_crc; 
    U32 crc_ihdr = crc(ihdr_data_crc_ptr, (png.p_IHDR->length)+4);
    png.p_IHDR->crc = crc_ihdr;
    printf("new ihdr crc: %x\n", png.p_IHDR->crc);///IHDR CRC: WORKS/////
    U32 ihdr_total[120];
    // ihdr_total[0] = htonl(png.p_IHDR->length);
    U32 *ihdr_total_ptr;

    
    /*        */
    png.p_IDAT->p_data = def_result;
    png.p_IDAT->length = def_len;
    U8 *idat_data_crc_ptr;
    U8 idat_data_crc[40000];
    

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
    png.p_IDAT->crc = crc(idat_data_crc_ptr, (png.p_IDAT->length)+4);
    printf("new idata crc:%x\n", png.p_IDAT->crc);
    
    FILE *new_pic = fopen("./cat_all.png", "wb");
    U64 *initial = 0x0a1a0a0d474e5089;
    U32 *ihdr_len = 0x0d000000;
    fwrite(&initial, 8, 1, new_pic);
    fwrite(&ihdr_len, 4, 1, new_pic);
    for(i=0;i<4;++i){
        printf("ihdr type: %x\n", (png.p_IDAT->type)[i]);
    }
    fwrite(&(png.p_IHDR->type), 4, 1, new_pic);
    fwrite(png.p_IHDR->p_data, 13, 1, new_pic);
    U32 *ihdr_crc = ntohl(png.p_IHDR->crc);
    fwrite(&ihdr_crc, 4, 1, new_pic);
    U32 *idat_length = ntohl(def_len);
    printf("def_len: %x\n", idat_length);
    fwrite(&idat_length, 4, 1, new_pic);
    fwrite(&(png.p_IDAT->type), 4, 1, new_pic);
    fwrite(def_result_ptr, def_len, 1, new_pic);
    U32 *idat_crc = ntohl(png.p_IDAT->crc);
    fwrite(&idat_crc, 4, 1, new_pic);
    U32 *iend_len = 0x0;
    U64 *iend_hardcoded = 0x826042ae444e4549;
    fwrite(&iend_len, 4, 1, new_pic);
    fwrite(&iend_hardcoded,8,1,new_pic);
    fclose(new_pic);
    




    return 0;
}