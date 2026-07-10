#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <errno.h>

ssize_t my_writev(int fd, const struct iovec *iov, int iovcnt)
{
    size_t total = 0;
    for (int i = 0; i < iovcnt; i++)
        total += iov[i].iov_len;

    if (total == 0)
        return 0;

    char *buf = malloc(total);
    if (buf == NULL)
        return -1;

    size_t offset = 0;
    for (int i = 0; i < iovcnt; i++) {
        memcpy(buf + offset, iov[i].iov_base, iov[i].iov_len);
        offset += iov[i].iov_len;
    }

    ssize_t written = write(fd, buf, total);

    free(buf);
    return written;
}

ssize_t my_readv(int fd, const struct iovec *iov, int iovcnt)
{
    size_t total = 0;
    for (int i = 0; i < iovcnt; i++)
        total += iov[i].iov_len;

    if (total == 0)
        return 0;

    char *buf = malloc(total);
    if (buf == NULL)
        return -1;

    ssize_t nread = read(fd, buf, total);
    if (nread == -1) {
        free(buf);
        return -1;
    }

    size_t remaining = (size_t) nread;
    size_t offset = 0;
    for (int i = 0; i < iovcnt && remaining > 0; i++) {
        size_t chunk = iov[i].iov_len < remaining ? iov[i].iov_len : remaining;
        memcpy(iov[i].iov_base, buf + offset, chunk);
        offset += chunk;
        remaining -= chunk;
    }

    free(buf);
    return nread;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open (write test)");
        exit(EXIT_FAILURE);
    }

    char part1[] = "Hello, ";
    char part2[] = "scattered ";
    char part3[] = "world!\n";

    struct iovec wiov[3];
    wiov[0].iov_base = part1; wiov[0].iov_len = strlen(part1);
    wiov[1].iov_base = part2; wiov[1].iov_len = strlen(part2);
    wiov[2].iov_base = part3; wiov[2].iov_len = strlen(part3);

    ssize_t written = my_writev(fd, wiov, 3);
    if (written == -1) {
        perror("my_writev");
        exit(EXIT_FAILURE);
    }
    printf("my_writev() wrote %zd bytes\n", written);
    close(fd);

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open (read test)");
        exit(EXIT_FAILURE);
    }

    char buf1[7];  memset(buf1, 0, sizeof(buf1));
    char buf2[10]; memset(buf2, 0, sizeof(buf2));
    char buf3[7];  memset(buf3, 0, sizeof(buf3));

    struct iovec riov[3];
    riov[0].iov_base = buf1; riov[0].iov_len = sizeof(buf1) - 1;
    riov[1].iov_base = buf2; riov[1].iov_len = sizeof(buf2) - 1;
    riov[2].iov_base = buf3; riov[2].iov_len = sizeof(buf3) - 1;

    ssize_t got = my_readv(fd, riov, 3);
    if (got == -1) {
        perror("my_readv");
        exit(EXIT_FAILURE);
    }
    printf("my_readv() read %zd bytes\n", got);
    printf("buf1: [%s]\n", buf1);
    printf("buf2: [%s]\n", buf2);
    printf("buf3: [%s]\n", buf3);

    close(fd);
    exit(EXIT_SUCCESS);
}