#include <stdio.h>
#include <string.h>
#include <gumbo.h>

int main(void)
{
    FILE *fp = fopen("white.html", "r");
    int size, n;

    if (fp == NULL)
    {
        perror("Cannot find file");
        return -1;
    }

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char buf[size + 1];
    memset(buf, 0, sizeof(buf));

    if ((n = fread(buf, 1, sizeof(buf), fp)) > 0)
    {
        GumboOutput *output = gumbo_parse(buf);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
    }
    return 0;
}
