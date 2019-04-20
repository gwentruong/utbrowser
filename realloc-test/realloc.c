#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    unsigned char *buf = malloc(4);
    buf[0] = 0xDE;
    buf[1] = 0xAD;
    buf[2] = 0xBE;
    buf[3] = 0xEF;

    unsigned char *new_buf = realloc(buf, 8);

    printf("buf %p\tnew_buf %p\n", buf, new_buf);

    for (int i = 0; i < 8; i++)
        printf("%x", *(new_buf + i));

    printf("\n");
    free(new_buf);
    return 0;
}
