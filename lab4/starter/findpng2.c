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
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>
#include <search.h>

#include "starter/curl_xml/main.c"

// #include "starter/png_util/lab_png.h"
// #include "starter/png_util/crc.c"
// #include "starter/png_util/zutil.c"
// #include "starter/shm/shm_stack.c"
// #include "starter/png_util/lab_png.h"
// #include "starter/png_util/crc.h"
// #include "starter/png_util/zutil.h"


//Defines
#define SEED_URL "http://ece252-1.uwaterloo.ca/lab4"
#define ECE252_HEADER "X-Ece252-Fragment: "
#define BUF_SIZE 1048576  /* 1024*1024 = 1M */
#define BUF_INC  524288   /* 1024*512  = 0.5M */
#define NUM_OF_ELEMS 50
#define NUM_CHILD 20 // P+C
#define STACK_SIZE 10 //B
#define NUM_SEMS 4
#define SLEEP 200 //X
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

//Global Varibales
int server_counter = 0;
int NumberOfElements = 0;
//Arrays
U8 *strip_set[50];
// int test = 10;
struct strip_arr arr_of_each_strip;
struct strip_arr arr_of_each_inf;
//sems declaration



int main( int argc, char* argv[] ) {

    if (argc != 1) {
		char * tmp = argv[1];
        strcpy(init_url, tmp); 
    } else {
		char * tmp = SEED_URL; 
        strcpy(init_url, tmp); 
    }

    //Parse command line options
    int c;
    int num_threads = 1;
    int find_num = 50;
    int logfile = 0;
    char *str = "option requires an argument";
    
    while ((c = getopt (argc, argv, "t:n:v:")) != -1) {
        switch (c) {
        case 't':
	    num_threads = strtoul(optarg, NULL, 10);
	    printf("option -t specifies a value of %d.\n", num_threads);
	    if (num_threads <= 0) {
                fprintf(stderr, "%s: %s > 0 -- 't'\n", argv[0], str);
                return -1;
            }
            break;
        case 'm':
            find_num = strtoul(optarg, NULL, 10);
	    printf("option -m specifies a value of %d.\n", find_num);
            if (find_num < 0 ) { 
                fprintf(stderr, "%s: %s 1, 2, or 3 -- 'n'\n", argv[0], str);
                return -1;
            }
            break;
        case 'v':
            find_num = optarg
	        // printf("option -m specifies a value of %d.\n", find_num);
            if (find_num == "" ) { 
                fprintf(stderr, "missing file");
                return -1;
            }
            break;
        default:
            return -1;
        }
    }
    if (argc - optind < 6) {
        fprintf(stderr, "%s: too few arguments\n", argv[0]);
        usage(argv[0]);
    }

    //put seed url onto URL Frontiers

    // Thread 1 - function pipeline
        // takes from URL frontiers
        // check if url exists already through the hash table
        // If not
            // then add it to the table
            // send request and check response
            // Error check resopnse
            // If 2xx or 3xx
                // check type field
                // If HTML Page
                    // Collect URLs and add to frontier
                // Else PNG
                    // Check PNG Header
                    // Add to png url list
            // Else 4xx or 5xx
                // Move to next URL
        // if so
            // then go to next url

    // make sure to check if stack empty before trying to move to next url

}