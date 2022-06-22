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

    //Arguments taken in.
    int buffer_size = 2;
    int num_producers = 1;
    int num_consumers = 1;
    int sleep = 10;
    int image = 1;
    
    // fork
    int i=0;
    int k=0;
    pid_t pid=0;
    pid_t cpids[NUM_CHILD];
    int state;
    double times[2];
    struct timeval tv;

    // Shared mem
    key_t key;
    int shmid = shmget(IPC_PRIVATE, sizeof(U8), IPC_CREAT | 0600);
    int shmid1 = shmget(IPC_PRIVATE, sizeof(U8), IPC_CREAT | 0600);
    int shmid2 = shmget(IPC_PRIVATE, sizeof(U64), IPC_CREAT | 0600);
    void *mem;
    *strip_set = shmat(shmid, NULL, 0);//global
    U8 *dest_total_ptr = shmat(shmid1, NULL, 0); //consumer and parent
    U64 *dest_len_total_ptr = shmat(shmid2, NULL, 0); //consumer and parent

    // Get time
    if (gettimeofday(&tv, NULL) != 0) {
        perror("gettimeofday");
        abort();
    }
    times[0] = (tv.tv_sec) + tv.tv_usec/1000000.;
    
    //Child
    ///////////Producer///////////////
    for ( i = 0; i < NUM_CHILD; i++) {
        
        pid = fork();

        if ( pid > 0 ) {        /* parent proc */
            cpids[i] = pid;
        } else if ( pid == 0 ) { /* child proc */
            // producer
            if(i==0){
                // *strip_set = shmat(shmid, NULL, 0);
                // printf("Producer: %d\n", *test);
                int img = 1;
                //void *img_ptr = img;
                for(k=1;k<=50;k++){
                    printf("producer good\n");
                    //void *k_ptr=k;
                    producer(img, k);
                    for(k=0;k<10;k++){
                        printf("producer data[%d]: %x\n", k, strip_set[0][k]);
                    }
                }
                
            }else{
                waitpid(cpids[0], NULL, 0);
                // *strip_set = shmat(shmid, NULL, 0);
                // *dest_total_ptr = shmat(shmid, NULL, 0);
                // *dest_len_total_ptr = shmat(shmid, NULL, 0);
                consumer(strip_set, dest_total_ptr, dest_len_total_ptr);
                printf("Consumer: %d\n", 0);
            }
            break;
        } else {
            perror("fork");
            abort();
        }
        
    }
    //////////////////////////////////

    // Parent
    if ( pid > 0 ) {            /* parent process */
        for ( i = 0; i < NUM_CHILD; i++ ) {
            waitpid(cpids[i], &state, 0);          
            if (WIFEXITED(state)) {
                printf("Child cpid[%d]=%d terminated with state: %d.\n", i, cpids[i], state);
            }
              
        }
        // *strip_set = shmat(shmid, NULL, 0);
        // *dest_total_ptr = shmat(shmid, NULL, 0);
        // *dest_len_total_ptr = shmat(shmid, NULL, 0);
        parent_catpng(strip_set, dest_total_ptr, dest_len_total_ptr);
        if (gettimeofday(&tv, NULL) != 0) {
            perror("gettimeofday");
            abort();
        }
        times[1] = (tv.tv_sec) + tv.tv_usec/1000000.;
        printf("Parent pid = %d: total execution time is %.6lf seconds\n", getpid(),  times[1] - times[0]);
        // printf("test value: %d\n", *test);
        shmdt(*strip_set);
        shmdt(*dest_total_ptr);
        shmdt(*dest_len_total_ptr);
        shmctl(shmid, IPC_RMID, NULL);
    }
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



/**
 * @brief  read strip from buffer --> sleep for X ms --> inflate data --> copy to shared memory
 */

