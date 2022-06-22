/*
 * To use paster you can try pasting in the terminal, "./paster -t 10 -n 3".
 * This is where -t signifies the number of threads and -n signifies which image to be used.
 */

//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>
//#include "cURL/main_write_header_cb.c"
// #include "/home/sm8hasan/G_202_10/lab1/starter/png_util/lab_png.h"
//#include "png_util/lab_png.h"
//#include "png_util/crc.h"
//#include "png_util/zutil.h"

//Defines
#define ECE252_HEADER "X-Ece252-Fragment: "
#define BUF_SIZE 1048576  /* 1024*1024 = 1M */
#define BUF_INC  524288   /* 1024*512  = 0.5M */
#define NUM_OF_ELEMS 50

//Global Varibales
int server_counter = 0;


void choose_server();
int producer();
int consumer();

int main( int argc, char* argv[] ) {

    //Arguments taken in.
    int buffer_size = 2;
    int num_producers = 1;
    int num_consumers = 1;
    int sleep = 10;
    int image = 1;
    
    // fork
    //parent

        // Create and initialize sempahores and mutex in shared memory

        //Create producer processes

        //Create consumer processes

        //wait for all processes

        //produce all.png

        //cleanup semaphores, mutex, etc
    


    return 0;
}

/**
 * @brief  request image strips from server --> save to buffer
 * 
 * Will need Multiple Write callback
 */

int producer(){

    //loop to get all data (buffer could be too small)

        //request strip
        
        //wait (spaces) - Blocks if buffer is full

       /* wait (mutex)
        * copy data to buffer
        * post(mutex)
        */
       
       // post(items) - increments

    return 1;
}



/**
 * @brief  read strip from buffer --> sleep for X ms --> inflate data --> copy to shared memory
 */

int consumer(){

    //consuming loop
        
        //wait (items) - Blocks if buffer is empty

       /* wait (mutex)
        * read data from buffer
        * post(mutex)
        */
       
        // post(spaces) - increments

        // further processing

    return 1;
}

void choose_server() {
    int serv;
    if (server_counter < 3) {
        server_counter++;
    }
    else if (server_counter >= 3) {
        server_counter = 1;
    }
}