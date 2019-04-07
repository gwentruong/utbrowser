// Using SDL and standard IO
#include <SDL.h>
#include <cairo.h>
#include <pango/pangocairo.h>
#include <stdio.h>


// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char **argv)
{
    SDL_Window      *window    = NULL;
    SDL_Surface     *sdlsurf   = NULL;
    cairo_surface_t *cairosurf = NULL;
    cairo_t         *cr        = NULL;
    PangoLayout     *layout;
    PangoFontDescription *font_description;

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
            pango_font_description_set_absolute_size(font_description, 32 * PANGO_SCALE);

            layout = pango_cairo_create_layout(cr);
            pango_layout_set_font_description(layout, font_description);
            pango_layout_set_text(layout, "Hello, world", -1);

            cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);
            cairo_move_to(cr, 10.0, 40.0);
            pango_cairo_show_layout(cr, layout);

            g_object_unref(layout);
            pango_font_description_free(font_description);

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
                        quit = 1;

                    SDL_Delay(10);
                }
            }
        }
    }

    cairo_destroy(cr);
    cairo_surface_destroy(cairosurf);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
