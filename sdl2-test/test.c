// Using SDL and standard IO
#include <SDL.h>
#include <cairo.h>
#include <stdio.h>

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char **argv)
{
    SDL_Window      *window    = NULL;
    SDL_Surface     *sdlsurf   = NULL;
    cairo_surface_t *cairosurf = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    else
    {
        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        else
        {
            sdlsurf = SDL_GetWindowSurface(window);

            cairosurf = cairo_image_surface_create_for_data (
                sdlsurf->pixels,
                CAIRO_FORMAT_RGB24,
                sdlsurf->w,
                sdlsurf->h,
                sdlsurf->pitch);

            // Fill the surface white
            SDL_FillRect(sdlsurf, NULL, SDL_MapRGB(sdlsurf->format, 0xFF, 0xFF, 0xFF));

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

    cairo_surface_destroy(cairosurf);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
