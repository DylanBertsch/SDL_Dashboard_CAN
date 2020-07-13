/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <stdio.h>
#include "gui_library.h"
#include <unistd.h>
#include "dataCommunication_mega2560.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
SDL_Window * window = NULL;
SDL_Renderer* renderer = NULL;

void drawCoolant(int coolantValue)
{
    int index = 1000+(coolantValue)+1;
    std::string filepath = "/home/dylan/Documents/airStyle_AME/Comp " + std::to_string(index) + ".bmp";
    SDL_Surface *image = SDL_LoadBMP(filepath.c_str());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_Rect r;
    r.x = 600;
    r.y = 490;
    r.w = 300;
    r.h = 300;
    SDL_RenderCopy(renderer, texture, NULL, &r);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);
    ////////////////////////////
    //SDL_RenderDrawRect(renderer,&r);
}

void drawRPM(int RPMValue)
{
    int index = 10000+(RPMValue);
    std::string filepath = "/home/dylan/Documents/Gauge1_AME/Comp " + std::to_string(index) + ".bmp";
    SDL_Surface *image = SDL_LoadBMP(filepath.c_str());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_Rect r;
    r.x = 150;
    r.y = 0;
    r.w = 500;
    r.h = 500;
    SDL_RenderCopy(renderer, texture, NULL, &r);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);
    //SDL_RenderDrawRect(renderer,&r);
    ////////////////////////////
}

int main( int argc, char* args[] )
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        TTF_Init();
        window = SDL_CreateWindow("SDL2 Window",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  850, 800,
                                  0);
        renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
        int count = 0;
        Renderer RENDERER = Renderer(window,renderer);
        menuPage mainPage = menuPage();
        menuPage testPage = menuPage();
        VerticalGraph oilGraph = VerticalGraph(0, 550, 75, 25,"OIL", "PSI");
        BitmapWidget RPM_Widget = BitmapWidget(150,0,500,500,"/home/dylan/Documents/Gauge1_AME/Comp ",10000,15000);
        //mainPage.addWidget(&oilGraph);
        //mainPage.addWidget(&RPM_Widget);
        pagePicker Drawer1 = pagePicker(0,25,600,500);
        pagePicker::iconStruct icon1;
        icon1.iconPath = "/home/dylan/Desktop/icon1.bmp";
        icon1.onClickPagePointer = (menuPage*)&testPage;
        Drawer1.addItem(icon1);
        icon1.iconPath = "/home/dylan/Desktop/icon2.bmp";
        Drawer1.addItem(icon1);
        icon1.iconPath = "/home/dylan/Desktop/icon3.bmp";
        Drawer1.addItem(icon1);
        icon1.iconPath = "/home/dylan/Desktop/icon4.bmp";
        Drawer1.addItem(icon1);
        mainPage.addWidget(&Drawer1);
        RENDERER.addPage(&mainPage);
        testPage.addWidget(&oilGraph);
        oilGraph.onClick(&RENDERER);

            while (true) {
                SDL_Event event;
                const Uint8* keyboard_state_array = SDL_GetKeyboardState(NULL);
                SDL_WaitEventTimeout(&event,1);
                if(keyboard_state_array[SDL_SCANCODE_DOWN])
                {
                    Drawer1.incrementSelectedIcon();
                    //testConsole.incrementSelectedIndex();
                }
                if(keyboard_state_array[SDL_SCANCODE_UP])
                {
                    Drawer1.decrementSelectedIcon();
                    //testConsole.decrementSelectedIndex();
                }
                if(keyboard_state_array[SDL_SCANCODE_E])
                {
                    //Run onClick from currently selected widget
                    Widget* selectedWidget = RENDERER.currentPage->widget_array[RENDERER.currentPage->selectedItem];
                    selectedWidget->onClick(&RENDERER);
                    //selectedWidget->onClick(&RENDERER);
                    //RENDERER.loadPage(Drawer1.selectIcon());
                    //testConsole.selectOption();
                }
                if(keyboard_state_array[SDL_SCANCODE_B])
                {
                    RENDERER.back();
                }
                /*
                dataCommunication DATA = dataCommunication();
                    char* data = DATA.read_data();
                    if(data != (char*)-1)
                    {
                        std::vector<float>* numbers = DATA.processDataString(data);
                        if(numbers != nullptr)
                        {
                            oilGraph.setValue(numbers->at(0));
                        }
                    }
                */
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                count = count + 1;
                count = count % 6000;
                RPM_Widget.setValue(count);
                oilGraph.setValue(count%14);
                SDL_RenderClear(renderer);
                RENDERER.render();
                SDL_RenderPresent(renderer);

                usleep(30000);
            }
        }

        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
    SDL_Quit();
    return 0;
}
#pragma clang diagnostic pop