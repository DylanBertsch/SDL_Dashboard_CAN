//Dashboard library
#include <SDL2/SDL.h>
#include <stdio.h>
#include "gui_library.h"
#include "canBus_Comms.h"
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
#include <GLES/gl.h>
#include <SDL_opengl.h>

#endif
int count = 0;
void testCanBusRead();
CanBus_Comms canbus_comms;
TextView* textView;
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
    auto context = SDL_GL_CreateContext(window);
    // Check that the window was successfully made
    if(window==NULL){
        // In the event that the window could not be made...
        std::cout << "Could not create window: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    auto Gl_Context = SDL_GL_CreateContext(window);

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
    Button testBus_Button = Button(0, 150, "Test Bus");
    testBus_Button.setOnClickHandler(testCanBusRead);
    VerticalGraph oilGraph = VerticalGraph(0, 600, 75, 25,"OIL", "PSI");
    VerticalGraph batteryGraph = VerticalGraph(0,350,14,5,"Batt","Volt");
    BitmapWidget RPM_Widget = BitmapWidget(150,0,500,500,"/home/dylan/Documents/RPM_2_AME/Comp ",10000,10064);
    BitmapWidget Coolant_Widget = BitmapWidget(600,540,300,300,"/home/dylan/Documents/airStyle_AME/Comp ",1000,3000);
    Table table = Table(250,550,300,300);
    textView = new TextView(175,350,500,500);
    table.setValue(0,"CTS:","150");
    table.setValue(1,"Timing:","15.00");
    canBusPage.addWidget(&testBus_Button);
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
    canBusPage.addWidget(textView);
    dashPage.addWidget(&oilGraph);
    dashPage.addWidget(&RPM_Widget);
    dashPage.addWidget(&Coolant_Widget);
    dashPage.addWidget(&batteryGraph);
    dashPage.addWidget(&table);
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
                    //RENDERER.onClick();
                    DashboardWidget* selectedWidget = (DashboardWidget*)RENDERER.currentPage->widgetVector[RENDERER.currentPage->selectedItem].widgetPTR;
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
            }
            SDL_SetRenderDrawColor(renderer, 0xd3, 0xd3, 0xd3, 0xff );
            SDL_RenderClear( renderer );
            // Render the rect to the screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            std::string str = std::to_string(count);
            table.setValue(0,"CTS:",(char*)str.c_str());
            batteryGraph.setValue(10);
            RPM_Widget.setValue(count);
            Coolant_Widget.setValue(count);
            SDL_RenderClear(renderer);
            RENDERER.render();
            SDL_RenderPresent(renderer);
            count = count + 1;
            count = count % 15;
        }
    }
    catch (const std::runtime_error &e)
    {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
#if defined(_WIN32)
        MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
        
#endif
        return -1;
    }
    return 0;
}

void testCanBusRead()
{
    std::string output;
    if(canbus_comms.getStatus() == 0)
    {
        output = "Can Bus Error.";
        textView->insertString(output.c_str());
    }
    else {
        canbus_comms.readFrame();
        output = std::to_string(canbus_comms.frame.data[0]);
        textView->insertString(output.c_str());
    }
}