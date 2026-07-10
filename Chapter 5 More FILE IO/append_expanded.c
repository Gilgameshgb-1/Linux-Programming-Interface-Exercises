#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"
#include "sys/stat.h"

void err(int errCode){
    if(errCode == -1) {
        fprintf(stderr, "Error: Could not open file.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {

    if(argc < 2){
        fprintf(stderr, "Error: Not enough arguments were provided. Atleast 2 must be provided\n");
        exit(EXIT_FAILURE);
    }

    int fd; 
    char byte_to_append = 'S';
    
    int omitted_append = 0;
    if(argc == 4)
    {
        fd = open(argv[1], O_RDWR | O_CREAT, 0644);
        err(fd);
        omitted_append = 1;
    }else
    {
        fd = open(argv[1], O_APPEND | O_RDWR | O_CREAT, 0644);  
        err(fd);
    }

    for(int i = 0; i < atoi(argv[2]); i++){
        if(omitted_append == 1)
        {
            lseek(fd, 0, SEEK_END);
            write(fd, &byte_to_append, 1);
        }else{
            write(fd, &byte_to_append, 1);
        }
    }
    
    //-rw-r--r-- 1 faris faris 2000000 Jul 10 21:36 f1
    //-rw-r--r-- 1 faris faris 1029053 Jul 10 21:36 f2

    //From executing the example, the first call was atomic and we had a full write of bytes
    //Second byte was not but rather lseek + lwrite which are separate kernel operations
    //each process does its open call and has its own file descriptor
    //even though they are pointing to the same file 
    //making everything randomised by what the kernel decides to do first

    close(fd);
    exit(EXIT_SUCCESS);
}