int consumer(U8 *array[], U64 *shdest_total, U8 *shdest_len_total){
    //var declare//
    int k = 0;
    U64 dest_len=0;
    U64 dest_len_total=0;
    U8 dest_total[2000000]; 
    U32 index_flag=0;

    //Loop to inflate all data// *TO Be CHanged
    for(k=0;k<50;++k){
        U8 dest[2000000];   
        int i = 0;
        simple_PNG_p png = get_image_thread(array[k]);
        // for(i=0; i<chunk.length; ++i){
        //     printf("idat: %x\n", chunk.actual_data[i]);
        // }
        dest_len = 0;
        i = mem_inf(&dest, &dest_len, png.p_IDAT->p_data, png.p_IDAT->length);
        printf("length: %d\n", dest_len);
        zerr(i);
        if(i!=0){
            printf("Cat failed %d\n", k);
            zerr(i);
            break;
        }
        
        dest_len_total += dest_len;
         
    
        int p = 0;
        for(i=index_flag; i<(index_flag+dest_len); ++i){
            dest_total[i] = dest[p];
            ++p;
        }
        index_flag += dest_len;

    }
    shdest_total = dest_total;
    shdest_len_total = dest_len_total;
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

int parent_catpng(U8 *array[], U8 *dest_total, U64 *dest_len_total){
    //////Var declare////////
    int k=0;
    U8 def_result[2000000];
    U8 *def_result_ptr = def_result;
    U64 def_len = 0;
    int total_height = 300;

    ///////////Deflate Operation//////////////////
    k = mem_def(def_result_ptr, &def_len, dest_total, dest_len_total, Z_DEFAULT_COMPRESSION);
    int i=0;
    for(i=0;i<500;i++){
        printf("def result: %x\n",def_result[i]);
    }    
    if(k==0){
        printf("yessir\n");
    }else{
        printf("mem_def failed\n");
    }
    //////////////////////////////////////////

    ////////////STRUCT PNG ASSIGNMENT//////////
    simple_PNG_p png;
    png = get_image_thread(array[0]);

    
    /*INITIAL ARRAY*/
    k=0;
    /*       */
    /* Height */
    U8 height_arr[4];
    height_arr[0] = (total_height >> 24);
    height_arr[1] = ((total_height << 8)>>24);
    height_arr[2] = ((total_height <<16)>>24);
    height_arr[3] = ((total_height << 24)>>24);
    k=0;
    for(i=4;i<8;++i){
        png.p_IHDR->p_data[i] = height_arr[k];
        printf("%x\n", height_arr[k]);
        ++k;
    }
    // for(i=0;i<(png.p_IHDR->length);++i){
    //     printf("The current IHDR: %x\n", png.p_IHDR->p_data[i]);
    // }
    ///////IHDR CRC///////////
    U8 *ihdr_data_crc_ptr;
    U8 ihdr_data_crc[120];

    for(i=0;i<4;++i){
        ihdr_data_crc[i] = png.p_IHDR->type[i];
    }
    k=0;
    for(i=4;i<(4+(png.p_IHDR->length));++i){
        ihdr_data_crc[i] = png.p_IHDR->p_data[k];
        ++k;
    }
    // for(i=0;i<50;++i){
    //     printf("current ihdr: %x\n", png.p_IDAT->length);
    // }
    
    ihdr_data_crc_ptr = ihdr_data_crc; 
    U32 crc_ihdr = crc(ihdr_data_crc_ptr, (png.p_IHDR->length)+4);
    png.p_IHDR->crc = crc_ihdr;
    printf("new ihdr crc: %x\n", png.p_IHDR->crc);///IHDR CRC: WORKS/////
    U32 ihdr_total[120];
    // ihdr_total[0] = htonl(png.p_IHDR->length);
    U32 *ihdr_total_ptr;

    
    /*        */
    png.p_IDAT->p_data = def_result;
    png.p_IDAT->length = def_len;
    U8 *idat_data_crc_ptr;
    U8 idat_data_crc[40000];
    

    /* IDAT */
    
    for(i=0;i<4;++i){
        idat_data_crc[i] = png.p_IDAT->type[i];
    }
    k=0;
    for(i=4;i<(4+(png.p_IDAT->length));++i){
        idat_data_crc[i] = png.p_IDAT->p_data[k];
        ++k;
    }
    idat_data_crc_ptr = idat_data_crc;
    png.p_IDAT->crc = crc(idat_data_crc_ptr, (png.p_IDAT->length)+4);
    printf("new idata crc:%x\n", png.p_IDAT->crc);
    
    FILE *new_pic = fopen("./cat_all.png", "wb");
    U64 *initial = 0x0a1a0a0d474e5089;
    U32 *ihdr_len = 0x0d000000;
    fwrite(&initial, 8, 1, new_pic);
    fwrite(&ihdr_len, 4, 1, new_pic);
    for(i=0;i<4;++i){
        printf("ihdr type: %x\n", (png.p_IDAT->type)[i]);
    }
    fwrite(&(png.p_IHDR->type), 4, 1, new_pic);
    fwrite(png.p_IHDR->p_data, 13, 1, new_pic);
    U32 *ihdr_crc = ntohl(png.p_IHDR->crc);
    fwrite(&ihdr_crc, 4, 1, new_pic);
    U32 *idat_length = ntohl(def_len);
    printf("def_len: %x\n", idat_length);
    fwrite(&idat_length, 4, 1, new_pic);
    fwrite(&(png.p_IDAT->type), 4, 1, new_pic);
    fwrite(def_result_ptr, def_len, 1, new_pic);
    U32 *idat_crc = ntohl(png.p_IDAT->crc);
    fwrite(&idat_crc, 4, 1, new_pic);
    U32 *iend_len = 0x0;
    U64 *iend_hardcoded = 0x826042ae444e4549;
    fwrite(&iend_len, 4, 1, new_pic);
    fwrite(&iend_hardcoded,8,1,new_pic);
    fclose(new_pic);

    return 1;
}