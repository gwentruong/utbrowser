#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gumbo.h"

const char *cleantext(GumboNode* node)
{
    if (node->type == GUMBO_NODE_TEXT)
    {
        return node->v.text.text;
    }
    else if (node->type == GUMBO_NODE_ELEMENT &&
             node->v.element.tag != GUMBO_TAG_SCRIPT &&
             node->v.element.tag != GUMBO_TAG_STYLE)
    {
        char *contents = calloc(8192, 1);
        GumboVector* children = &node->v.element.children;

        for (int i = 0; i < children->length; i++)
        {
            const char *text = cleantext((GumboNode*) children->data[i]);
            if (i != 0 && text != NULL)
                strcat(contents, " ");

            strcat(contents, text);
        }
        return contents;
    }
    else
        return "";
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        perror("Usage: clean_text <html filenname>");
        return -1;
    }

    FILE *fp = fopen(argv[1], "r");
    int size;

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char *content = malloc(size + 1);
    memset(content, 0, size + 1);

    fread(content, size, 1, fp);

    GumboOutput *output = gumbo_parse(content);
    printf("%s\n", cleantext(output->root));

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    fclose(fp);
    free(content);
    return 0;
}
