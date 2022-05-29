#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

#define SIZE 2

void convertToChar(int avg, char new_avg[]);

int main(int argc, char **argv)
{
    int fd;
    off_t file_size, i;
    char *data;
    int sum = 0, counter = 0, curr_num = 0;
    char avg[2];

    if(argc != 2){
        fprintf(stderr, "Incorrect number of arguments\n");
        exit(EXIT_FAILURE);
    }

    //Open file for r/w
    if((fd = open(argv[1], O_RDWR)) == -1){
        fprintf(stderr, "Cannot open %s for r+w\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    file_size = lseek(fd, 0, SEEK_END); //getting file size

    //mapping file
    if((data = mmap((caddr_t)0, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0))==(caddr_t)-1){
        fprintf(stderr, "mmap() for %s failed\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    close(fd);

	//Calculating average for each line and inputing in place of 00
    for(i = 0; i < file_size; i++)
    {
        if(isdigit(data[i]))
        {
            curr_num = curr_num*10 + (data[i] - '0');
        }
        else if (curr_num == 0 && counter != 0)
        {
			convertToChar(sum/counter, avg);
			counter = sum = 0;
			data[i-2] = avg[0];
			data[i-1] = avg[1];
        }
        else
		{
			counter++;
			sum += curr_num;
			curr_num = 0;
		}
    }

    exit(EXIT_SUCCESS);
}

void convertToChar(int avg, char new_avg[])
{
    int digit;

    digit = avg % 10;
    new_avg[1] = digit + '0';
    digit = avg / 10;
    new_avg[0] = digit + '0';
}
