#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"
#include "sys/stat.h"

#define BUFFER_SIZE 4096

int areTheBytesAllZero(char* buffer, ssize_t bytes_read)
{
    for(ssize_t i = 0; i < bytes_read; i++)
    {
        if(buffer[i] != 0)
        {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {

    if(argc < 3) {
        fprintf(stderr, "Error: Arguments were not provided.\n");
        exit(EXIT_FAILURE);
    }

    char* source = argv[1];
    char* destination = argv[2];

    int fd_src = open(argv[1], O_RDONLY);

    if(fd_src == -1) {
        fprintf(stderr, "Error: Could not open file %s.\n", source);
        exit(EXIT_FAILURE);
    }

    //trialing 0s
    struct stat src_stat;
    if (fstat(fd_src, &src_stat) == -1) {
        fprintf(stderr, "Error: Could not stat file %s.\n", source);
        exit(EXIT_FAILURE);
    }
    off_t src_size = src_stat.st_size;

    int fd_dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if(fd_dest == -1) {
        fprintf(stderr, "Error: Could not open file %s.\n", destination);
        exit(EXIT_FAILURE);
    }
    
    ssize_t bytes_read;
    ssize_t bytes_written;
    char buffer[BUFFER_SIZE];

    while((bytes_read = read(fd_src, buffer, BUFFER_SIZE)) != 0)
    {
        if(bytes_read == -1) {
            fprintf(stderr, "Error: Could not read from file.\n");
            exit(EXIT_FAILURE);
        }

        if(areTheBytesAllZero(buffer, bytes_read) == 1){
            int lseekstat = lseek(fd_dest, bytes_read, SEEK_CUR);

            if(lseekstat == -1) {
                fprintf(stderr, "Error: Could not seek in file.\n");
                exit(EXIT_FAILURE);
            }
        }else{
            bytes_written = write(fd_dest, buffer, bytes_read);
            
            if(bytes_written == -1) {
                fprintf(stderr, "Error: Could not write to file.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (ftruncate(fd_dest, src_size) == -1) {
        fprintf(stderr, "Error: Could not set final file size.\n");
        exit(EXIT_FAILURE);
    }

    close(fd_src);
    close(fd_dest);

    exit(EXIT_SUCCESS);
}