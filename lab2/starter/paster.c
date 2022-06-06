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
#include "cURL/main_write_header_cb.c"
#include "/home/sm8hasan/G_202_10/lab1/starter/png_util/lab_png.h"

//Defines
#define ECE252_HEADER "X-Ece252-Fragment: "
#define BUF_SIZE 1048576  /* 1024*1024 = 1M */
#define BUF_INC  524288   /* 1024*512  = 0.5M */
#define NUM_OF_ELEMS 50

//Global Varibales
int temp_test = 0; //Testing - earse afterwards

int NumberOfElements = 0;
int server_counter = 0;
extern char *optarg;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

//Arrays
char *strip_set[NUM_OF_ELEMS];

void choose_server();
void *strip_header_call(void *img);

int main( int argc, char* argv[] ) {

    //Parse command line options
    int c;
    int num_threads = 1;
    int img = 1;
    char *str = "option requires an argument";
    
    while ((c = getopt (argc, argv, "t:n:")) != -1) {
        switch (c) {
        case 't':
	    num_threads = strtoul(optarg, NULL, 10);
	    printf("option -t specifies a value of %d.\n", num_threads);
	    if (num_threads <= 0) {
                fprintf(stderr, "%s: %s > 0 -- 't'\n", argv[0], str);
                return -1;
            }
            break;
        case 'n':
            img = strtoul(optarg, NULL, 10);
	    printf("option -n specifies a value of %d.\n", img);
            if (img <= 0 || img > 3) {
                fprintf(stderr, "%s: %s 1, 2, or 3 -- 'n'\n", argv[0], str);
                return -1;
            }
            break;
        default:
            return -1;
        }
    }

    //Global initialize
    curl_global_init(CURL_GLOBAL_DEFAULT);

    //Pthread
    pthread_t thread[50];
    memset(thread, 0, sizeof(pthread_t)*50);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(thread + i, NULL, *strip_header_call, &img);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(thread[i], NULL);
    }

    //Print 'strip_set'
    for (int i = 0; i < 50; i++) {
        printf("memory %s, seq=%d.\n", strip_set[i], i);
    }

    //Global Cleanup
    curl_global_cleanup();

    return 0;
}

void *strip_header_call(void *img) {

    int image = *(int *) img;

    while (NumberOfElements <= 50) {
        CURL *curl_handle;
        CURLcode res;
        char url[256];
        RECV_BUF recv_buf;
        char fname[256];
        pid_t pid =getpid();
        recv_buf_init(&recv_buf, BUF_SIZE);
        
        choose_server();

        sprintf(url, "http://ece252-%d.uwaterloo.ca:2520/image?img=%d",server_counter, image);

        /* init a curl session */
        curl_handle = curl_easy_init();

        if (curl_handle == NULL) {
            fprintf(stderr, "curl_easy_init: returned NULL\n");
            return 1;
        }   

        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_cb_curl3); 
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&recv_buf);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_cb_curl); 
        curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, (void *)&recv_buf);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(curl_handle);

        if( res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        sprintf(fname, "./output_%d_%d.png", recv_buf.seq, pid);
        curl_easy_cleanup(curl_handle);

        //Mutex lock
        pthread_mutex_lock( &mutex );
        if (strip_set[recv_buf.seq] == NULL) {
            strip_set[recv_buf.seq] = malloc(sizeof(struct chunk));
            char* buf_i = recv_buf.buf;
            memcpy(strip_set[recv_buf.seq], buf_i, recv_buf.size);
            NumberOfElements ++;
            //Testing - erase afterwards
            printf("%d\n", temp_test);
            temp_test++;
        }
        //Mutex unlock
        pthread_mutex_unlock( &mutex );

        /* cleaning up */
        recv_buf_cleanup(&recv_buf);
        if (NumberOfElements == 50){
            break;
        }
    }

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