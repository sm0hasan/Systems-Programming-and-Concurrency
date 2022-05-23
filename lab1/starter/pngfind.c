#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "lab_png.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

///////Variable Declaration//////
int k;
int width;
int height;
U32 readed_file[100];
U8 readed_file_e[200];
char chunk_type;
// int tracker = 0;
FILE *files;
int *png_bytes = -1991225785;
struct data_IHDR ihdr_in;
struct chunk chunk;
U32 crc_calculated;
///////////////////////////////

int is_png(U8 *buf, size_t n);
int f_name(char *path);
char f_type(char *file_path);

int f_name(char *path) {
    char* original_path;
    original_path = path;
    int i = 0;
    DIR *p_dir;
    struct dirent *p_dirent;
    char str[64];
    struct stat buf;
    p_dir = opendir(path);

    while ((p_dirent = readdir(p_dir)) != NULL) {
        //printf(path);
        char *str_path = p_dirent->d_name;  /* relative path name! */
            
        char *concat = malloc(sizeof(str_path));
        char* slash = "/";
        //concat = str_path;
        strcpy(concat, str_path);
        
        //printf(concat);
        char type = f_type(str_path);
        //printf(concat);
        printf(type);
        if (type == "directory") {
            strcat(path, slash);
            strcat(path, str_path);
            printf("this is the: %s", path);
            f_name(path);
        }
        else {
            FILE* fp;
            fp = fopen(str_path, "rb");

            U8 s_id[PNG_SIG_SIZE];

            fread(s_id, PNG_SIG_SIZE, 1, fp);
            //if (is_png(s_id, PNG_SIG_SIZE) == 0){
                char *printed = malloc(sizeof(path));
                strcpy(printed, path);
                strcat(printed, slash);
                strcat(printed, concat);
                printf("%s\n", printed);
            //}
        }
    }
    if ( closedir(p_dir) != 0 ) {
        perror("closedir");
        exit(3);
    }
    return 0;
}

char f_type(char *file_path) {


    int i;
    char *ptr;
    char *res;
    //ptr = file_path;
    
    struct stat buf;

        
        if (lstat(file_path, &buf) < 0) {
            perror("lstat error");
        }   
        

        if      (S_ISREG(buf.st_mode))  return "regular";
        else if (S_ISDIR(buf.st_mode))  return "directory";
        else if (S_ISCHR(buf.st_mode))  return "character special";
        else if (S_ISBLK(buf.st_mode))  return "block special";
        else if (S_ISFIFO(buf.st_mode)) return "fifo";
#ifdef S_ISLNK
        else if (S_ISLNK(buf.st_mode))  return "symbolic link";
#endif
#ifdef S_ISSOCK
        else if (S_ISSOCK(buf.st_mode)) return "socket";
#endif
        else                            return "**unknown mode**";
    // printf("%s\n", ptr);
    // char type;
    // type = ptr;
    // return type;
}

int main(int argc, char *argv[]) {

    if (argc == 1) {
        fprintf(stderr, "Usage: %s <directory name>\n", argv[0]);
        exit(1);
    }

    f_name(argv[1]);

    return 0;
}

int is_png(U8 *buf, size_t n){
    U8 ID[PNG_SIG_SIZE] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    return memcmp(ID, buf, n);
};
