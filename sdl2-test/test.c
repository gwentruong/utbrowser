// Using SDL and standard IO
#include <SDL.h>
#include <cairo.h>
#include <pango/pangocairo.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 1

// Screen dimension constants
const int SCREEN_WIDTH = 760;
const int SCREEN_HEIGHT = 580;
char *story = "Alice was beginning to get very tired of sitting by her sister on the"
              "bank, and of having nothing to do: once or twice she had peeped into the"
              "book her sister was reading, but it had no pictures or conversations in"
              "it, 'and what is the use of a book,' thought Alice 'without pictures or"
              "conversations?'\n\n"
              ""
              "So she was considering in her own mind (as well as she could, for the"
              "hot day made her feel very sleepy and stupid), whether the pleasure"
              "of making a daisy-chain would be worth the trouble of getting up and"
              "picking the daisies, when suddenly a White Rabbit with pink eyes ran"
              "close by her.\n\n"
              ""
              "There was nothing so VERY remarkable in that; nor did Alice think it so"
              "VERY much out of the way to hear the Rabbit say to itself, 'Oh dear!"
              "Oh dear! I shall be late!' (when she thought it over afterwards, it"
              "occurred to her that she ought to have wondered at this, but at the time"
              "it all seemed quite natural); but when the Rabbit actually TOOK A WATCH"
              "OUT OF ITS WAISTCOAT-POCKET, and looked at it, and then hurried on,"
              "Alice started to her feet, for it flashed across her mind that she had"
              "never before seen a rabbit with either a waistcoat-pocket, or a watch"
              "to take out of it, and burning with curiosity, she ran across the field"
              "after it, and fortunately was just in time to see it pop down a large"
              "rabbit-hole under the hedge.\n\n"
              ""
              "In another moment down went Alice after it, never once considering how"
              "in the world she was to get out again.\n\n"
              ""
              "The rabbit-hole went straight on like a tunnel for some way, and then"
              "dipped suddenly down, so suddenly that Alice had not a moment to think"
              "about stopping herself before she found herself falling down a very deep"
              "well.";

int main(int argc, char **argv)
{
    SDL_Window      *window    = NULL;
    SDL_Surface     *sdlsurf   = NULL;
    cairo_surface_t *cairosurf = NULL;
    cairo_t         *cr        = NULL;
    PangoLayout     *layout;
    PangoFontDescription *font_description;
    PangoLayoutLine *line;
    char buf[1024];

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

            // cairo_select_font_face(cr, "serif",
            //                        CAIRO_FONT_SLANT_NORMAL,
            //                        CAIRO_FONT_WEIGHT_BOLD);
            // cairo_set_font_size(cr, 32.0);
            // cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);
            // cairo_move_to(cr, 10.0, 50.0);
            // cairo_show_text(cr, "Hello!... Yifan");

            font_description = pango_font_description_new();
            pango_font_description_set_family(font_description, "serif");
            pango_font_description_set_weight(font_description, PANGO_WEIGHT_BOLD);
            pango_font_description_set_absolute_size(font_description, 16 * PANGO_SCALE);

            layout = pango_cairo_create_layout(cr);
            pango_layout_set_width(layout, (sdlsurf->w - 40) * PANGO_SCALE);
            pango_layout_set_height(layout, (sdlsurf->h / 2) * PANGO_SCALE);
            pango_layout_set_spacing(layout, 10 * PANGO_SCALE);
            pango_layout_set_justify(layout, TRUE);
            pango_layout_set_font_description(layout, font_description);
            pango_layout_set_text(layout, story, -1);

            int line_count = pango_layout_get_line_count(layout);
            printf("Line %d\n", line_count);
            // printf("Basline %d\n", pango_layout_get_baseline(layout));

#if DEBUG
            for (int i = 0; i < line_count; i++)
            {
                line = pango_layout_get_line(layout, i);
                // printf("Index of the end of last line %d\n", line->start_index + line->length);
                strncpy(buf, story + line->start_index, line->length);
                printf("%s\n", buf);
                memset(buf, 0, sizeof(buf));
            }
#endif

            cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);
            cairo_move_to(cr, 20.0, 40.0);
            pango_cairo_show_layout(cr, layout);

            // g_object_unref(layout);
            // pango_font_description_free(font_description);
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

    return 0;
}
