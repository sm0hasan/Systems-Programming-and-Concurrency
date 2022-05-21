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

typedef struct chunk {
    U32 length;  /* length of data in the chunk, host byte order */
    U8  type[4]; /* chunk type */
    U8  *p_data; /* pointer to location where the actual data are */
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
int is_png(U8 *buf, size_t n);
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
int get_png_data_IHDR(struct data_IHDR *out, FILE *fp, long offset, int whence);

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
    int if_iend = 0;
    int i = 0;
    int tracker = 0;
    U32 chunk_type_hex;
    U32 data_chunk[24] = {0};
    U32 small_chunk[100] = {0};
    char temp[2];
    chunk_p chunk;
    size_t a;
    size_t b;
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
        int index = 0;
        chunk_type_hex = 1229278788;
        for(i; i<100 && readed_files[i+1]!=NULL; i++){
            if(tracker == 1){
                data_chunk[1] = ntohl(readed_files[i]);
                data_chunk[1] = ((data_chunk[0]<<8)>>8)*256;
                // printf("%0x\n", data_chunk[0]);
                data_chunk[2] = ntohl(readed_files[i+1]);
                data_chunk[2] = (data_chunk[1]>>24);
                // printf("%0x\n", data_chunk[1]);
                data_chunk[1] = ntohl(data_chunk[0] + data_chunk[1]);
                data_chunk[0] = ntohl(chunk_type_hex);
                data_chunk[2] = 0;
                break;
            }
            if((ntohl(readed_files[i])<<16)>>16 == num_iend){
                tracker = 1;
            }
        }

    }
    ///////////Automated code for extracting chunk//////////////
    if(chunk_type != iend){
        int index = 0;
        for(i; i<100 && readed_files[i+1]!=NULL; i++){
            if(if_iend == 0){
                if((ntohl(readed_files[i])<<16)>>16 == back_num){
                    tracker = 0;
                    data_chunk[index] = ntohl(chunk_type_hex);
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
    for(i=0; i<20;i++){
        small_chunk[small_index] = ntohl(data_chunk[0])>>24;
        small_chunk[small_index+1] = (ntohl(data_chunk[0])>>16)-small_chunk[small_index];
        small_chunk[small_index+2] = ((ntohl(data_chunk[0]<<16))>>24);
        small_chunk[small_index+3] = ((ntohl(data_chunk[0]<<24))>>24);
        small_index += 4;
    }
    for(i=0; i<100; i++){
        printf("%x\n", small_chunk[i]);
    }
    chunk.p_data = data_chunk;
    for(i=0; i<20;i++){
        if(data_chunk[i+1] == 0){
            chunk.length = 4*(i);
            chunk.crc = data_chunk[i-1];
        }
        // printf("%0x\n", ntohl(data_chunk[i]));
    }

    return chunk;
}