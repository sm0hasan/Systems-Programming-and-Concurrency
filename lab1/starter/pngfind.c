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
char *f_name(char *path);
char f_type(char *file_path);

char *f_name(char *path) {

    int i = 0;
    DIR *p_dir;
    struct dirent *p_dirent;
    char str[64];
    char *fname_arr[100];
    struct stat buf;

    if ((p_dir = opendir(path)) == NULL) {
        sprintf(str, "opendir(%s)", path);
        perror(str);
        exit(2);
    }

    while ((p_dirent = readdir(p_dir)) != NULL) {
        printf(path);
        char *str_path = p_dirent->d_name;  /* relative path name! */

        if (str_path == NULL) {
            fprintf(stderr,"Null pointer found!"); 
            exit(3);
        } else {
            
            FILE* fp;
            fp = fopen(str_path, "rb");

            U8 s_id[PNG_SIG_SIZE];

            fread(s_id, PNG_SIG_SIZE, 1, fp);
            
            if (is_png(s_id, PNG_SIG_SIZE) == 0){
                fname_arr[i] = path;
                i++;
            }
            else if (is_png(s_id, PNG_SIG_SIZE) != 0) {
                char concat;
                char* slash = "/";
                char type = f_type(str_path);
                if (type == "directory") {
                    strcat(path, slash);
                    strcat(path, str_path);
                    f_name(path);
                }
            }
        }
    }

    if ( closedir(p_dir) != 0 ) {
        perror("closedir");
        exit(3);
    }

    return fname_arr;
}

char f_type(char *file_path) {

    int i;
    char *ptr;
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
    printf("%s\n", ptr);
    char type;
    type = ptr;
    return type;
}

int main(int argc, char *argv[]) {

    char array1[] = "Hello";
char array2[sizeof( array1 )];

strcpy( array2, array1 );

    int size = sizeof(f_name(argv[1]));
    char list[size];
    strcpy(list, f_name(argv[1]));
    //list = f_name(argv[1]);

    if (argc == 1) {
        fprintf(stderr, "Usage: %s <directory name>\n", argv[0]);
        exit(1);
    }

    for (int i = 0; i < sizeof(size); i++){
        printf(list[i]);
    }

    return 0;
}

int is_png(U8 *buf, size_t n){
    U8 ID[PNG_SIG_SIZE] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    //U8 PNG_SIGNATURE[PNG_SIG_SIZE] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    return memcmp(ID, buf, n);
    /*
    int *png_bytes = -1991225785;
    int i = ntohl(buf[0]);
    if(i == png_bytes){
        return 1;
    }else{
        return 0;
    }
    */
};