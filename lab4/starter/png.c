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
#include "shm_stack.c"
#include <curl/curl.h>

#include "curl_xml/main.c"

// #include "starter/png_util/lab_png.h"
// #include "starter/png_util/crc.c"
// #include "starter/png_util/zutil.c"
// #include "starter/shm/shm_stack.c"
// #include "starter/png_util/lab_png.h"
// #include "starter/png_util/crc.h"
// #include "starter/png_util/zutil.h"



// typedef struct recv_buf2 {
//     char *buf;       /* memory to hold a copy of received data */
//     size_t size;     /* size of valid data in buf in bytes*/
//     size_t max_size; /* max capacity of buf in bytes*/
//     int seq;         /* >=0 sequence number extracted from http header */
//                      /* <0 indicates an invalid seq number */
// } RECV_BUF;



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
int url_num;

FRONT frontier;
struct int_stack *frontier_stk;

// url_visited
struct hsearch_data *urls_visited = {0};

// Array
char png_list[500][256];

// Function Declarations

// Function Declarations
int find_http(char *fname, int size, int follow_relative_links, const char *base_url);
void pipeline();
int url_checker(int temp_url);
int response_content(int temp_url);

int main( int argc, char* argv[] ) {

    //Parse command line options
    int c;
    int num_threads = 1;
    int find_num = 50;
    char* v = NULL;
    int logfile = 0;
    char url[256];
    char *str = "option requires an argument";
    frontier_stk = create_stack(50);


    while ((c = getopt (argc, argv, "t:m:v:")) != -1) {
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
            v = optarg;
            break;
        default:
            return -1;
    
    }
    }
    if (optind < argc){
        strcpy(url, argv[optind]);
    }
    else {
        strcpy(url, SEED_URL);
    }
    url_num = 0;
    strcpy(frontier.to_visit[url_num], url);
    // struct char_stack item;
    // struct char_stack item2;
    // strcpy(item.char_list, url);
    // push(frontier_stk, item); 
    // push(frontier_stk, item2); 
    // // char *p_item;
    // pop(frontier_stk);

    // printf("Url entered: %s\n", (frontier_stk->items[frontier_stk->pos]).char_list);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    pipeline();
    pop(frontier_stk);
    printf("Url entered: %s\n", (frontier_stk->items[frontier_stk->pos]).char_list);
    // pthread
    // pthread_t thread_id;
    // pthread_create(&thread_id, NULL, pipeline, NULL);
    // pthread_join(thread_id, NULL);


    //hdestroy_r(&url_visited);

    return 0;
}

/**
 * @brief thread function for pipelining
 * @param url takes in the url from main
 */

void pipeline() { 
    
    
    int check_rt;
    check_rt = response_content(url_num);

    return 0;
}

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
        printf("response bad or sucsess\n");// move to next url // recursively call pipeline??
    }
    else if (ret == 2){
        printf("erorr occured\n");
        // error happened, move to next url???????
    }


    return 0;
}









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
                strcpy(frontier.to_visit[index_val], href);
                struct char_stack char_stk;
                strcpy(char_stk.char_list, href);
                push(frontier_stk, char_stk);
                printf("The Frontier stored URL is : %s\n", (frontier_stk->items[frontier_stk->pos]).char_list);
                index_val += 1;
            }
            xmlFree(href);
        }
        xmlXPathFreeObject (result);
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}