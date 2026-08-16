#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern char **environ;

char **synthethic_environ;

static char *test_entries[] = {
    "FOO=1",
    "FOO=2",
    "FOO=3",
    "HOME=/home/faris",
    "HOMEBREW=/opt/brew",
    "BAR=x"};

void init_synthetic_environ(void)
{
    size_t n = sizeof(test_entries) / sizeof(test_entries[0]);

    synthethic_environ = malloc((n + 1) * sizeof(char *));
    if (synthethic_environ == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < n; i++)
    {
        synthethic_environ[i] = strdup(test_entries[i]);
        if (synthethic_environ[i] == NULL)
        {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
    }
    synthethic_environ[n] = NULL;

    environ = synthethic_environ;
}

void print_out_environ()
{
    int i = 0;
    while (environ[i] != NULL)
    {
        printf("[%d] %s \n", i, environ[i]);
        i++;
    }
}

int my_unsetenv(const char *name)
{
    if (name == NULL || name[0] == '\0' || (strchr(name, '=') != NULL))
    {
        errno = EINVAL;
        return -1;
    }
    size_t len = strlen(name);
    int i = 0;
    while (environ[i] != NULL)
    {
        if (strncmp(environ[i], name, len) == 0 && environ[i][len] == '=')
        {
            for (int j = i; environ[j] != NULL; j++)
            {
                environ[j] = environ[j + 1];
            }
        }
        else
        {
            i++;
        }
    }
    return 0;
}

int my_setenv(const char *name, const char *value, int overwrite)
{
    if (name == NULL || name[0] == '\0' || (strchr(name, '=') != NULL) || value == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    if (getenv(name) != NULL && overwrite == 0)
    {
        return 0;
    }
    if (my_unsetenv(name) == -1)
    {
        return -1;
    }
    size_t size = strlen(name) + strlen(value) + 2;
    char *buf = malloc(size);
    if (buf == NULL)
    {
        return -1;
    }
    snprintf(buf, size, "%s=%s", name, value);
    if (putenv(buf) != 0)
    {
        free(buf);
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    init_synthetic_environ();
    printf("\nInitial environment\n");
    print_out_environ();

    printf("\nSet environment HOME into /home/changedhomeenv\n");
    my_setenv("HOME", "/home/changedhomeenv", 1);
    print_out_environ();

    printf("\nNow we set environment var HOME without overwrite zero flag, nothing should happen\n");
    my_setenv("HOME", "/should/not/appear", 0);
    print_out_environ();

    printf("\nWe attempt un-setting three same named env vars\n");
    my_unsetenv("FOO");
    print_out_environ();

    printf("\nAttempt to cause errors, nulls = signs empty str");
    errno = 0;
    printf("my_unsetenv(\"\")    rc = %d, errno = %d\n", my_unsetenv(""), errno);
    errno = 0;
    printf("my_unsetenv(\"A=B\") rc = %d, errno = %d\n", my_unsetenv("A=B"), errno);
    errno = 0;
    printf("my_setenv(NULL...) rc = %d, errno = %d\n", my_setenv(NULL, "x", 1), errno);

    printf("\nFinal output\n");
    print_out_environ();
    exit(EXIT_SUCCESS);
}