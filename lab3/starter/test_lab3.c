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
//#include "cURL/main_write_header_cb.c"
// #include "/home/sm8hasan/G_202_10/lab1/starter/png_util/lab_png.h"
#include "png_util/lab_png.h"
#include "png_util/crc.h"
#include "png_util/zutil.h"
#include <pthread.h>
// #include "cURL_IPC/main_2proc.h"
// #include "shm/shm_stack.h"
// #include "shm/shm_struct.c"
#include "cURL/main_write_header_cb.c"
#include "png_util/lab_png.h"
#include "png_util/crc.h"
#include "png_util/zutil.h"

//Defines
#define ECE252_HEADER "X-Ece252-Fragment: "
#define BUF_SIZE 1048576  /* 1024*1024 = 1M */
#define BUF_INC  524288   /* 1024*512  = 0.5M */
#define NUM_OF_ELEMS 50
#define NUM_CHILD 2
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

//Global Varibales
int server_counter = 0;
int NumberOfElements = 0;
//Arrays
U8 *strip_set[NUM_OF_ELEMS];
// int test = 10;



void choose_server();
int producer(int img, int part);
int consumer();
int parent_catpng();

int main( int argc, char* argv[] ) {
    int k=0;
    producer(1, 0);
    for(k=0;k<10;k++){
        printf("producer data[%d]: %x\n", k, strip_set[0][k]);
    }


    


    return 0;
}

/**
 * @brief  request image strips from server --> save to buffer
 * 
 * Will need Multiple Write callback
 */
void choose_server() {
    int serv;
    if (server_counter < 3) {
        server_counter++;
    }
    else if (server_counter >= 3) {
        server_counter = 1;
    }
}
int producer(int img, int part) {
    printf("entereeddddd\n");
    int image = img;
    int strip_num = part;

    CURL *curl_handle;
    CURLcode res;
    char url[256];
    RECV_BUF recv_buf;
    char fname[256];
    pid_t pid =getpid();
    recv_buf_init(&recv_buf, BUF_SIZE);
    
    choose_server();

    sprintf(url, "http://ece252-%d.uwaterloo.ca:2530/image?img=%d&part=%d",server_counter, image, strip_num);
    printf("%s\n", url);
    /* init a curl session */
    curl_handle = curl_easy_init();

    if (curl_handle == NULL) {
        fprintf(stderr, "curl_easy_init: returned NULL\n");
        return 1;
    }   

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_cb_curl3); // add recursion if it doesnt fully get the strip??
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

    //wait(spaces) - block if buffer is full

    //Mutex lock
    // pthread_mutex_lock( &mutex );
    if (strip_set[recv_buf.seq] == NULL) {
        strip_set[recv_buf.seq] = malloc(sizeof(struct chunk));
        U8* buf_i = recv_buf.buf;
        memcpy(strip_set[recv_buf.seq], buf_i, recv_buf.size);
        NumberOfElements ++;
    }
    //Mutex unlock
    // pthread_mutex_unlock( &mutex );

    //post(items)

    /* cleaning up */
    recv_buf_cleanup(&recv_buf);
    return 1;
}