/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <stdio.h>
#include "gui_library.h"
#include <unistd.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
SDL_Window * window = NULL;
SDL_Renderer* renderer = NULL;

void testFunction()
{
    int i = 0;
}

int main( int argc, char* args[] )
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        TTF_Init();
        window = SDL_CreateWindow("SDL2 Window",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  850, 850,
                                  0);
        renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
        int count = 0;
        Renderer RENDERER = Renderer(window,renderer);
        menuPage mainPage = menuPage();
        menuPage dashPage = menuPage();
        menuPage canBusPage = menuPage();
        canBusPage.setTitle("Can Bus Settings");
        VerticalGraph oilGraph = VerticalGraph(0, 600, 75, 25,"OIL", "PSI");
        VerticalGraph batteryGraph = VerticalGraph(0,350,14,5,"Batt","Volt");
        BitmapWidget RPM_Widget = BitmapWidget(150,0,500,500,"/home/dylan/Documents/RPM_2_AME/Comp ",10000,10064);
        //mainPage.addWidget(&oilGraph);
        //mainPage.addWidget(&RPM_Widget);
        pagePicker Drawer1 = pagePicker(0,25,600,500);
        pagePicker::iconStruct icon1;
        strcpy(icon1.iconName,"Main Dash");
        icon1.iconPath = "/home/dylan/Desktop/icon1.bmp";
        icon1.onClickPagePointer = (menuPage*)&dashPage;
        Drawer1.addItem(icon1);
        icon1.iconPath = "/home/dylan/Desktop/icon2.bmp";
        icon1.onClickPagePointer = (menuPage*)&canBusPage;
        strcpy(icon1.iconName,"Can Bus Info");
        Drawer1.addItem(icon1);
        strcpy(icon1.iconName,"");
        icon1.iconPath = "/home/dylan/Desktop/icon3.bmp";
        Drawer1.addItem(icon1);
        icon1.iconPath = "/home/dylan/Desktop/icon4.bmp";
        Drawer1.addItem(icon1);
        mainPage.addWidget(&Drawer1);
        RENDERER.addPage(&mainPage);
        RENDERER.addPage(&canBusPage);
        Button button = Button(50,150,"Start");
        TextView textView = TextView(50,250,250,250);
        textView.insertString("RPM: 250");
        textView.insertString("BATT: 11.3");
        textView.insertString("CTS 168");
        

        button.setOnClickHandler(testFunction);
        canBusPage.addWidget(&button);
        canBusPage.addWidget(&textView);
        dashPage.addWidget(&oilGraph);
        dashPage.addWidget(&RPM_Widget);
        dashPage.addWidget(&batteryGraph);
        oilGraph.onClick(&RENDERER);

            while (true) {
                SDL_Event event;
                const Uint8* keyboard_state_array = SDL_GetKeyboardState(NULL);
                SDL_WaitEventTimeout(&event,1);
                if(keyboard_state_array[SDL_SCANCODE_DOWN])
                {
                    RENDERER.incrementSelectedWidget();
                    //testConsole.incrementSelectedIndex();
                }
                if(keyboard_state_array[SDL_SCANCODE_UP])
                {
                    RENDERER.decrementSelectedWidget();
                    //testConsole.decrementSelectedIndex();
                }
                if(keyboard_state_array[SDL_SCANCODE_E])
                {
                    //Run onClick from currently selected widget
                    Widget* selectedWidget = RENDERER.currentPage->widget_array[RENDERER.currentPage->selectedItem];
                    selectedWidget->onClick(&RENDERER);
                }
                if(keyboard_state_array[SDL_SCANCODE_B])
                {
                    RENDERER.back();
                }

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                count = count + 1;
                count = count % 6000;
                RPM_Widget.setValue(count%60);
                batteryGraph.setValue(10);
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