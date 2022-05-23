#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "png_util/lab_png.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

///////Variable Declaration//////
// int k;
// int width;
// int height;
// U32 readed_file[100];
// U8 readed_file_e[200];
// char chunk_type;
// // int tracker = 0;
// FILE *files;
// int *png_bytes = -1991225785;
// struct data_IHDR ihdr_in;
// struct chunk chunk;
// U32 crc_calculated;
///////////////////////////////

// int is_png(U8 *buf, size_t n);
// int f_name(char *path);
// char f_type(char *file_path);

char *f_type(char *file_path) {


    int i;
    char *ptr;
    char *res;
    res = file_path;
    
    struct stat buf;

    if (lstat(file_path, &buf) < 0) {
        perror("lstat error");
    }   

    if      (S_ISREG(buf.st_mode))  ptr = "regular";
    else if (S_ISDIR(buf.st_mode))  ptr = "directory";
    else if (S_ISCHR(buf.st_mode))  ptr = "character special";
    else if (S_ISBLK(buf.st_mode))  ptr = "block special";
    else if (S_ISFIFO(buf.st_mode)) ptr = "fifo";
#ifdef S_ISLNK
    else if (S_ISLNK(buf.st_mode))  ptr = "symbolic link";
#endif
#ifdef S_ISSOCK
    else if (S_ISSOCK(buf.st_mode)) ptr = "socket";
#endif
    else                            ptr = "**unknown mode**";
    // printf("%s: %s\n",file_path, ptr);
    // char type;
    // type = ptr;
    return ptr;
}

void *f_name(char *argv, char *file_path, U32 *output[]) {
    DIR *p_dir;
    struct dirent *p_dirent;
    int index = 0;
    char *output_path[100] = {0};
    char str[64];
    p_dir = opendir(argv);
    // if (argc == 1) {
    //     fprintf(stderr, "Usage: %s <directory name>\n", argv[0]);
    //     exit(1);
    // }

    // if ((p_dir = opendir(argv[1])) == NULL) {
    //     sprintf(str, "opendir(%s)", argv[1]);
    //     perror(str);
    //     exit(2);
    // }

    while ((p_dirent = readdir(p_dir)) != NULL) {
        char *str_path = p_dirent->d_name;  /* relative path name! */
        char *full_strpath[1024];
        strcpy(full_strpath, file_path);
        strcat(full_strpath, str_path);
        // printf("%s\n", f_type(full_strpath));

        if (str_path == NULL) {
            fprintf(stderr,"Null pointer found!"); 
            exit(3);
        }
        else if(!strcmp(str_path, ".")|| !strcmp(str_path, "..")){
            printf("%s: no need for this dir\n", full_strpath);
        }
        else if(f_type(full_strpath) == "directory"){
            char *next_full[1024];
            strcpy(next_full, full_strpath);
            strcat(next_full, "/");
            // printf("%s, %s\n", next_full, full_strpath);
            f_name(full_strpath, next_full, output);

        }
        else {
            if(f_type(full_strpath) == "regular"){
                U32 readed_file[100];
                char chunk_type;
                // int tracker = 0;
                FILE *files;
                int *png_bytes = -1991225785;
                char stack_fullstr[100][1024];
                strcpy(stack_fullstr[index], full_strpath);
                ///////////////////////////////

                ///////file operations//////////////
                files = fopen(full_strpath, "rb");
                if(files == NULL){
                    printf("%s: failed to open file", full_strpath);
                }
                fread(readed_file, sizeof(U32), 100, files);
                if(is_png(readed_file) == 1){
                    printf("%s\n", full_strpath);
                    int i = 0;
                    for(i; i<100; ++i){
                        if(output[i] == "nmsl"){
                            // printf("hey\n");
                            // if(i>0){
                            //     printf("output[%d]: %s\n", i-1, output[i-1]);
                            // }
                            output[i] = &stack_fullstr[index];
                            output[i+1] = "nmsl";
                            break;
                        }
                    }

                }
            }
            ++index;
        }
    }

    if ( closedir(p_dir) != 0 ) {
        perror("closedir");
        exit(3);
    }

    return;
}



int main(int argc, char *argv[]) {

    if (argc == 1) {
        fprintf(stderr, "Usage: %s <directory name>\n", argv[0]);
        exit(1);
    }
    char *open_dir = argv[1];
    char *file_path[1024];
    U32 *output_path[100] = {"nmsl"};
    strcpy(file_path, "./images/");
    
    f_name(open_dir, file_path, output_path);
    printf("xxxxxxxx This is the print of the list: xxxxxxxxx\n");
    for(int i=0; i<100; ++i){
        if(output_path[i]=="nmsl"){
            break;
        }
        printf("%s\n", output_path[i]);
    }

    return 0;
}

// int is_png(U8 *buf, size_t n){
//     U8 ID[PNG_SIG_SIZE] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
//     return memcmp(ID, buf, n);
// };
