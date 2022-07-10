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
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/uri.h>

#include "curl_xml/main.c"

// #include "starter/png_util/lab_png.h"
// #include "starter/png_util/crc.c"
// #include "starter/png_util/zutil.c"
// #include "starter/shm/shm_stack.c"
// #include "starter/png_util/lab_png.h"
// #include "starter/png_util/crc.h"
// #include "starter/png_util/zutil.h"

typedef unsigned char U8;
typedef unsigned int  U32;
typedef unsigned long int U64;

typedef struct recv_buf2 {
    char *buf;       /* memory to hold a copy of received data */
    size_t size;     /* size of valid data in buf in bytes*/
    size_t max_size; /* max capacity of buf in bytes*/
    int seq;         /* >=0 sequence number extracted from http header */
                     /* <0 indicates an invalid seq number */
} RECV_BUF;

typedef struct front {
  char to_visit[10000][256];
  int size;
} FRONT;

//Defines
#define SEED_URL "http://ece252-1.uwaterloo.ca/lab4"
#define ECE252_HEADER "X-Ece252-Fragment: "
#define BUF_SIZE 1048576  /* 1024*1024 = 1M */
#define BUF_INC  524288   /* 1024*512  = 0.5M */

#define CT_PNG  "image/png"
#define CT_HTML "text/html"
#define CT_PNG_LEN  9
#define CT_HTML_LEN 9

#define NUM_OF_ELEMS 50 
#define STACK_SIZE 10 
#define NUM_SEMS 4

#define max(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

//Global Varibales
int server_counter = 0;
int NumberOfElements = 0;
int keychain = 1;
//int url_num;

FRONT frontier;

// url_visited
struct hsearch_data *urls_visited = {0};

// Array
char png_list[500][256];

// Function Declarations
CURL *easy_handle_init(RECV_BUF *ptr, const char *url);
int process_data(CURL *curl_handle, RECV_BUF *p_recv_buf);
int find_http(char *fname, int size, int follow_relative_links, const char *base_url);
void pipeline();
int url_checker(int temp_url);
int response_content(int temp_url);


