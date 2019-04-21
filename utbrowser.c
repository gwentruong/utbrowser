#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gumbo.h"
#include "utbrowser.h"

void remove_string (char text[], int index, int rm_length)
{
    int i;

    for ( i = 0; i < index; ++i )
        if ( text[i] == '\0' )
            return;

    for ( ; i < index + rm_length; ++i )
        if ( text[i] == '\0' ) {
            text[index] = '\0';
            return;
        }

    do {
        text[i - rm_length] = text[i];
    } while ( text[i++] != '\0' );
}

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
