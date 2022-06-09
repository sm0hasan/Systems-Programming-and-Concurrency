/**
 * @brief  micros and structures for a simple PNG file 
 *
 * Copyright 2018-2020 Yiqing Huang
 *
 * This software may be freely redistributed under the terms of MIT License
 */
#pragma once

/******************************************************************************
 * INCLUDE HEADER FILES
 *****************************************************************************/
#include <stdio.h>
#include <string.h>

/******************************************************************************
 * DEFINED MACROS 
 *****************************************************************************/

#define PNG_SIG_SIZE    8 /* number of bytes of png image signature data */
#define CHUNK_LEN_SIZE  4 /* chunk length field size in bytes */          
#define CHUNK_TYPE_SIZE 4 /* chunk type field size in bytes */
#define CHUNK_CRC_SIZE  4 /* chunk CRC field size in bytes */
#define DATA_IHDR_SIZE 13 /* IHDR chunk data field size */

/******************************************************************************
 * STRUCTURES and TYPEDEFS 
 *****************************************************************************/
typedef unsigned char U8;
typedef unsigned int  U32;
typedef unsigned long int U64;

typedef struct chunk {
    U64 length;  /* length of data in the chunk, host byte order */
    U8  type[4]; /* chunk type */
    U8  *p_data; /* pointer to location where the actual data are */
    U8 *actual_data[4000];
    U32 crc;     /* CRC field  */
} chunk_p;

/* note that there are 13 Bytes valid data, compiler will padd 3 bytes to make
   the structure 16 Bytes due to alignment. So do not use the size of this
   structure as the actual data size, use 13 Bytes (i.e DATA_IHDR_SIZE macro).
 */
typedef struct data_IHDR {// IHDR chunk data 
    U32 width;        /* width in pixels, big endian   */
    U32 height;       /* height in pixels, big endian  */
    U8  bit_depth;    /* num of bits per sample or per palette index.
                         valid values are: 1, 2, 4, 8, 16 */
    U8  color_type;   /* =0: Grayscale; =2: Truecolor; =3 Indexed-color
                         =4: Greyscale with alpha; =6: Truecolor with alpha */
    U8  compression;  /* only method 0 is defined for now */
    U8  filter;       /* only method 0 is defined for now */
    U8  interlace;    /* =0: no interlace; =1: Adam7 interlace */
} data_IHDR_p;

/* A simple PNG file format, three chunks only*/
typedef struct simple_PNG {
    struct chunk *p_IHDR;
    struct chunk *p_IDAT;  /* only handles one IDAT chunk */  
    struct chunk *p_IEND;
} simple_PNG_p;

/******************************************************************************
 * FUNCTION PROTOTYPES 
 *****************************************************************************/
int is_png(U32 *buf){
    int *png_bytes = -1991225785;
    int i = ntohl(buf[0]);
    if(i == png_bytes){
        return 1;
    }else{
        return 0;
    }
};
int get_png_height(struct data_IHDR buf){
    int i;
    i = buf.height;
    return i;
};
int get_png_width(struct data_IHDR buf){
    int i;
    i = buf.width;
    return i;
};
// int get_png_data_IHDR(struct data_IHDR *out, FILE *fp, long offset, int whence);

