#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"
#include "sys/stat.h"


int main(int argc, char *argv[]) {

    int fd = open(argv[1], O_APPEND | O_RDWR);

    int newfd = fcntl(fd, F_DUPFD, 69);

    fprintf(stdout, "fd: %d\n", fd);
    fprintf(stdout, "newfd: %d\n", newfd);

    exit(EXIT_SUCCESS);
}