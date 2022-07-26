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
#include "cURL_IPC/main.c"
#include "shm/shm_stack.c"
// #include "shm/shm_struct.c"
// #include "cURL/main_write_header_cb.c"
#include "png_util/lab_png.h"
#include "png_util/crc.h"
#include "png_util/zutil.h"

//Defines
#define ECE252_HEADER "X-Ece252-Fragment: "
#define BUF_SIZE 1048576  /* 1024*1024 = 1M */
#define BUF_INC  524288   /* 1024*512  = 0.5M */
#define NUM_OF_ELEMS 50
#define NUM_CHILD 2
#define STACK_SIZE 50
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

//Global Varibales
int server_counter = 0;
int NumberOfElements = 0;
//Arrays
U8 *strip_set[NUM_OF_ELEMS];
// int test = 10;
struct strip_arr arr_of_each_strip;
struct strip_arr arr_of_each_inf;



void choose_server();
int producer(int img, int part, int shmid, RECV_BUF *p_shm_recv_buf);
int consumer(U8 *array, U8 (*shdest_total)[], U64 *shdest_len_total, U32 index_flag);
// int cat_inf(U8 *array[], U32 index_flag, );
int parent_catpng(U8 *array, struct int_stack *infstack, U64 *dest_len_total);

int main( int argc, char* argv[] ) {
    // VAR DECLARATION
    int buf_size = sizeof_shm_recv_buf(BUF_SIZE);
    int shm_size = sizeof_shm_stack(STACK_SIZE);
    int inf_size = sizeof_shm_stack(STACK_SIZE);
    int stack_id = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    int buf_id = shmget(IPC_PRIVATE, buf_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    int inf_stack_id = shmget(IPC_PRIVATE, inf_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);


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
    pid_t pid1=0;
    pid_t cpids[NUM_CHILD];
    pid_t ppids[NUM_CHILD];
    int state;
    int state_c;
    double times[2];
    struct timeval tv;

    // Shared mem
    int shmid1 = shmget(IPC_PRIVATE, sizeof(U8)*500000, IPC_CREAT | 0600);//Consumer and parent for destination array
    int shmid2 = shmget(IPC_PRIVATE, sizeof(U64), IPC_CREAT | 0600);//Consumer and parent for destination array_size
    int shmid3 = shmget(IPC_PRIVATE, sizeof(U32), IPC_CREAT | 0600);//Consumer for index_flag for each consumer
    
    U8 (*dest_total_ptr)[500000]; //consumer and parent
    dest_total_ptr = shmat(shmid1, NULL, 0);
    U64 *dest_len_total_ptr; //consumer and parent
    dest_len_total_ptr = shmat(shmid2, NULL, 0);
    // U32 *index_flag = 0;
    // index_flag = shmat(shmid3, NULL, 0);

    // Get time
    if (gettimeofday(&tv, NULL) != 0) {
        perror("gettimeofday");
        abort();
    }
    times[0] = (tv.tv_sec) + tv.tv_usec/1000000.;
    
    //Child
    ///////////Producer///////////////
    for ( i = 0; i < 2; i++) {
        
        pid = fork();

        if ( pid > 0 ) {        /* parent proc */
            waitpid(pid, &state, 0); // wait for 
            cpids[i] = pid;
        } else if ( (pid == 0) && (i == 0)) { /* Producer proc */
            //Initializing receive stack
            struct int_stack *pstack; 
            pstack = shmat(stack_id, NULL, 0);
            if ( pstack == (void *) -1 ) {
                perror("shmat");
                abort();
            }
            init_shm_stack(pstack, STACK_SIZE);
            int k=0;
            // int abc = push(pstack, 0);
            // if(abc != 0){
            //     printf("PUSH TO STACK FAILED\n");
            //     abort();
            // }

            
            for(k=0;k<50;k++){
                RECV_BUF *p_shm_recv_buf;
                p_shm_recv_buf = shmat(buf_id, NULL, 0);
                if(p_shm_recv_buf == -1){
                    perror("shmat");
                    abort();
                }
                shm_recv_buf_init(p_shm_recv_buf, BUF_SIZE);
                producer(1, k, stack_id, p_shm_recv_buf);
                U8 *buf_i = p_shm_recv_buf->buf;
                memcpy(&arr_of_each_strip.arr, buf_i, p_shm_recv_buf->size);
                push(pstack, arr_of_each_strip);
                shmdt(p_shm_recv_buf);
            }
            // exit(0);
        //testing code
       /*
        for(k=0;k<30;k++){
            printf("producer exited\n");
            printf("producer data[%d]: %x\n", k, strip_set[0][k]);
        }

        for(k=0;k<30;k++){
            printf("producer exited\n");
            printf("producer data[%d]: %x\n", k, (pstack->items[1]).arr[k]);
        }*/
            break;
        } else if((pid == 0) && (i == 1)){ // Consumer Proc
            // waitpid(ppids[0], NULL, 0);
            //Initializing receive stack
            struct int_stack *pstack; 
            struct int_stack *inf_stack;
            pstack = shmat(stack_id, NULL, 0);
            inf_stack = shmat(inf_stack_id, NULL, 0);
            if ( pstack == (void *) -1 ) {
                perror("shmat");
                abort();
            }
            init_shm_stack(pstack, STACK_SIZE);
            init_shm_stack(inf_stack, STACK_SIZE);
            U32 index_flag = 0;
            U64 dest_len_prev = 0;
            *dest_len_total_ptr = 0;
            for(k=0;k<50;k++){
                consumer(&((pstack->items[k]).arr), dest_total_ptr, dest_len_total_ptr, index_flag);
                index_flag = ((*dest_len_total_ptr) - dest_len_prev); // size of each inf_strip
                arr_of_each_inf.index = index_flag;
                memcpy(&arr_of_each_inf.arr, dest_total_ptr, arr_of_each_inf.index);
                push(inf_stack, arr_of_each_inf);
                // printf("dest_lentotal trial%d: %d\n", k, *dest_len_total_ptr);
                dest_len_prev = *dest_len_total_ptr;
            }
            for(k=0;k<10;k++){
                printf("inf_stack[0][%d]: %x\n",k,(inf_stack->items[0]).arr[k]);
            }
        }else {
            perror("fork");
            abort();
        }
        
    }
   /*
    // Parent
    // if(pid>0){ //producer parent
    //     for(i=0;i<1;i++){
    //         waitpid(ppids[i], &state, 0);
    //         if (WIFEXITED(state)) {
    //             printf("Child cpid[%d]=%d terminated with state: %d.\n", i, ppids[i], state);
    //         }
    //     }
    // }
    */
    if ( pid > 0 ) {            /* consumer parent */
        for ( i = 0; i < 2; i++ ) {
            waitpid(cpids[i], &state, 0);          
            if (WIFEXITED(state)) {
                printf("Child cpid[%d]=%d terminated with state: %d.\n", i, cpids[i], state);
            }
              
        }
        // dest_total_ptr = (U8 *)shmat(shmid1, NULL, 0);
        struct int_stack *pstack; 
        struct int_stack *inf_stack;
        pstack = shmat(stack_id, NULL, 0);
        inf_stack = shmat(inf_stack_id, NULL, 0);
        init_shm_stack(pstack, STACK_SIZE);
        init_shm_stack(inf_stack, STACK_SIZE);
        if (gettimeofday(&tv, NULL) != 0) {
            perror("gettimeofday");
            abort();
        }
        times[1] = (tv.tv_sec) + tv.tv_usec/1000000.;
        printf("Parent pid = %d: total execution time is %.6lf seconds\n", getpid(),  times[1] - times[0]);
        // remove("./cat.png");
        // for(k=0;k<10;k++){
        //     printf("inf data before cat: %x\n", (*dest_total_ptr)[k]);
        // }
        printf("dest_len_total before cat: %d\n", *dest_len_total_ptr);
        parent_catpng(&((pstack->items[0]).arr), inf_stack, dest_len_total_ptr);
    }

    return 0;
}

/**
 * @brief  request image strips from server --> save to buffer
 * 
 * Will need Multiple Write callback
 */

int producer(int img, int part, int shmid, RECV_BUF *p_shm_recv_buf) {
    CURL *curl_handle;
    CURLcode res;
    char url[256];
    char fname[256];
    // pid_t pid =getpid();

    printf("entereeddddd\n");
    int image = img;
    int strip_num = part;


    if ( shmid == -1 ) {
        perror("shmget");
        abort();
    }

    
    
    choose_server();
    sprintf(url, "http://ece252-%d.uwaterloo.ca:2530/image?img=%d&part=%d",server_counter, image, strip_num);
    printf("%s\n", url);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    /* init a curl session */
    curl_handle = curl_easy_init();

    if (curl_handle == NULL) {
        fprintf(stderr, "curl_easy_init: returned NULL\n");
        return 1;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_cb_curl); 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)p_shm_recv_buf);
    curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_cb_curl); 
    curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, (void *)p_shm_recv_buf);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    
    res = curl_easy_perform(curl_handle);

    if( res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }else {
	    printf("%lu bytes received in memory %p, seq=%d.\n",  \
        p_shm_recv_buf->size, p_shm_recv_buf->buf, p_shm_recv_buf->seq);
        
    }

    if (strip_set[p_shm_recv_buf->seq] == NULL) {
        strip_set[p_shm_recv_buf->seq] = malloc(sizeof(struct chunk));
        U8* buf_i = p_shm_recv_buf->buf;
        memcpy(strip_set[p_shm_recv_buf->seq], buf_i, p_shm_recv_buf->size);
        NumberOfElements ++;
    }

    // write_file(fname, p_shm_recv_buf->buf, p_shm_recv_buf->size);

    /* cleaning up */
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    // shmdt(p_shm_recv_buf);
    // shmctl(shmid, IPC_RMID, NULL);
    return 0;
}



