#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <libgen.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <SDL.h>
#include <cairo.h>
#include <pango/pangocairo.h>

#include "gumbo.h"

#define PORT "80"
#define MAX_LEN 16384

const int SCREEN_WIDTH = 760;
const int SCREEN_HEIGHT = 580;

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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        perror("Usage: ./http <hostname>");
        return -1;
    }

    struct addrinfo hints, *servinfo, *p;
    int   sockfd;
    int   status;
    char  ipstr[INET6_ADDRSTRLEN];
    char *request = "GET / HTTP/1.1\r\nHost: www.savewalterwhite.com\r\n\r\n";
    int   n;
    void *addr;
    char buf[MAX_LEN] = { '\0' };     // Big buffer to append content of html

    SDL_Window      *window    = NULL;
    SDL_Surface     *sdlsurf   = NULL;
    cairo_surface_t *cairosurf = NULL;
    cairo_t         *cr        = NULL;
    PangoLayout     *layout;
    PangoFontDescription *font_description;
    PangoLayoutLine *line;

    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(argv[1], NULL, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }
    printf("IP addresses for %s:\n\n", argv[1]);

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        addr  = &(ipv4->sin_addr);

        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf("\t%s\n", ipstr);        // Got IP address
        break;
    }

    freeaddrinfo(servinfo);

    if ((status = getaddrinfo(ipstr, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    // Loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    if (send(sockfd, request, strlen(request), 0) == -1)
    {
        perror("Can't send request to server");
        return -1;
    }
    printf("Request to server sent!\n");

    char *ptr = buf;
    char *head = buf;
    char length_str[16] = { '\0' };
    int j = 0;
    int i;
    int got_num = 0;

    n = recv(sockfd, ptr, MAX_LEN, 0);
    ptr += n;

    char *str_str = strstr(buf, "Content-Length");
    for (i = 0; i < strlen(str_str); i++)
    {
        if (got_num == 0)
        {
            if (str_str[i] == ' ')
                got_num = 1;
        }
        else
        {
            if (str_str[i] != '\r' || str_str[i] != '\n')
            {
                length_str[j] = str_str[i];
                j++;
            }
            else
                break;
        }
    }
    int html_size = atoi(length_str);
    printf("Size %d\n", html_size);

    while ((n = recv(sockfd, ptr, MAX_LEN, 0)) > 0)
    {
        // printf("Current length %ld\n", strlen(head));

        if (strlen(head) >= html_size)
        {
            close(sockfd);
            break;
        }
        ptr += n;
    }

    // Remove extra text from header
    char *extra = strstr(buf, "<!DOCTYPE");
    remove_string(buf, 0, extra - buf - 1);

    // printf("\n\n%s\n", buf);

    GumboOutput *output = gumbo_parse(buf);
    const char *story = cleantext(output->root);
    printf("%s\n", story);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    else
    {
        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                  SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        else
        {
            sdlsurf   = SDL_GetWindowSurface(window);
            cairosurf = cairo_image_surface_create_for_data(sdlsurf->pixels,
                                                            CAIRO_FORMAT_RGB24,
                                                            sdlsurf->w,
                                                            sdlsurf->h,
                                                            sdlsurf->pitch);
            cr        = cairo_create(cairosurf);

            cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
            cairo_paint(cr);

            font_description = pango_font_description_new();
            pango_font_description_set_family(font_description, "serif");
            pango_font_description_set_weight(font_description, PANGO_WEIGHT_BOLD);
            pango_font_description_set_absolute_size(font_description, 16 * PANGO_SCALE);

            layout = pango_cairo_create_layout(cr);
            pango_layout_set_width(layout, (sdlsurf->w - 40) * PANGO_SCALE);
            pango_layout_set_height(layout, (sdlsurf->h - 70) * PANGO_SCALE);
            pango_layout_set_spacing(layout, 10 * PANGO_SCALE);
            pango_layout_set_justify(layout, TRUE);
            pango_layout_set_font_description(layout, font_description);
            pango_layout_set_text(layout, story, -1);

            int line_count = pango_layout_get_line_count(layout);
            printf("Line %d\n", line_count);
            line = pango_layout_get_line(layout, line_count - 1);

            cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
            cairo_move_to(cr, 20.0, 40.0);
            pango_cairo_show_layout(cr, layout);

            printf("width %d, height %d\n", pango_layout_get_width(layout),
                                            pango_layout_get_height(layout));
            SDL_UpdateWindowSurface(window);

            SDL_Event e;
            int quit = 0;
            while (!quit)
            {
                while (SDL_PollEvent(&e))
                {
                    if (e.type == SDL_QUIT)
                        quit = 1;

                    if (e.type == SDL_KEYDOWN)
                        quit = 1;

                    if (e.type == SDL_MOUSEBUTTONDOWN)
                    {
                        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
                        cairo_paint(cr);

                        pango_layout_set_text(layout,
                            story + line->start_index + line->length, -1);

                        cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);
                        cairo_move_to(cr, 20.0, 40.0);
                        pango_cairo_show_layout(cr, layout);
                        SDL_UpdateWindowSurface(window);
                    }

                    SDL_Delay(10);
                }
            }
            g_object_unref(layout);
            pango_font_description_free(font_description);
        }
    }

    cairo_destroy(cr);
    cairo_surface_destroy(cairosurf);
    SDL_DestroyWindow(window);

    SDL_Quit();

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return 0;
}