/* declare your own functions prototypes here */
chunk_p get_chunk(U32 readed_files[], char chunk_type){

    ///////Declaration///////
    char ihdr = "ihdr";
    char idat = "idat";
    char iend = "iend";
    int front_num = 0;
    int back_num = 0;
    int num_ihdr = 17490;
    int num_idat = 17473;
    int num_iend = 17742;
    int num_iend_1 = 20036;
    int if_iend = 0;
    int i = 0;
    int tracker = 0;
    U32 chunk_type_hex;
    U32 data_chunk[30] = {0};
    U32 actual_chunk[30] = {0};
    U32 small_chunk[100] = {0};
    U32 normal_chunk[30] = {0};
    char temp[2];
    chunk_p chunk;
    size_t a;
    size_t b;
    int iend_mode;
    /////////////////////////
    if(chunk_type == ihdr){
        front_num = num_ihdr;
        back_num = num_idat;
        chunk_type_hex = 1229472850;
    }else if(chunk_type == idat){
        front_num = num_idat;
        back_num = num_iend;
        chunk_type_hex = 1229209940;
    }else if(chunk_type == iend){
        int index = 1;
        chunk_type_hex = 1229278788;
        for(i; i<100 && readed_files[i+1]!=NULL; i++){
            if(tracker == 1){
                data_chunk[0] = ntohl(chunk_type_hex);
                data_chunk[index] = ntohl(readed_files[i]);
                ++index;
            }
            if((ntohl(readed_files[i])<<16)>>16 == num_iend){
                tracker = 1;
                iend_mode = 0;
            }else if((ntohl(readed_files[i])<<16)>>16 == num_iend_1){
                tracker = 1;
                iend_mode = 1;
            }
        }
        data_chunk[3] = 16777215;
        normal_chunk[0] = ntohl(data_chunk[0]);
        normal_chunk[1] = ntohl(data_chunk[1]);
        normal_chunk[2] = ntohl(data_chunk[2]);
        normal_chunk[3] = data_chunk[3];
    }
    ///////////Automated code for extracting chunk//////////////
    if(chunk_type != iend){
        int index = 1;
        for(i; i<100 && readed_files[i]!=NULL; i++){
            if(chunk_type == ihdr){
                if((ntohl(readed_files[i-1])<<16)>>16 == back_num){
                    tracker = 0;
                    data_chunk[0] = ntohl(chunk_type_hex);
                    data_chunk[index] = 16777215;
                    break;
                }
            }else{
                if(((ntohl(readed_files[i])<<16)>>16 == back_num) || ((ntohl(readed_files[i+1])<<16)>>16 == num_iend_1)){
                    tracker = 0;
                    data_chunk[0] = ntohl(chunk_type_hex);
                    data_chunk[index] = 16777215;
                    break;
                }
            }
            if(tracker == 1){
                data_chunk[index] = readed_files[i];
                ++index;
            }
            if((ntohl(readed_files[i])<<16)>>16 == front_num){
                tracker = 1;
            }

        }

    }
    ////////////////////////////////////////////////////////////
    int small_index = 0;
    for(i=0; i<30; i++){
        printf("%x\n", data_chunk[i]);
    }

    //////////////get crc for idat//////////////
    if(chunk_type == idat){
        chunk.p_data = &data_chunk;
        U8 *temp_idat = &data_chunk;
        tracker = 0;
        small_index = 0;
        for(i=0; i<90; i++){//i for this loop *=4i (size of data_chunk)

            if((chunk.p_data[i+1]==0xff) && (chunk.p_data[i+2]==0xff) && (chunk.p_data[i+3]==0xff)){
                small_chunk[small_index] = 0xffffff;
                tracker = 0;
                // printf("tracker == 2\n");
                break;
            }
            if(tracker == 1){
                if(i<=4){
                    small_chunk[small_index] = chunk.p_data[i-1];
                    ++small_index;
                }else{
                    small_chunk[small_index] = chunk.p_data[i];
                    ++small_index;
                }
                
            }
            
            if((chunk.p_data[i]==73) && (chunk.p_data[i+1]==68) && (chunk.p_data[i+2]==65) && (chunk.p_data[i+3]==84)){
                tracker = 1;
                
            }
        }
        // small_chunk[small_index] = 99999999;
        // printf("Small index: %x\n", small_chunk[small_index]);
        U32 crc;
        for(i=0; i<90; i++){
            if(small_chunk[i] == 0xffffff){
                crc = (small_chunk[i-6]<<24) + (small_chunk[i-5]<<16) + (small_chunk[i-4]<<8) + (small_chunk[i-3]);
                // printf("crc: %x\n", crc);
                chunk.length = i-6;
                break;
            }
            // printf("%x\n", small_chunk[i]);
        }
        small_index = 0;
        for(i=0;i<30;i++){
            data_chunk[i] = htonl((small_chunk[small_index]<<24) + (small_chunk[small_index+1]<<16) + (small_chunk[small_index+2]<<8) + (small_chunk[small_index+3]));
            small_index+=4;
            // printf("%x\n", data_chunk[i]);
        }
        chunk.p_data = &data_chunk;
        chunk.crc = crc;
    }
    ////////////////////get crc for ihdr/////////////////////
    if(chunk_type == ihdr){
        U32 temp_chunk[2];
        for(i=0; i<30; i++){
            ///////get crc/////////
            if(chunk_type == ihdr){
                if(data_chunk[i+3] == 16777215){
                    temp_chunk[0] = ntohl(data_chunk[i]);
                    temp_chunk[1] = ntohl(data_chunk[i+1]);
                    temp_chunk[0] = (temp_chunk[0]<<8) + (temp_chunk[1]>>24);
                    // printf("crc: %x\n", temp_chunk[0]);
                    break;
                }
            }
        }
        chunk.p_data = &data_chunk;
        chunk.crc = temp_chunk[0];
        chunk.length = 17;
        for(i=0; i<30; ++i){
            normal_chunk[i] = data_chunk[i];
        }
        
    }
    /////////////////////////////////////////////////////////
    if(chunk_type == iend){
        small_index = 0;
        U32 crc_iend;
        chunk.p_data = &normal_chunk;
        for(i=0;i<30;i++){
            if(iend_mode == 0){
                if((chunk.p_data[i] == 255)){
                    small_chunk[small_index] = chunk.p_data[i-7];
                    small_chunk[small_index+1] = chunk.p_data[i-6];
                    small_chunk[small_index+2] = chunk.p_data[i-5];
                    small_chunk[small_index+3] = chunk.p_data[i-4];
                    crc_iend = htonl((small_chunk[0]<<24) + (small_chunk[1]<<16) + (small_chunk[2]<<8) + (small_chunk[3]));
                    // printf("crc_iend: %x\n", ntohl(crc_iend));
                    break;
                }
            }else if(iend_mode == 1){
                if((chunk.p_data[i] == 255)){
                    small_chunk[small_index] = chunk.p_data[i-6];
                    small_chunk[small_index+1] = chunk.p_data[i-5];
                    small_chunk[small_index+2] = chunk.p_data[i-4];
                    small_chunk[small_index+3] = chunk.p_data[i-3];
                    crc_iend = htonl((small_chunk[0]<<24) + (small_chunk[1]<<16) + (small_chunk[2]<<8) + (small_chunk[3]));
                    // printf("crc_iend: %x\n", ntohl(crc_iend));
                    break;
                }
            }
            // printf("%x\n", data_chunk[i]);
        }
        chunk.p_data = &data_chunk;
        chunk.crc = ntohl(crc_iend);
        chunk.length = 4;
        // U32 iend_type;
        // iend_type = htonl(0x49454e44);
    }

    return chunk;
}

