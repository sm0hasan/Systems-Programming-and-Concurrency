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
#include "starter/shm_stack.c"
#include <curl/curl.h>

#include "starter/curl_xml/main.c"

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

pthread_mutex_t frontier_mutex; 
pthread_mutex_t png_list_mutex;
pthread_mutex_t hash_mutex;
pthread_mutex_t check_num;

pthread_cond_t cond_var;
pthread_cond_t cond_var1;

//Global Varibales
int server_counter = 0;
int NumberOfElements = 0;
int keychain = 1;
int url_num;
int first_url = 1;
int find_num = 50;

FRONT frontier;
struct int_stack *frontier_stk;
struct int_stack *png_stk;
struct int_stack *visited_stk;

// url_visited
struct hsearch_data *urls_visited = {0};

// Array
// char png_list[500][1024];
// png_list[0] = malloc(sizeof(char)*1024*500);
int png_list_index=0;

// Function Declarations

// Function Declarations
int find_http(char *fname, int size, int follow_relative_links, const char *base_url);
void* pipeline() { 
    //printf("thread execution\n");
    int check_url;
    int check_rt;
    int return_status;
    int active = 0;
    //printf("Entered pipeline\n");
    pthread_mutex_lock(&frontier_mutex);
    while(is_empty(frontier_stk)){
        //printf("stuck in cond_var 1\n");
        pthread_cond_wait(&cond_var1, &frontier_mutex);
    }
    pthread_mutex_unlock(&frontier_mutex);
    //printf("out of cond_var1\n");
    while (is_empty(frontier_stk) == 0){
        pthread_mutex_lock(&frontier_mutex);
        if((png_stk->pos >= (find_num - 1)) || (png_stk->pos >= 49)) {// check whether enough png is found
            pthread_mutex_unlock(&frontier_mutex);
            break;
        }
        pthread_mutex_unlock(&frontier_mutex);
        char url[256];
        strcpy(url, (frontier_stk->items[frontier_stk->pos]).char_list);
        //printf("url_current: %s\n", url);
        pthread_mutex_lock(&frontier_mutex);
        pop(frontier_stk);
        pthread_mutex_unlock(&frontier_mutex);
        check_url = url_checker(url);
        //printf("After url_checker\n");
        if (check_url == 0) {
            check_rt = response_content(url);
            pthread_cond_signal(&cond_var);
            pthread_cond_signal(&cond_var1);
            //pthread_cond_broadcast(&cond_var);
            //printf("After resposne_content\n");
            if (check_rt == 0){ // success
                return_status = 0;
            }
            else if (check_rt == 1){ // bad response
                return_status = 1;
            }
        }
        else if (check_url == 1){ // aready there
            return_status = 1;
        }
        pthread_mutex_lock(&frontier_mutex);
        while(frontier_stk->pos == -1){
            if((png_stk->pos >= (find_num - 1)) || (png_stk->pos >= 49)){
                pthread_mutex_unlock(&frontier_mutex);
                break;
            }    
            pthread_cond_wait(&cond_var, &frontier_mutex); 
        }
        pthread_mutex_unlock(&frontier_mutex);
    }
    //printf("thread finished\n");
    pthread_mutex_unlock(&frontier_mutex);
    pthread_mutex_unlock(&png_list_mutex);
    pthread_mutex_unlock(&hash_mutex);
    pthread_mutex_unlock(&check_num);
    pthread_cond_signal(&cond_var);
    pthread_cond_signal(&cond_var1);
    //return return_status;
}
int url_checker(char url[]);
int response_content(char url[]);
int png_check(unsigned char *buf){
    int a = 0x89;
    int b = 0x50;
    int c = 0x4e;
    int d = 0x47;
    if(buf[0] == a && buf[1] == b && buf[2] == c && buf[3] == d){
        return 1;
    }
    return 0;
}

