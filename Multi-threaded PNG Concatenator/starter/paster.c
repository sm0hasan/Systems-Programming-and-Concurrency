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
// #include "/home/sm8hasan/G_202_10/lab1/starter/png_util/lab_png.h"
#include "/home/q282liu/G_202_10/lab1/starter/png_util/lab_png.h"
#include "/home/q282liu/G_202_10/lab1/starter/png_util/crc.h"
#include "/home/q282liu/G_202_10/lab1/starter/png_util/zutil.h"

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
U8 *strip_set[NUM_OF_ELEMS];

void choose_server();
void *strip_header_call(void *img);
int catpng(char *strip_array[]);

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
        for(int k = 0; k<50;k++){
            printf("pic array[%d][%d]: %x\n", i,k,strip_set[0][k]);
        }
    }
    get_image_thread(strip_set[0]);

    catpng(strip_set);
    // int width;
    // width = ((strip_set[49][18] << 16) + strip_set[49][19]);
    // printf("width: %x\n", width);

    // if((strip_set[48][0]) <0){
    //     strip_set[48][0] -= (2*strip_set[48][0]);
    //     printf("pic array: %x\n", strip_set[48][0]);
    // }

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
            U8* buf_i = recv_buf.buf;
            memcpy(strip_set[recv_buf.seq], buf_i, recv_buf.size);
            NumberOfElements ++;
            // write_file(fname, recv_buf.buf, recv_buf.size);
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
int catpng(char *strip_array[]) 
{
    ///////Variable Declaration//////
    int k;
    int width = 0;
    int height = 0;
    U32 readed_file[100000];
    char chunk_type;
    // int tracker = 0;
    // FILE *files;
    int *png_bytes = -1991225785;
    struct data_IHDR ihdr_in;
    struct chunk chunk;
    U32 crc_calculated;
    U32 total_height = 0;
    
    U8 dest_total[2000000]; 
    U8 def_result[2000000];
    U8 *def_result_ptr = def_result;
    U64 def_len = 0;
    U64 dest_len = 0;
    U64 dest_len_total = 0;
    U64 idat_len = 0;
    U32 index_flag = 0;
    ///////////////////////////////

    ///////file operations//////////////
    for(k=0;k<50;++k){
        U8 dest[2000000];   
        int i = 0;
        // FILE *files;
        // files = fopen(argv[k], "rb");
        // if(files == NULL){
        //     printf("failed to open file");
        //     return 3;
        // }
        // fread(readed_file, sizeof(U32), 100000, files);
        /////////Print width and height///////////
        // (ihdr_in).width = ntohl(strip_array[k][4]);
        // (ihdr_in).height = ntohl(strip_array[k][5]);
        // printf("%s: %d x %d\n", k, get_png_height(ihdr_in), get_png_width(ihdr_in));
        // total_height += get_png_height(ihdr_in);
        // printf("%0x\n", ntohl(readed_file[7]));
        // printf("%x\n", ((ntohl(readed_file[7])<<8)));
        ///////////////////////////////////////
        // chunk = extract_actual_chunk(readed_file, chunk_type = "idat");
        simple_PNG_p png = get_image_thread(strip_array[k]);
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
    total_height = 300;
    printf("TOTAL HEIGHT: %d\n", total_height);
    ///////////Deflate Operation//////////////////
    k = mem_def(def_result_ptr, &def_len, &dest_total, dest_len_total, Z_DEFAULT_COMPRESSION);
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
    // FILE *files;
    // files = fopen(argv[1], "rb");
    // if(files == NULL){
    //     printf("failed to open file");
    //     return 3;
    // }
    // fread(readed_file, sizeof(U32), 400000, files);
    png = get_image_thread(strip_array[0]);

    
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
    




    return 0;
}