chunk_p extract_actual_chunk(U32 readed_files[], char chunk_type){
        ///////Declaration///////
    char ihdr = "ihdr";
    char idat = "idat";
    char iend = "iend";
    int front_num = 0;
    int back_num = 0;
    int num_ihdr = 17490;
    int num_idat = 17473;
    int num_iend = 17742;
    int num_iend_1 = 20036;
    int if_iend = 0;
    int i = 0;
    int tracker = 0;
    U32 chunk_type_hex;
    U32 data_chunk[1000] = {0};
    U32 actual_chunk[30] = {0};
    U32 small_chunk[4000] = {0};
    U32 normal_chunk[30] = {0};
    char temp[2];
    chunk_p chunk;
    size_t a;
    size_t b;
    int iend_mode;
    /////////////////////////
    if(chunk_type == ihdr){
        front_num = num_ihdr;
        back_num = num_idat;
        chunk_type_hex = 1229472850;
    }else if(chunk_type == idat){
        front_num = num_idat;
        back_num = num_iend;
        chunk_type_hex = 1229209940;
    }else if(chunk_type == iend){
        int index = 1;
        chunk_type_hex = 1229278788;
        for(i; i<100 && readed_files[i+1]!=NULL; i++){
            if(tracker == 1){
                data_chunk[0] = ntohl(chunk_type_hex);
                data_chunk[index] = ntohl(readed_files[i]);
                ++index;
            }
            if((ntohl(readed_files[i])<<16)>>16 == num_iend){
                tracker = 1;
                iend_mode = 0;
            }else if((ntohl(readed_files[i])<<16)>>16 == num_iend_1){
                tracker = 1;
                iend_mode = 1;
            }
        }
        data_chunk[3] = 16777215;
        normal_chunk[0] = ntohl(data_chunk[0]);
        normal_chunk[1] = ntohl(data_chunk[1]);
        normal_chunk[2] = ntohl(data_chunk[2]);
        normal_chunk[3] = data_chunk[3];
    }
    ///////////Automated code for extracting chunk//////////////
    if(chunk_type != iend){
        int index = 0;
        for(i; i<1000 && readed_files[i]!=NULL; i++){
            if(chunk_type == ihdr){
                if((ntohl(readed_files[i-1])<<16)>>16 == back_num){
                    tracker = 0;
                    data_chunk[1] = ntohl(chunk_type_hex);
                    data_chunk[index] = 16777215;
                    break;
                }
            }else{
                if(((ntohl(readed_files[i-2])<<16)>>16 == back_num) || ((ntohl(readed_files[i-1])<<16)>>16 == num_iend_1)){
                    tracker = 0;
                    break;
                }
            }
            if(tracker == 1){
                data_chunk[index] = readed_files[i];
                ++index;
            }
            if((ntohl(readed_files[i+2])<<16)>>16 == front_num){
                tracker = 1;
            }

        }

    }
    ////////////////////////////////////////////////////////////
    int small_index = 0;
    for(i=0; i<1000; i++){
        printf("%x\n", data_chunk[i]);
    }

    //////////////get crc for idat//////////////
    if(chunk_type == idat){
        chunk.p_data = &data_chunk;
        tracker = 0;
        small_index = 0;
        for(i=0; i<4000; i++){//i for this loop *=4i (size of data_chunk)
            
            if((chunk.p_data[i]==73) && (chunk.p_data[i+1]==68) && (chunk.p_data[i+2]==65) && (chunk.p_data[i+3]==84)){
                // printf("tracker == 2\n");
                // small_chunk[small_index] = chunk.p_data[i-4];
                // small_chunk[1] = chunk.p_data[i-3];
                // small_chunk[2] = chunk.p_data[i-2];
                // small_chunk[3] = chunk.p_data[i-1];
                // small_chunk[4] = chunk.p_data[i];
                // small_chunk[5] = chunk.p_data[i+1];
                // small_chunk[6] = chunk.p_data[i+2];
                // small_chunk[7] = chunk.p_data[i+3];
                // small_index = 8;
                chunk.length = (chunk.p_data[i-4]<<24) + (chunk.p_data[i-3]<<16) + (chunk.p_data[i-2]<<8) + (chunk.p_data[i-1]);
                printf("Chunk.length: %x\n", chunk.length);
                i = i+4;
                tracker = i;
                break;
                
            }
        }
        for(i=tracker;i<(chunk.length+9);++i){
            chunk.p_data[small_index] = chunk.p_data[i];
            // printf("idat: %x\n", chunk.p_data[i]);
            ++small_index;
        }
        // U8 tiny_chunk[4000];
        // for(i=0;i<4000;i++){
        //     chunk.actual_data[i] = small_chunk[i];
        //     // printf("small: %x\n", small_chunk[i]);
        // }
    }
    ////////////////////get crc for ihdr/////////////////////
    if(chunk_type == ihdr){
        U32 temp_chunk[2];
        for(i=0; i<30; i++){
            ///////get crc/////////
            if(chunk_type == ihdr){
                if(data_chunk[i+3] == 16777215){
                    temp_chunk[0] = ntohl(data_chunk[i]);
                    temp_chunk[1] = ntohl(data_chunk[i+1]);
                    temp_chunk[0] = (temp_chunk[0]<<8) + (temp_chunk[1]>>24);
                    // printf("crc: %x\n", temp_chunk[0]);
                    break;
                }
            }
        }
        chunk.p_data = &data_chunk;
        chunk.crc = temp_chunk[0];
        chunk.length = 17;
        for(i=0; i<30; ++i){
            normal_chunk[i] = data_chunk[i];
        }
        for(i=0;i<30;++i){
            if(data_chunk)
            actual_chunk[0] = data_chunk[i+1];
        }
    }
    /////////////////////////////////////////////////////////
    if(chunk_type == iend){
        small_index = 0;
        U32 crc_iend;
        chunk.p_data = &normal_chunk;
        for(i=0;i<30;i++){
            if(iend_mode == 0){
                if((chunk.p_data[i] == 255)){
                    small_chunk[small_index] = chunk.p_data[i-7];
                    small_chunk[small_index+1] = chunk.p_data[i-6];
                    small_chunk[small_index+2] = chunk.p_data[i-5];
                    small_chunk[small_index+3] = chunk.p_data[i-4];
                    crc_iend = htonl((small_chunk[0]<<24) + (small_chunk[1]<<16) + (small_chunk[2]<<8) + (small_chunk[3]));
                    // printf("crc_iend: %x\n", ntohl(crc_iend));
                    break;
                }
            }else if(iend_mode == 1){
                if((chunk.p_data[i] == 255)){
                    small_chunk[small_index] = chunk.p_data[i-6];
                    small_chunk[small_index+1] = chunk.p_data[i-5];
                    small_chunk[small_index+2] = chunk.p_data[i-4];
                    small_chunk[small_index+3] = chunk.p_data[i-3];
                    crc_iend = htonl((small_chunk[0]<<24) + (small_chunk[1]<<16) + (small_chunk[2]<<8) + (small_chunk[3]));
                    // printf("crc_iend: %x\n", ntohl(crc_iend));
                    break;
                }
            }
            // printf("%x\n", data_chunk[i]);
        }
        chunk.p_data = &data_chunk;
        chunk.crc = ntohl(crc_iend);
        chunk.length = 4;
        // U32 iend_type;
        // iend_type = htonl(0x49454e44);
    }

    return chunk;
}


