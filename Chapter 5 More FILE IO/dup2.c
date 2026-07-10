#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"
#include "sys/stat.h"
#include <errno.h>

int my_dup2(int oldfd, int newfd) {
    if(oldfd == newfd){
        if(fcntl(oldfd, F_GETFD) >= 0){
            return newfd;
        }else{
            errno = EBADF;
            return -1;
        }
    }else{
        close(newfd);
        newfd = fcntl(oldfd, F_DUPFD, newfd);
        if(newfd == -1){
            return -1;
        }
    }
    return newfd;
}

int main(int argc, char *argv[]) {

    int fd = open(argv[1], O_APPEND | O_RDWR);
    int newfd = my_dup2(fd, 69);

    char* buffer = "SadButTrue\n";
    int writestat = write(fd, buffer, 11);

    if(writestat == -1) {
        fprintf(stderr, "Error: Could not write to file %s.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int valseekfd = lseek(fd, 0, SEEK_CUR);
    int valseeknewfd = lseek(newfd, 0, SEEK_CUR);

    fprintf(stdout, "fd offset: %d\n", valseekfd);
    fprintf(stdout, "newfd offset: %d\n", valseeknewfd);

    int flagsfd = fcntl(fd, F_GETFL);
    int flagsnewfd = fcntl(newfd, F_GETFL);

    fprintf(stdout, "fd flags: %d\n", flagsfd);
    fprintf(stdout, "newfd flags: %d\n", flagsnewfd);

    fprintf(stdout, "fd: %d\n", fd);
    fprintf(stdout, "newfd: %d\n", newfd);

    exit(EXIT_SUCCESS);
}