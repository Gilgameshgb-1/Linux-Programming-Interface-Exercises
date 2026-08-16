#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

jmp_buf jumpBuffer;

int returnANumber()
{
    if(setjmp(jumpBuffer) == 0)
    {
        printf("Set jump for the first time\n");
    }else
    {
        printf("Return wasn't 0\n");
    }
    return 69;
}

int main(int argc, char *argv[])
{
    returnANumber();
    printf("Making sure we overwrite the stack\n");
    longjmp(jumpBuffer, 0);
    printf("We sure did jump somewhere\n");
}