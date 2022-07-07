// Libraries
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
#include "stack.h"

#include "curl_xml/main.c"

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
#define STACK_SIZE 10 
#define NUM_SEMS 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

//Global Varibales
int server_counter = 0;
int NumberOfElements = 0;

// Stack
struct int_stack *frontier;

// Array
U8 png_list[50]

// Function Declarations
void pipeline(const char *url);
int url_checker(const char *url);
int response_type(const char *url);


int main( int argc, char* argv[] ) {

    //Parse command line options
    int c;
    int num_threads = 1;
    int find_num = 50;
    int logfile = 0;
    char url[256];
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
            break;
        default:
            return -1;
        }
    }
    strcpy(url, argv[optind]);

    // Initialize
    
    // Stack - frontier
    if ( (frontier = create_stack(STACK_SIZE)) == NULL) {
        fprintf(stderr, "Failed to create a new stack, abort...\n");
        abort();
    };

    // Hash table - url_visited
    hcreate(10000);

    // pthread
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, pipeline, NULL);
    pthread_join(thread_id, NULL);



    return 0;
}

/**
 * @brief thread function for pipelining
 * @param url takes in the url from main
 */

void pipeline(const char *url) { 

    int check_url;
    int check_rt;
    check_url = url_checker();
    if (check_url == 0) {
        check_rt = response_type();
        if (check_rt == 0){ // success
            // move to next url
            // if (no next url || max # of png urls have been found) {
            //    if (no threads are processing an url){
            //        terminate
            //    }
            // }
            // call pipeline recursively with new url 
        }
        else if (check_rt == 1){ // bad response
            // move to next url
            // if (no next url || max # of png urls have been found) {
            //    if (no threads are processing an url){
            //        terminate
            //    }
            // }
            // call pipeline recursively with new url  
        }
    }
    else if (check_url == 1){
        // move to next url
        // if (no next url || max # of png urls have been found) {
        //    if (no threads are processing an url){
        //        terminate
        //    }
        // }
        // call pipeline recursively with new url 
    }

    return 0;
}

/**
 * @brief checks if in url_visited, adds to url_visited if not there
 * @param url takes in the url from main
 * @param const char *url is the target url to fetch resoruce
 * @return returns 0 on success, 1 on url_visited already, 2 on error
 */

int url_checker() {

    //take in url

    // Search hash table
    // If exists
        // return 1
    // Else If does not exist
        // return 0
    // Else
        // return 2

    return 0;
}

/**
 * @brief sends request, checks response, cehsk type of field, collects urls and checks pngs
 * @param url takes in url for curl operations
 * @return returns 0 on success, 1 no operation on response, 2 on error
 */

int response_type() {

    // take in url
    
    // send request
    
    //check response

    //Error check
    // If response HTTP/1.1 5XX || HTTP/1.1 4XX
        // return 1
    // Else if response HTTP/1.1 3XX
        // feed the 'curl_easy_setopt' with the 'CURLOPT_FOLLOWLOCATION'
        // AND THE 'CURLOPT_MAXREDIRS' allows us to specify the # of redirects
    // Else if response HTTP/1.1 2XX
        // check the type of field
        // If HTML Page
            // Collect the urls and add to frontier
            // return 0
        // Else if PNG
            // Check Png header (need to download)
            // add to png url list
            // return 0


    return 0;
}


