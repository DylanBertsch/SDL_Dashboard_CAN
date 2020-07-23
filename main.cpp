/*
    sdlgui/example1.cpp -- C++ version of an example application that shows
    how to use the various widget classes.

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/



//Dashboard library
#include <SDL2/SDL.h>
#include <stdio.h>
#include "gui_library.h"
#include <unistd.h>



#include <memory>

#if defined(_WIN32)
#include <windows.h>
#endif
#include <iostream>

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#if defined(_WIN32)
#include <SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

using std::cout;
using std::cerr;
using std::endl;

#undef main

using namespace sdlgui;

class TestWindow : public Screen
{
public:
    TestWindow( SDL_Window* pwindow, int rwidth, int rheight )
            : Screen( pwindow, Vector2i(rwidth, rheight), "SDL_gui Test")
    {
        auto& nwindow = window("Button demo", Vector2i{0, 0}).withFixedSize(Vector2i{850,850});
        auto& b1 = button("Test",[]{cout<<"ok"<<endl;}).withPosition(Vector2i{50,50});
        nwindow.addChild(&b1);
        sdlgui::Button* b = (sdlgui::Button*)&b1;
        Color color(255,0,0,75);
        b->setTextColor(color);
        b1.mouseButtonEvent(Vector2i{0,0},SDL_BUTTON_LEFT,0,1);
        performLayout(mSDL_Renderer);
    }

    ~TestWindow() {
    }

    virtual void draw(SDL_Renderer* renderer)
    {

        Screen::draw(renderer);
        //Do SDL calls here
    }

    virtual void drawContents()
    {
    }
private:
    std::vector<SDL_Texture*> mImagesData;
    int mCurrentImage;
};

int main(int /* argc */, char ** /* argv */)
{
    char rendername[256] = {0};
    SDL_RendererInfo info;

    SDL_Init(SDL_INIT_VIDEO);   // Initialize SDL2
    TTF_Init();
    SDL_Window *window;        // Declare a pointer to an SDL_Window

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    int winWidth = 850;
    int winHeight = 850;

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
            "An SDL2 window",         //    const char* title
            SDL_WINDOWPOS_UNDEFINED,  //    int x: initial x position
            SDL_WINDOWPOS_UNDEFINED,  //    int y: initial y position
            winWidth,                      //    int w: width, in pixels
            winHeight,                      //    int h: height, in pixels
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN  | SDL_WINDOW_ALLOW_HIGHDPI        //    Uint32 flags: window options, see docs
    );

    // Check that the window was successfully made
    if(window==NULL){
        // In the event that the window could not be made...
        std::cout << "Could not create window: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    auto context = SDL_GL_CreateContext(window);

    for (int it = 0; it < SDL_GetNumRenderDrivers(); it++) {
        SDL_GetRenderDriverInfo(it, &info);
        strcat(rendername, info.name);
        strcat(rendername, " ");
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    //TestWindow *screen = new TestWindow(window, winWidth, winHeight);

    Renderer RENDERER = Renderer((SDL_Window*)&window,renderer);
    menuPage mainPage = menuPage(window);
    menuPage dashPage = menuPage(window);
    menuPage canBusPage = menuPage(window);
    canBusPage.setTitle("Can Bus Settings");
    //Integrate sdlgui with our gui widgets???
    auto& b1 = canBusPage.button("Test",[]{cout<<"ok"<<endl;}).withPosition(Vector2i{50,50});
    sdlgui::Button* b = (sdlgui::Button*)&b1;
    auto& b2 = canBusPage.button("HMM",[]{cout<<"ok"<<endl;}).withPosition(Vector2i{50,100});
    auto& t1 = canBusPage.textbox().withPosition(Vector2i{50,150}).withFixedSize(Vector2i{250,25});
    sdlgui::TextBox* t2 = (sdlgui::TextBox*)&t1;
    t2->setEditable(true);
    t2->setValue("Hello");
    t2->setAlignment(sdlgui::TextBox::Alignment::Left);
    Color color(255,0,0,75);
    b->setTextColor(color);
    b1.mouseButtonEvent(Vector2i{0,0},SDL_BUTTON_LEFT,0,1);
    canBusPage.performLayout(renderer);

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
    TextView textView = TextView(50,250,500,500);
    textView.insertString("RPM: 250");
    textView.insertString("BATT: 11.3");
    textView.insertString("This is a test of the dfljksljsdfljksdf");
    textView.insertString("Canbus test successful");
    canBusPage.addWidget(&textView);
    dashPage.addWidget(&oilGraph);
    dashPage.addWidget(&RPM_Widget);
    dashPage.addWidget(&batteryGraph);
    oilGraph.onClick(&RENDERER);

    bool quit = false;
    try
    {
        SDL_Event e;
        while(true)
        {
            SDL_Event event;
            while( SDL_PollEvent( &e ) != 0 )
            {
                const Uint8* keyboard_state_array = SDL_GetKeyboardState(NULL);
                //SDL_WaitEventTimeout(&event,50);
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
                    DashboardWidget* selectedWidget = (DashboardWidget*)RENDERER.currentPage->widget_array[RENDERER.currentPage->selectedItem].widgetPTR;
                    selectedWidget->onClick(&RENDERER);
                }
                if(keyboard_state_array[SDL_SCANCODE_B])
                {
                    RENDERER.back();
                }
                //User requests quit
                if( e.type == SDL_QUIT )
                {
                    quit = true;
                }
                canBusPage.onEvent( e );
            }
            SDL_SetRenderDrawColor(renderer, 0xd3, 0xd3, 0xd3, 0xff );
            SDL_RenderClear( renderer );
            // Render the rect to the screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            batteryGraph.setValue(10);
            SDL_RenderClear(renderer);
            RENDERER.render();

            if(RENDERER.currentPage == &canBusPage)
            {
                //screen->drawAll();
            }

            SDL_RenderPresent(renderer);

        }
    }
    catch (const std::runtime_error &e)
    {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
#if defined(_WIN32)
        MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
        std::cerr << error_msg << endl;
#endif
        return -1;
    }
    return 0;
}