/**
 * @brief  read strip from buffer --> sleep for X ms --> inflate data --> copy to shared memory
 */

int consumer(U8 *array, U8 (*shdest_total)[], U64 *shdest_len_total, U32 index_flag){
    //var declare//
    int k = 0;
    U64 dest_len=0;
    U64 dest_len_total=0;
    // U8 dest_total[2000000]; 
    // U32 index_flag=0;

    //Loop to inflate all data// *TO Be CHanged
    U8 dest[20000] = {0};   
    int i = 0;
    simple_PNG_p png = get_image_thread(array);
    // for(i=0; i<chunk.length; ++i){
    //     printf("idat: %x\n", chunk.actual_data[i]);
    // }
    // dest_len = 0;
    i = mem_inf(&dest, &dest_len, png.p_IDAT->p_data, png.p_IDAT->length);
    printf("length: %d\n", dest_len);
    zerr(i);

    int p = 0;
    for(i=0; i<dest_len; ++i){
        // printf("index flag: %d", index_flag);
        (*shdest_total)[i] = dest[p];
        // printf("shdest_total[%d]: %x, dest[%d]: %x\n", i, (*shdest_total)[i], p, dest[p]);
        ++p;
    }
    // index_flag += dest_len;
    // printf("dest_len in consumer: %d\n", dest_len);
    // printf("index_flag trial in consumer: %d\n", index_flag);
    
    // shdest_total = dest_total;
    *shdest_len_total += dest_len;
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

int parent_catpng(U8 *array, struct int_stack *infstack, U64 *dest_len_total){
    //////Var declare////////
    int k=0;
    int i=0;
    int a=0;
    U8 dest_total[2000000];
    U8 def_result[2000000];
    U8 *def_result_ptr = def_result;
    U64 def_len = 0;
    int total_height = 300;
    U32 dest_total_index=0;
    ///////////copy stack to array///////////
    for(i=0;i<50;i++){
        a=dest_total_index;
        for(k=0;k<((infstack->items[i]).index);k++){
            dest_total[a] = ((infstack->items[i]).arr[k]);
            ++a;
        }
        dest_total_index += ((infstack->items[i]).index);
        printf("dest_total_index:%d\n",((infstack->items[i]).index));
    }
    ///////////Deflate Operation/////////////////
    k = mem_def(def_result_ptr, &def_len, &dest_total, *dest_len_total, Z_DEFAULT_COMPRESSION);
    printf("def_len: %d\n", def_len);
    
    // for(i=0;i<10;i++){
    //     printf("def result: %x\n",def_result[i]);
    // }    
    if(k==0){
        printf("yessir\n");
    }else{
        printf("mem_def failed\n");
    }
    //////////////////////////////////////////

    ////////////STRUCT PNG ASSIGNMENT//////////
    simple_PNG_p png = get_image_thread(array);
    // printf("get_image failed\n");
    // png = get_image_thread(array);

    
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
        // printf("height: %x\n", height_arr[k]);
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
    
    ihdr_data_crc_ptr = ihdr_data_crc; 
    U32 crc_ihdr = crc(ihdr_data_crc_ptr, (png.p_IHDR->length)+4);
    png.p_IHDR->crc = crc_ihdr;
    printf("new ihdr crc: %x\n", png.p_IHDR->crc);///IHDR CRC: WORKS/////
    U32 ihdr_total[120];
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
    // remove("./cat_all.png");
    FILE *new_pic = fopen("./cat.png", "wb");
    U64 *initial = 0x0a1a0a0d474e5089;
    U32 *ihdr_len = 0x0d000000;
    fwrite(&initial, 8, 1, new_pic);
    fwrite(&ihdr_len, 4, 1, new_pic);
    // for(i=0;i<4;++i){
    //     printf("ihdr type: %x\n", (png.p_IDAT->type)[i]);
    // }
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
    // free(def_result_ptr);

    return 0;
}