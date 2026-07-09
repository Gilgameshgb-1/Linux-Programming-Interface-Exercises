#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"

extern int optind;

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {

    int opt;
    int append_flag = 0;

    const char *optstring = {"a"};

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 'a':
                append_flag = 1;
                break;
            default:
                fprintf(stderr, "Error: Invalid command line argument.\n");
                exit(EXIT_FAILURE);                  
        }
    }

    //grab filename
    char *filename = NULL;
    if (optind < argc) {
        filename = argv[optind];
    }else{
        fprintf(stderr, "Error: No filename provided.\n");
        exit(EXIT_FAILURE);
    }

    int fd;
    
    if(append_flag == 1)
    {
        fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    }else
    {
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }

    if(fd == -1) {
        fprintf(stderr, "Error: Could not open file %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_read;
    ssize_t bytes_written;
    char buffer[BUFFER_SIZE];
    while((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) != 0)
    {
        if(bytes_read == -1) {
            fprintf(stderr, "Error: Could not read from stdin.\n");
            exit(EXIT_FAILURE);
        }

        if((bytes_written = write(STDOUT_FILENO, buffer, bytes_read)) != bytes_read) {
            fprintf(stderr, "Error: Could not write to stdout.\n");
            exit(EXIT_FAILURE);
        }

        if((bytes_written = write(fd, buffer, bytes_read)) != bytes_read) {
            fprintf(stderr, "Error: Could not write to stdout.\n");
            exit(EXIT_FAILURE);
        }
    }

    close(fd);
    exit(EXIT_SUCCESS);
}