int main( int argc, char* argv[] ) {
    xmlInitParser();
    pthread_t th[8];
    pthread_mutex_init(&frontier_mutex, NULL);
    pthread_mutex_init(&png_list_mutex, NULL);
    pthread_mutex_init(&hash_mutex, NULL);
    pthread_mutex_init(&check_num, NULL);
    pthread_cond_init(&cond_var, NULL);
    pthread_cond_init(&cond_var1, NULL);
    //Parse command line options
    int log_check = 0;
    int i = 0;
    int c;
    int num_threads = 1;
    char* v = NULL;
    int logfile = 0;
    char url[256];
    char *str = "option requires an argument";
    frontier_stk = create_stack(1000);
    png_stk = create_stack(500);
    visited_stk = create_stack(2000);
    double times[2];
    struct timeval tv;
    // char png_list[500][1024] = malloc(sizeof(char)*500*1024);

    while ((c = getopt (argc, argv, "t:m:v:")) != -1) {
        switch (c) {
        case 't':
            num_threads = strtoul(optarg, NULL, 10);
            //printf("option -t specifies a value of %d.\n", num_threads);
            if (num_threads <= 0) {
                fprintf(stderr, "%s: %s > 0 -- 't'\n", argv[0], str);
                return -1;
            }
            break;
        case 'm':
            find_num = strtoul(optarg, NULL, 10);
	        //printf("option -m specifies a value of %d.\n", find_num);
            if (find_num < 0 ) { 
                fprintf(stderr, "%s: %s 1, 2, or 3 -- 'n'\n", argv[0], str);
                return -1;
            }
            break;
        case 'v':
            log_check = 1;
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

    char seed[256];
    strcpy(seed, url);

    // URLs Visited
    hcreate_r(1024, &urls_visited);
    ENTRY e, *ep;
    e.key = malloc(sizeof(char)*256);
    strcpy(e.key, url);
    hsearch_r(e, ENTER, &ep, &urls_visited);  
    //printf("SEED URL Entered: %s\n", ep->key);


    // // Stack
    struct char_stack char_stk;
    strcpy(char_stk.char_list, url);
    push(frontier_stk, char_stk);
    //push(visited_stk, char_stk);
    //printf("is empty%d\n", is_empty(frontier_stk));
    //pipeline();
    /////////Thread Init/////////
    // Get time
    if (gettimeofday(&tv, NULL) != 0) {
        perror("gettimeofday");
        abort();
    }
    times[0] = (tv.tv_sec) + tv.tv_usec/1000000.;


    for(i=0;i<num_threads;i++){
        if (pthread_create(&th[i], NULL, &pipeline, NULL) != 0){
            perror("Failed to create thread\n");
        }
    }
    for(i=0;i<num_threads;i++){
        if(pthread_join(th[i], NULL) != 0){
            perror("failed to join thread\n");
        }
        //sleep(1);
    }
    
    if (gettimeofday(&tv, NULL) != 0) {
            perror("gettimeofday");
            abort();
        }
        times[1] = (tv.tv_sec) + tv.tv_usec/1000000.;
        printf("findpng2 execution time: %.6lf seconds\n",  times[1] - times[0]);


    /////////////////////////////
    ///////Testing png_stack/////
    FILE *fl;
    fl = fopen( "png_urls.txt" , "w" );

    //printf("png_stk.pos: %d\n", png_stk->pos);
    for(i=0;i< find_num;i++){
        char purl[10000];
        sprintf(purl, "%s\n", (png_stk->items[(png_stk->pos)-i]).char_list);
        fwrite(purl , 1 , strlen(purl) , fl );
        //printf("png_list[%d]: %s\n", i, (png_stk->items[(png_stk->pos)-i]).char_list);
    }

    if (log_check == 1) {
        FILE *fp;
        fp = fopen( v , "w" );
        char eurls[10000];
        sprintf(eurls, "%s\n", seed);
        fwrite(eurls , 1 , strlen(eurls) , fp );

        for(i=0;i<=visited_stk->pos;i++){
            char eurl[10000];
            sprintf(eurl, "%s\n", (visited_stk->items[(visited_stk->pos)-i]).char_list);
            fwrite(eurl , 1 , strlen(eurl) , fp );
            //printf("visited_list[%d]: %s\n", i, (visited_stk->items[(visited_stk->pos)-i]).char_list);
        }
        fclose(fp);
    }


    //hdestroy_r(&url_visited);
    
    pthread_mutex_destroy(&frontier_mutex);
    pthread_mutex_destroy(&png_list_mutex);
    pthread_mutex_destroy(&hash_mutex);
    pthread_mutex_destroy(&check_num);
    pthread_cond_destroy(&cond_var);
    pthread_cond_destroy(&cond_var1);
    //free(v);
    //free(str);
    //free(&char_stk);
    destroy_stack(frontier_stk);
    destroy_stack(png_stk);
    destroy_stack(visited_stk);
    //frontier_stk = NULL;
    //png_stk = NULL;
    hdestroy_r(urls_visited);
    return 0;
}


/**
 * @brief thread function for pipelining
 * @param url takes in the url from main
 */



int url_checker(char *url) {
    if (first_url == 1){
        first_url = 0;
        return 0;
    }
    //printf("Entered url_checker\n");
    ENTRY e, *ep;

    e.key = malloc(sizeof(char)*256);
    // e.data = (void *) keychain;
    // keychain++;
    
    strcpy(e.key, url);
    pthread_mutex_lock(&hash_mutex);
    hsearch_r(e, FIND, &ep, &urls_visited);
    //printf("url_visited in hash: %s\n", e.key);
    //printf("Url finding: %s\n", e.key);

    if ( ep==NULL){
        hsearch_r(e, ENTER, &ep, &urls_visited); //not there
        struct char_stack char_stk;
        strcpy(char_stk.char_list, url);
        push(visited_stk, char_stk);
        pthread_mutex_unlock(&hash_mutex);
        return 0;
    }
    else if(ep != 0){ // already there
        pthread_mutex_unlock(&hash_mutex);
        return 1;
    }
    else {
        pthread_mutex_unlock(&hash_mutex);
        return 2; // error
    }
    

    return 0;
}

int response_content(char url[]) {
    //printf("Entered response_content\n");
    CURL *curl_handle;
    CURLcode res;
    RECV_BUF recv_buf;
    int ret;

    curl_handle = easy_handle_init(&recv_buf, url);
    res = curl_easy_perform(curl_handle);
    ret = process_data(curl_handle, &recv_buf);
    //printf("png_url_index: %d\n", png_list_index);
    pthread_mutex_lock(&png_list_mutex);
    if(png_check(recv_buf.buf)){
        struct char_stack png_url;
        strcpy(png_url.char_list, url);
        push(png_stk, png_url);
        //printf("str cpy success\n");
    }
    pthread_mutex_unlock(&png_list_mutex);
    cleanup(curl_handle, &recv_buf);

    if (ret == 1 || ret == 0){
        //printf("response bad or sucsess\n");
        return 0;
    }
    else if (ret == 2){
        //printf("erorr occured\n");
        return 1;
    }

    return 1;
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
                pthread_mutex_lock(&frontier_mutex);
                struct char_stack char_stk;
                strcpy(char_stk.char_list, href);
                push(frontier_stk, char_stk);
                //push(visited_stk, char_stk);
                //printf("The Frontier stored URL is : %s\n", (frontier_stk->items[frontier_stk->pos]).char_list);
                pthread_mutex_unlock(&frontier_mutex);
            }
            xmlFree(href);
        }
        xmlXPathFreeObject (result);
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}





