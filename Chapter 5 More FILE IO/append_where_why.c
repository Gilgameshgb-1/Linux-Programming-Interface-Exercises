#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"
#include "sys/stat.h"


int main(int argc, char *argv[]) {

    int fd_src = open(argv[1], O_APPEND | O_RDWR);
    //The deal is the O_APPEND flag will set the seek pointer to EOF
    //always, regardless of what we do with lseek after
    //Our text always ends up at the end of the file (atomicity of O_APPEND)

    if(fd_src == -1) {
        fprintf(stderr, "Error: Could not open file %s.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int lseekstat = lseek(fd_src, 0, SEEK_SET);

    if(lseekstat == -1) {
        fprintf(stderr, "Error: Could not seek to beginning of file %s.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    char* buffer = "SadButTrue\n";
    int writestat = write(fd_src, buffer, 11);

    if(writestat == -1) {
        fprintf(stderr, "Error: Could not write to file %s.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    close(fd_src);

    exit(EXIT_SUCCESS);
}