simple_PNG_p get_image(U32 readed_files[]){
    struct simple_PNG png;
    chunk_p rt;
    ///////Declaration///////
    struct chunk ihdr;
    struct chunk idat;
    struct chunk iend;
    U8 *chunk_ptr;
    U8 *ntohl_ptr;
    U8 *file_ptr;
    U32 data_chunk[100000] = {0};
    U32 ntohl_data_chunk[100000] = {0};
    U8 chunk_ihdr[120];
    U8 chunk_idat[90000];
    U8 chunk_iend[80];
    int i = 0;
    int k = 0;
    ////////////////////////
    ////LINKs/////
    png.p_IHDR = &ihdr;
    png.p_IDAT = &idat;
    png.p_IEND = &iend;
    ihdr.p_data = &chunk_ihdr;
    idat.p_data = &chunk_idat;
    iend.p_data = &chunk_iend;
    ///////Move over readed_files//////////
    for(i=0;i<100000;++i){
        if(readed_files[i] == NULL){
            break;
        }
        data_chunk[i] = readed_files[i];
        ntohl_data_chunk[i] = ntohl(readed_files[i]);
        // printf("data_chunk[%d]: %x\n", i, ntohl(data_chunk[i]));
    }
    chunk_ptr = &data_chunk;
    ntohl_ptr = &ntohl_data_chunk;
    file_ptr = &readed_files;
    /////////////////////////////////////
    
    //////////////Length Assignment/////////////////
    ihdr.length = ntohl(data_chunk[2]);
    // for(i=33;i<37;++i){
    //     printf("ntohl_chunk[%d]: %x\n",i,chunk_ptr[i]);
    // }
    idat.length = (chunk_ptr[33]<<24) + (chunk_ptr[34]<<16) + (chunk_ptr[35]<<8) + (chunk_ptr[36]);
    iend.length = 0;
    // printf("length: %x\n", idat.length);
    ///////////////////////////////////////////////

    /////////////Data, CRC, and TYPE////////////////
    /* IHDR */
    k = 0;
    for(i=12;i<16;++i){
        ihdr.type[k] = chunk_ptr[i];
        ++k;
    }
    k = 0;
    for(i=16; i<29; ++i){ //ihdr data chunk
        chunk_ihdr[k] = chunk_ptr[i];
        // printf("ntohl_chunk[%d]: %x\n",k,png.p_IHDR->p_data[k]);
        ++k;
    }
    ihdr.crc = (chunk_ptr[29]<<24) + (chunk_ptr[30]<<16) + (chunk_ptr[31]<<8) + (chunk_ptr[32]);
    // printf("IHDR CRC: %x\n", png.p_IHDR->crc);
    
    /* IDAT */
    k=0;
    for(i=37;i<41;++i){//Idat type
        idat.type[k] = chunk_ptr[i];
        ++k;
    }
    k=0;
    for(i=41;i<(41+(idat.length));++i){
        chunk_idat[k] = chunk_ptr[i];
        ++k;
    }
    i = 41+(idat.length);
    idat.crc = (chunk_ptr[i]<<24) + (chunk_ptr[i+1]<<16) + (chunk_ptr[i+2]<<8) + (chunk_ptr[i+3]);
    // printf("IDAT CRC: %x\n", png.p_IDAT->crc);

    /* IEND */
    long int m;
    long int n;
    m = i+8;
    n = i+12;
    int p = i+4;
    k=0;
    for(m;m<n;++m){
        iend.type[k] = chunk_ptr[m];
        // printf("ntohl_chunk[%d]: %x\n",k,png.p_IEND->type[k]);
        ++k;
    }
    iend.crc = (chunk_ptr[n]<<24) + (chunk_ptr[n+1]<<16) + (chunk_ptr[n+2]<<8) + (chunk_ptr[n+3]);
    // printf("IEND CRC: %x\n", png.p_IEND->crc);
    return png;
    
}

