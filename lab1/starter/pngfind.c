#include <stdio.h>	/* printf needs to include this header file */
#include <ctype.h>
#include <stdlib.h>
#include "png_util/lab_png.h"
//#include <sys/types.h>
#include <dirent.h>
//#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>  /* for strcat().  man strcat   */

bool PNG (char *png_check);

/**
 *@file:  ls_fname.c
 *@brief: main function that lists all command line arguments
 *@param: int argc is the number of command line arguments. 
 *        The executable itself is the first argument.
 *@param: char *argv is an array to hold each command line argument as an element.
 *        argv[0] contains the string representation of executable's name
 *        argv[1] contains the string representation of the 1st input argument
 *        ...
 *        argv[n] contains the string representation of the n'th input argument
 */

int main(int argc, char *argv[]) {

    // Get cmd-line arguments -> open directory -> get all paths in directory
    char command[100];

    strcpy( command, "cd \n ./G_202_10/lab1/starter/ls/lsfname.c"); //wrong directory??
    system( command );

    /*
    For each path...
    check if png
    //use pnginfo to get the appropriate result
    

    yes: Print relative path

    no: check if directory
        yes: Open directory
        no: exit out
    */

    for (int i=0; i < 100; i++) {
        if (command[i] == NULL){
            break;
        }
        char *png_check;
        strcpy( png_check, "./pnginfo $command[i]");
        system( png_check );
        if (PNG(png_check) == true){
            printf(command[i]);
        }
        else {
            char *file_check;
            strcpy( file_check, "cd \n ./G_202_10/lab1/starter/ls/lsftype.c");
            system( file_check );
            if (file_check == "directory"){
                
            }
        }
    }

    // Recursion

    return 0;
}

bool PNG (char *png_check) {
    char *check_not = ": Not a PNG file";
    char *ell = strstr(png_check, check_not);
    if (ell != NULL){
        return true;
    }
    else if (ell == NULL) {
        return false;
    }
    return false;
}