int find_http(char *buf, int size, int follow_relative_links, const char *base_url)
{

    int i;
    htmlDocPtr doc;
    xmlChar *xpath = (xmlChar*) "//a/@href";
    xmlNodeSetPtr nodeset;
    xmlXPathObjectPtr result;
    xmlChar *href;
		
    if (buf == NULL) {
        return 1;
    }

    doc = mem_getdoc(buf, size, base_url);
    result = getnodeset (doc, xpath);
    if (result) {
        nodeset = result->nodesetval;
        for (i=0; i < nodeset->nodeNr; i++) {
            href = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
            if ( follow_relative_links ) {
                xmlChar *old = href;
                href = xmlBuildURI(href, (xmlChar *) base_url);
                xmlFree(old);
            }
            if ( href != NULL && !strncmp((const char *)href, "http", 4) ) {
                // printf("href: %s\n", href);
                url_num +=1;
                strcpy(frontier.to_visit[url_num], href);
            }
            xmlFree(href);
        }
        xmlXPathFreeObject (result);
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}

int process_html(CURL *curl_handle, RECV_BUF *p_recv_buf)
{
    char fname[256];
    int follow_relative_link = 1;
    char *url = NULL; 
    pid_t pid =getpid();

    curl_easy_getinfo(curl_handle, CURLINFO_EFFECTIVE_URL, &url);
    find_http(p_recv_buf->buf, p_recv_buf->size, follow_relative_link, url); 
    sprintf(fname, "./output_%d.html", pid);
    return 0; //write_file(fname, p_recv_buf->buf, p_recv_buf->size);
}

int process_png(CURL *curl_handle, RECV_BUF *p_recv_buf)
{
    pid_t pid =getpid();
    char fname[256];
    char *eurl = NULL;          /* effective URL */
    curl_easy_getinfo(curl_handle, CURLINFO_EFFECTIVE_URL, &eurl);
    if ( eurl != NULL) {
        printf("The PNG url is: %s\n", eurl);
    }

    sprintf(fname, "./output_%d_%d.png", p_recv_buf->seq, pid);
    return 0; //write_file(fname, p_recv_buf->buf, p_recv_buf->size);
}
/**
 * @brief process teh download data by curl
 * @param CURL *curl_handle is the curl handler
 * @param RECV_BUF p_recv_buf contains the received data. 
 * @return 0 on success; non-zero otherwise
 */

int process_data(CURL *curl_handle, RECV_BUF *p_recv_buf)
{
    CURLcode res;
    char fname[256];
    pid_t pid =getpid();
    long response_code;

    res = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    if ( res == CURLE_OK ) {
	    printf("Response code: %ld\n", response_code);
    }

    if ( response_code >= 400 ) { 
    	fprintf(stderr, "Error.\n");
        return 1;
    }

    char *ct = NULL;
    res = curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_TYPE, &ct);
    if ( res == CURLE_OK && ct != NULL ) {
    	printf("Content-Type: %s, len=%ld\n", ct, strlen(ct));
    } else {
        fprintf(stderr, "Failed obtain Content-Type\n");
        return 2;
    }

    if ( strstr(ct, CT_HTML) ) {
        return process_html(curl_handle, p_recv_buf);
    } else if ( strstr(ct, CT_PNG) ) {
        return process_png(curl_handle, p_recv_buf);
    } else {
        sprintf(fname, "./output_%d", pid);
    }

    //return write_file(fname, p_recv_buf->buf, p_recv_buf->size);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// int main( int argc, char* argv[] ) {

//     //Parse command line options
//     int c;
//     int num_threads = 1;
//     int find_num = 50;
//     char* v = NULL;
//     int logfile = 0;
//     char url[256];
//     char *str = "option requires an argument";

//     while ((c = getopt (argc, argv, "t:m:v:")) != -1) {
//         switch (c) {
//         case 't':
//             num_threads = strtoul(optarg, NULL, 10);
//             printf("option -t specifies a value of %d.\n", num_threads);
//             if (num_threads <= 0) {
//                 fprintf(stderr, "%s: %s > 0 -- 't'\n", argv[0], str);
//                 return -1;
//             }
//             break;
//         case 'm':
//             find_num = strtoul(optarg, NULL, 10);
// 	        printf("option -m specifies a value of %d.\n", find_num);
//             if (find_num < 0 ) { 
//                 fprintf(stderr, "%s: %s 1, 2, or 3 -- 'n'\n", argv[0], str);
//                 return -1;
//             }
//             break;
//         case 'v':
//             v = optarg;
//             break;
//         default:
//             return -1;
//         }
//     }
//     if (optind < argc){
//         strcpy(url, argv[optind]);
//     }
//     else {
//         strcpy(url, SEED_URL);
//     }
//     strcpy(frontier.to_visit[0], url);
//     printf("Url entered: %s\n", url);
    
//     hcreate_r(1024, &urls_visited);

//     ENTRY e, *ep;
//     e.key = malloc(strlen(frontier.to_visit[frontier.size]) + 1);
//     e.data = "1";
//     strcpy(e.key, frontier.to_visit[frontier.size]);
//     hsearch_r(e, ENTER, &ep, &urls_visited);
//     //hsearch_r(e, FIND, &ep, &urls_visited);
//     printf("Url entered: %s\n", ep->key);

//     curl_global_init(CURL_GLOBAL_DEFAULT);

//     // pthread
//     pthread_t thread_id;
//     pthread_create(&thread_id, NULL, pipeline, NULL);
//     pthread_join(thread_id, NULL);


//     //hdestroy_r(&url_visited);

//     return 0;
// }

/**
 * @brief thread function for pipelining
 * @param url takes in the url from main
 */

void pipeline() { 
    
    
    int check_url;
    int check_rt;
    check_url = url_checker(url_num + 1);
    if (check_url == 0) {
        url_num += 1;
        check_rt = response_content(url_num);
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

int url_checker(int temp_url) {

    ENTRY e, *ep;
    e.key = malloc(strlen(frontier.to_visit[frontier.size]) + 1);
    e.data = (void *) keychain;
    keychain++;
    strcpy(e.key, frontier.to_visit[temp_url]);
    hsearch_r(e, FIND, &ep, &urls_visited);

    if ( ep==NULL){
        hsearch_r(e, ENTER, &ep, &urls_visited);
        return 0;
    }
    else if(ep == 0){
        return 1;
    }
    else {
        return 2;
    }

    return 0;
}

/**
 * @brief sends request, checks response, cehsk type of field, collects urls and checks pngs
 * @param url takes in url for curl operations
 * @return returns 0 on success, 1 no operation on response, 2 on error
 */

int response_content(int temp_url) {

    frontier.to_visit[url_num];

    CURL *curl_handle;
    CURLcode res;
    RECV_BUF recv_buf;
    char url[256];
    int ret;
    strcpy(url, frontier.to_visit[url_num]);
    curl_handle = easy_handle_init(&recv_buf, url);
    res = curl_easy_perform(curl_handle);
    ret = process_data(curl_handle, &recv_buf);

    cleanup(curl_handle, &recv_buf);

    if (ret == 1 || ret == 0){ // 1 = response messed up - 0 means success
        // move to next url // recursively call pipeline??
    }
    else if (ret == 2){
        // error happened, move to next url???????
    }

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