simple_PNG_p get_image_thread(U8 *readed_files[]){
    struct simple_PNG png;
    chunk_p rt;
    ///////Declaration///////
    struct chunk ihdr;
    struct chunk idat;
    struct chunk iend;
    U8 *chunk_ptr;
    U8 *ntohl_ptr;
    U8 *file_ptr;
    U32 data_chunk[100000] = {0};
    U32 ntohl_data_chunk[100000] = {0};
    U8 chunk_ihdr[120];
    U8 chunk_idat[90000];
    U8 chunk_iend[80];
    int i = 0;
    int k = 0;
    ////////////////////////
    ////LINKs/////
    png.p_IHDR = &ihdr;
    png.p_IDAT = &idat;
    png.p_IEND = &iend;
    ihdr.p_data = &chunk_ihdr;
    idat.p_data = &chunk_idat;
    iend.p_data = &chunk_iend;
    ///////Move over readed_files//////////
    // for(i=0;i<100000;++i){
    //     if(readed_files[i] == NULL){
    //         break;
    //     }
    //     data_chunk[i] = readed_files[i];
    //     ntohl_data_chunk[i] = ntohl(readed_files[i]);
    //     // printf("data_chunk[%d]: %x\n", i, ntohl(data_chunk[i]));
    // }
    chunk_ptr = readed_files;
    ntohl_ptr = &ntohl_data_chunk;
    file_ptr = &readed_files;
    /////////////////////////////////////
    
    //////////////Length Assignment/////////////////
    ihdr.length = chunk_ptr[11];
    // for(i=33;i<37;++i){
    //     printf("ntohl_chunk[%d]: %x\n",i,chunk_ptr[i]);
    // }
    idat.length = (chunk_ptr[33]<<24) + (chunk_ptr[34]<<16) + (chunk_ptr[35]<<8) + (chunk_ptr[36]);
    iend.length = 0;
    // printf("length: %x\n", idat.length);
    ///////////////////////////////////////////////

    /////////////Data, CRC, and TYPE////////////////
    /* IHDR */
    k = 0;
    for(i=12;i<16;++i){
        ihdr.type[k] = chunk_ptr[i];
        ++k;
    }
    k = 0;
    for(i=16; i<29; ++i){ //ihdr data chunk
        chunk_ihdr[k] = chunk_ptr[i];
        printf("ntohl_chunk[%d]: %x\n",k,png.p_IHDR->p_data[k]);
        ++k;
    }
    ihdr.crc = (chunk_ptr[29]<<24) + (chunk_ptr[30]<<16) + (chunk_ptr[31]<<8) + (chunk_ptr[32]);
    // printf("IHDR CRC: %x\n", png.p_IHDR->crc);
    
    /* IDAT */
    k=0;
    for(i=37;i<41;++i){//Idat type
        idat.type[k] = chunk_ptr[i];
        ++k;
    }
    k=0;
    for(i=41;i<(41+(idat.length));++i){
        chunk_idat[k] = chunk_ptr[i];
        ++k;
    }
    i = 41+(idat.length);
    idat.crc = (chunk_ptr[i]<<24) + (chunk_ptr[i+1]<<16) + (chunk_ptr[i+2]<<8) + (chunk_ptr[i+3]);
    // printf("IDAT CRC: %x\n", png.p_IDAT->crc);

    /* IEND */
    long int m;
    long int n;
    m = i+8;
    n = i+12;
    int p = i+4;
    k=0;
    for(m;m<n;++m){
        iend.type[k] = chunk_ptr[m];
        // printf("ntohl_chunk[%d]: %x\n",k,png.p_IEND->type[k]);
        ++k;
    }
    iend.crc = (chunk_ptr[n]<<24) + (chunk_ptr[n+1]<<16) + (chunk_ptr[n+2]<<8) + (chunk_ptr[n+3]);
    // printf("IEND CRC: %x\n", png.p_IEND->crc);
    return png;
    
}