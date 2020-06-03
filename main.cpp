/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <stdio.h>
#include "gui_library.h"
#include <unistd.h>
//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int main( int argc, char* args[] )
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        TTF_Init();
        SDL_Window *window = SDL_CreateWindow("SDL2 Window",
                                              SDL_WINDOWPOS_CENTERED,
                                              SDL_WINDOWPOS_CENTERED,
                                              600, 600,
                                              0);
        SDL_Renderer* renderer = SDL_CreateRenderer(window,0,SDL_RENDERER_ACCELERATED);
        menuPage mainPage = menuPage();
        mainPage.setTitle("Engineering Sample");
        HorizontalGraph graph1 = HorizontalGraph("OIL","PSI",0,75,0,600-100);
        int count = 0;
        mainPage.addWidget(&graph1);
        if (1==1) {
            SDL_bool done = SDL_FALSE;
            Renderer RENDERER(window,renderer);
            RENDERER.addPage(&mainPage);
            while (!done) {
                SDL_Event event;

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);
                //Draw
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                RENDERER.render();
                //SDL_RenderDrawLine(renderer,0,0,200,200);
                count++;
                graph1.setValue(count);
                ///////////////Draw Picture
                for(int index = 10000; index < 11199; index++) {
                    std::string filepath = "/home/dylan/Documents/Gauge1_AME/Comp " + std::to_string(index) + ".bmp";

                    SDL_Surface *image = SDL_LoadBMP(filepath.c_str());
                    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
                    SDL_Rect r;
                    r.x = 0;
                    r.y = 0;
                    r.w = 400;
                    r.h = 400;
                    SDL_RenderCopy(renderer, texture, NULL, &r);
                    SDL_DestroyTexture(texture);
                    SDL_FreeSurface(image);
                    ////////////////////////////
                    SDL_RenderPresent(renderer);
                }
                usleep(100000);
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        done = SDL_TRUE;
                    }
                }
            }
        }

        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
    }
    SDL_Quit();
    return 0;
}