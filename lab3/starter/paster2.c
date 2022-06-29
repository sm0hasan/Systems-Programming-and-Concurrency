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
#include <semaphore.h>
#include <signal.h>

//Defines
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


void choose_server();
int producer(int img, int part, int shmid, RECV_BUF *p_shm_recv_buf, sem_t curl_sem);
int consumer(U8 *array, U8 (*shdest_total)[], U64 *shdest_len_total, U64 *index_flag);
// int cat_inf(U8 *array[], U32 index_flag, );
int parent_catpng(U8 *array, struct int_stack *infstack, U64 *dest_len_total);

int main( int argc, char* argv[] ) {

    //Arguments taken in.
    int i=0;
    int k=0;
    int buffer_size = 1;
    int num_producers = 1;//P
    int num_consumers = 1;//C
    int num_child = num_producers + num_consumers;
    int sleep = 0;
    int image = 1;

    int arguments = argc;
    printf("Arguments: %d\n", arguments);
    if (arguments != 1){
        if (arguments < 6){
            printf("missing argument\n");
            exit(-1);
        }
        if (arguments > 6) {
        printf("too many arguments\n");
        exit(-1);
        }
        buffer_size = atoi(argv[1]);
        num_producers = atoi(argv[2]);
        num_consumers = atoi(argv[3]);
        sleep = atoi(argv[4]);
        image = atoi(argv[5]);
        num_child = num_producers + num_consumers;
    }

    // VAR DECLARATION
    sem_t *sems; // 0: spaces (is_full), 1: mutex, 2: items(is_empty)
    int buf_size = sizeof_shm_recv_buf(BUF_SIZE);
    int shm_size = sizeof_shm_stack(buffer_size);
    int inf_size = sizeof_shm_stack(50);
    int stack_id = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    int buf_id = shmget(IPC_PRIVATE, buf_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    int inf_stack_id = shmget(IPC_PRIVATE, inf_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    int sem_id = shmget(IPC_PRIVATE, sizeof(sem_t) * NUM_SEMS, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    int parray[num_producers];
    int carray[num_consumers];
    for(i=0;i<num_producers;i++){
        parray[i]=(50/num_producers)*i;
    }
    parray[num_producers] = 50;
    for(i=0;i<num_consumers;i++){
        carray[i]=(50/num_consumers)*i;
    }
    carray[num_consumers] = 50;
    // fork
    pid_t pid=0;
    pid_t pid1=0;
    pid_t cpids[num_child];
    //pid_t ppids[num_child];
    int state;
    int state_c;
    double times[2];
    struct timeval tv;

    // Shared mem
    int shmid1 = shmget(IPC_PRIVATE, sizeof(U8)*500000, IPC_CREAT | 0600);//Consumer and parent for destination array
    int shmid2 = shmget(IPC_PRIVATE, sizeof(U64), IPC_CREAT | 0600);//Consumer and parent for destination array_size
    int shmid3 = shmget(IPC_PRIVATE, sizeof(U32), IPC_CREAT | 0600);//Consumer for index_flag for each consumer
    int shmid4 = shmget(IPC_PRIVATE, sizeof(U64), IPC_CREAT | 0600);//Consumer and parent for destination array_size
    sems = shmat(sem_id, NULL, 0);
    for(i=0;i<NUM_SEMS;i++){
        sem_init(&sems[i], num_child, 1);
    }

    
    U8 (*dest_total_ptr)[500000]; //consumer and parent
    dest_total_ptr = shmat(shmid1, NULL, 0);
    U64 *dest_len_total_ptr; //consumer and parent
    U64 *dest_len_total_ptr_prev;
    dest_len_total_ptr = shmat(shmid2, NULL, 0);
    dest_len_total_ptr_prev = shmat(shmid4, NULL, 0);
    struct int_stack *pstack; 
    struct int_stack *inf_stack;
    pstack = shmat(stack_id, NULL, 0);
    inf_stack = shmat(inf_stack_id, NULL, 0);
    init_shm_stack(pstack, buffer_size);
    init_shm_stack(inf_stack, 50);

    // Get time
    if (gettimeofday(&tv, NULL) != 0) {
        perror("gettimeofday");
        abort();
    }
    times[0] = (tv.tv_sec) + tv.tv_usec/1000000.;
    
    //Child
    for ( i = 0; i < num_child; i++) {
        
        pid = fork();

        if ( pid > 0 ) {        /* parent proc */
            cpids[i] = pid;
        } else if ( (pid == 0) && (i < num_producers)) { /* Producer proc */
            //Initializing receive stack
            
            if ( pstack == (void *) -1 ) {
                perror("shmat");
                abort();
            }
            
            int k=0;
            
            for(k=parray[i];k<parray[i+1];k++){

                sem_wait(&sems[0]);

                RECV_BUF *p_shm_recv_buf;
                p_shm_recv_buf = shmat(buf_id, NULL, 0);
                if(p_shm_recv_buf == -1){
                    perror("shmat");
                    abort();
                }
                shm_recv_buf_init(p_shm_recv_buf, BUF_SIZE);
                sem_wait(&sems[1]);
                producer(1, k, stack_id, p_shm_recv_buf, sems[3]);
                U8 *buf_i = p_shm_recv_buf->buf;
                memcpy(&arr_of_each_strip.arr, buf_i, p_shm_recv_buf->size);
                arr_of_each_strip.seq = k;
                push(pstack, arr_of_each_strip);
                sem_post(&sems[1]);
                int sem2_track=0;
                sem_getvalue(&sems[2], &sem2_track);
                
                sem_post(&sems[2]);
            
                shmdt(p_shm_recv_buf);
            }

            exit(0);
            break;
        } else if((pid == 0) && (i >=num_producers)){ // Consumer Proc

            usleep(sleep*1000);
            if ( pstack == (void *) -1 ) {
                perror("shmat");
                abort();
            }
            
            if(i==num_producers){
                sem_wait(&sems[2]);//it should only wait for the first time
            }
            
            U32 index_flag = 0;
            *dest_len_total_ptr = 0;
            for(k=carray[i-num_producers];k<carray[i-num_producers+1];k++){
                sem_wait(&sems[2]);
                sem_wait(&sems[1]);
                int a=0;
                a = pstack->pos;
                printf("pstack pos: %d\n", a);
                consumer(&((pstack->items[a]).arr), dest_total_ptr, dest_len_total_ptr, dest_len_total_ptr_prev);
                index_flag = *dest_len_total_ptr_prev;// size of each inf_strip
                arr_of_each_inf.index = index_flag;
                arr_of_each_inf.seq = ((pstack->items[a]).seq);
                printf("arr_of_each_inf.seq = %d", arr_of_each_inf.seq);
                memcpy(&arr_of_each_inf.arr, dest_total_ptr, arr_of_each_inf.index);
                push(inf_stack, arr_of_each_inf);
                pop(pstack);
                printf("pop_count: %d\n", k);
                sem_post(&sems[1]);
                int sem0_track =0;
                sem_getvalue(&sems[0], &sem0_track);
                sem_post(&sems[0]);
            }
            
            for(k=0;k<10;k++){
                printf("inf_stack[0][%d]: %x\n",k,(inf_stack->items[0]).arr[k]);
            }
            exit(0);
        }else {
            perror("fork");
            abort();
        }
        
    }

    if ( pid > 0 ) {            /* consumer parent */
        for ( i = 0; i < num_child; i++ ) {
            waitpid(cpids[i], &state, 0);          
            if (WIFEXITED(state)) {
                printf("Child cpid[%d]=%d terminated with state: %d.\n", i, cpids[i], state);
            }
              
        }
        
        if (gettimeofday(&tv, NULL) != 0) {
            perror("gettimeofday");
            abort();
        }
        times[1] = (tv.tv_sec) + tv.tv_usec/1000000.;
        printf("Parent pid = %d: total execution time is %.6lf seconds\n", getpid(),  times[1] - times[0]);
        
        printf("dest_len_total before cat: %d\n", *dest_len_total_ptr);
        parent_catpng(&((pstack->items[0]).arr), inf_stack, dest_len_total_ptr);
        for(i=0;i<NUM_SEMS;i++){
            sem_destroy(&sems[i]);
        }
    }
    kill(cpids[num_producers-1], SIGKILL);
    kill(cpids[num_child-1], SIGKILL);
    return 0;
}

/**
 * @brief  request image strips from server --> save to buffer
 * 
 * Will need Multiple Write callback
 */

int producer(int img, int part, int shmid, RECV_BUF *p_shm_recv_buf, sem_t curl_sem) {
    CURL *curl_handle;
    CURLcode res;
    char url[256];
    char fname[256];

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
    sem_wait(&curl_sem);
    curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_cb_curl); 
    curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, (void *)p_shm_recv_buf);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    
    res = curl_easy_perform(curl_handle);
    sem_post(&curl_sem);
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

    /* cleaning up */
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    
    return 0;
}



/**
 * @brief  read strip from buffer --> sleep for X ms --> inflate data --> copy to shared memory
 */

int consumer(U8 *array, U8 (*shdest_total)[], U64 *shdest_len_total, U64 *index_flag){
    //var declare//
    int k = 0;
    U64 dest_len=0;
    U64 dest_len_total=0;

    //Loop to inflate all data// *TO Be CHanged
    U8 dest[20000] = {0};   
    int i = 0;
    simple_PNG_p png = get_image_thread(array);

    i = mem_inf(&dest, &dest_len, png.p_IDAT->p_data, png.p_IDAT->length);
    printf("length: %d\n", dest_len);
    zerr(i);

    int p = 0;
    for(i=0; i<dest_len; ++i){
        (*shdest_total)[i] = dest[p];
        ++p;
    }
    
    *index_flag = dest_len;
    *shdest_len_total += dest_len;

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
    int b=0;
    int c=50;
    U8 dest_total[2000000];
    U8 def_result[2000000];
    U8 *def_result_ptr = def_result;
    U64 def_len = 0;
    int total_height = 300;
    U32 dest_total_index=0;
    ///////////copy stack to array///////////
    while(c!=0){
        for(i=0;i<50;i++){
            a=dest_total_index;
            if((infstack->items[i]).seq == b){
                for(k=0;k<((infstack->items[i]).index);k++){
                    dest_total[a] = ((infstack->items[i]).arr[k]);
                ++a;
                }
                ++b;
                --c;
                dest_total_index += ((infstack->items[i]).index);
                printf("dest_total_index:%d\n",((infstack->items[i]).index));
            }
        }    
    }
    ///////////Deflate Operation/////////////////
    k = mem_def(def_result_ptr, &def_len, &dest_total, *dest_len_total, Z_DEFAULT_COMPRESSION);
    printf("def_len: %d\n", def_len);
        
    if(k==0){
        printf("yessir\n");
    }else{
        printf("mem_def failed\n");
    }
    //////////////////////////////////////////

    ////////////STRUCT PNG ASSIGNMENT//////////
    simple_PNG_p png = get_image_thread(array);
    
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
        ++k;
    }

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
    FILE *new_pic = fopen("./cat.png", "wb");
    U64 *initial = 0x0a1a0a0d474e5089;
    U32 *ihdr_len = 0x0d000000;
    fwrite(&initial, 8, 1, new_pic);
    fwrite(&ihdr_len, 4, 1, new_pic);

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

    return 0;
}
