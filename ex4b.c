/*

File: ex4b.c
Going Over Directory and Deleting Files
=====================================================================
Written by: Tali Kalev, ID:208629691, Login: talikal
		and	Noga Levy, ID:315260927, Login: levyno

This program receives a directory in the argument vector. The program
then proceeds to scan the contents of the directory. If it encounters
an executable file, it will as the user if they would like to delete the file.
If so the program deletes the file from the current directory. If the
program encounters a subdirectory, it will then recursively scan the contents
of the subdirectory until it reaches the end of the tree.

Compile: gcc -Wall ex4b.c -o ex4b

Run:    ./ex4b <directory_name>

Input: yes or no to whether the user would like to delete the file
    ex: "Would you like to delete file: <file_name> Y/N?" y
        (following inputs are allowed : Y/y/N/n)

Output: number of bytes deleted from disk
    ex: Number of bytes deleted from disk: 56666

 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>


#define SIZE 2
#define MAX_STR_LEN 100

off_t readDirectoryContents(char* file_name);

int main(int argc, char **argv)
{
    off_t bytes_deleted;

    if(argc != SIZE){
        fprintf(stderr, "Incorrect number of arguments\n");
        exit(EXIT_FAILURE);
    }

    bytes_deleted = readDirectoryContents(argv[1]);
    printf("Number of bytes deleted from disk: %ld\n", bytes_deleted);

    return EXIT_SUCCESS;
}

off_t readDirectoryContents(char* file_name)
{
    struct stat the_stat;
    struct dirent* dentry;
    DIR* curr_dir;
    off_t size = 0; //num of bytes deleted

    if(stat(file_name, &the_stat) == -1){
        fprintf(stderr,"stat()1 failed for file name: %s\n", file_name);
        exit(EXIT_FAILURE);
    }

    if(!S_ISDIR(the_stat.st_mode))
        return 0;

    if((curr_dir = opendir(file_name)) == NULL){
        fprintf(stderr,"failed to open directory %s\n", file_name);
        exit(EXIT_FAILURE);
    }

    while((dentry = readdir(curr_dir)) != NULL)
    {
        char answer;
        char curr_f[MAX_STR_LEN];
        strcpy(curr_f, file_name);
        strcat(curr_f, "/");
        strcat(curr_f, dentry->d_name);

        if(stat(curr_f, &the_stat) == -1){
            fprintf(stderr,"stat()2 failed for file %s\n", curr_f);
            exit(EXIT_FAILURE);
        }

        if(curr_f[strlen(curr_f) - 1] == '.')
            continue;

		//if executable file:
        if(S_ISREG(the_stat.st_mode) && the_stat.st_mode & S_IXUSR)
        {
            printf("Would you like to delete file: %s Y/N? ", dentry->d_name);
            answer = getchar();
            getchar();
            if(answer == 'Y' || answer == 'y')
            {
                size += the_stat.st_size; //getting size of file to be deleted
                if (unlink(curr_f) != 0){
					fprintf(stderr,"failed to delete file %s\n", dentry->d_name);
				}
            }
        }
        else if(S_ISDIR(the_stat.st_mode))//else if directory
        {
            size += readDirectoryContents(curr_f);
        }
    }

    if(closedir(curr_dir)==-1){
        fprintf(stderr,"failed to close directory\n");
        exit(EXIT_FAILURE);
    }
    return size;
}
