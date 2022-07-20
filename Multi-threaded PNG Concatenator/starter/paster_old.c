//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <curl/curl.h>
#include "cURL/main_write_header_cb.c"
#include "/home/sm8hasan/G_202_10/lab1/starter/png_util/lab_png.h"
//Defines
//#define IMG_URL_SVR_1_IMG_1 "http://ece252-1.uwaterloo.ca:2520/image?img=1"
#define ECE252_HEADER "X-Ece252-Fragment: "
#define BUF_SIZE 1048576  /* 1024*1024 = 1M */
#define BUF_INC  524288   /* 1024*512  = 0.5M */
#define NUM_OF_ELEMS 50

//Global Varibales
CURL *curl_handle;
CURLcode res;
RECV_BUF recv_buf;
char fname[256];
int img;
int sequence;
int server_counter = 0;
//Arrays
char strip_set[NUM_OF_ELEMS];

//Mutex

void * fetch_image(int argc, char* argv[]);
void choose_server(int server_counter);
void strip_header_call(int argc, char* argv[], int img);
void url_choose(int serv, int img);

int main( int argc, char* argv[] ) {
    img = 1;
    fetch_image(argc, argv);
    //printf("%c", fname);
    //Initialize thread pointer

    //Run thread

    //Wait for thread to finish

    //Combine strips with catpng to all.png

    return 0;
}

void * fetch_image(int argc, char* argv[]) {

    int NumberOfElements = 0;
    
    while (NumberOfElements <= 50) {
        choose_server(server_counter);
        strip_header_call(argc, argv, img);
        if (strip_set[recv_buf.seq] == NULL) {
            strip_set[recv_buf.seq] = malloc(sizeof(struct simple_PNG));
            char* buf_i = recv_buf.buf;
            strip_set[recv_buf.seq] = buf_i;
            NumberOfElements += 1;
            printf("Server used: %d\n", server_counter);
        }
        printf("Server used outside: %d\n", server_counter);
        
        if (NumberOfElements == 50){
            break;
        }
    } 

    /* cleaning up */
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    recv_buf_cleanup(&recv_buf);
    for (int i = 0; i < 50; i++) {
        printf("memory %p, seq=%d.\n", strip_set[i], i);
    }
}

void strip_header_call(int argc, char* argv[], int img){
    //CURL *curl_handle;
    //CURLcode res;
    char url[256];
    // RECV_BUF recv_buf;
    // char fname[256];
    pid_t pid =getpid();
    
    recv_buf_init(&recv_buf, BUF_SIZE);

    sprintf(url, "http://ece252-%d.uwaterloo.ca:2520/image?img=%d",server_counter, img);

    // if (argc == 1) {
    //     strcpy(url, IMG_URL); 
    // } else {
    //     strcpy(url, argv[1]);
    // }
    
    //printf("%s: URL is %s\n", argv[0], url);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    /* init a curl session */
    curl_handle = curl_easy_init();

    if (curl_handle == NULL) {
        fprintf(stderr, "curl_easy_init: returned NULL\n");
        return 1;
    }

    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_cb_curl3); 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&recv_buf);
    curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_cb_curl); 
    curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, (void *)&recv_buf);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    
    /* get it! */
    res = curl_easy_perform(curl_handle);

    if( res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } /*else {
	printf("%lu bytes received in memory %p, seq=%d.\n", \
               recv_buf.size, recv_buf.buf, recv_buf.seq);
    }*/

    sprintf(fname, "./output_%d_%d.png", recv_buf.seq, pid);
    //write_file(fname, recv_buf.buf, recv_buf.size);
    //printf("%d\n", recv_buf.buf);
    //printf("%d\n", fname);
}

void choose_server(int server_counter) {
    if (server_counter < 3) {
        server_counter++;
    }
    else if (server_counter >= 3) {
        server_counter = 1;
    }
}