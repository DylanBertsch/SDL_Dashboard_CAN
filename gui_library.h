#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <math.h>
#include <SDL2/SDL_ttf.h>
#define Widget_OPTION 0
#define Widget_BUTTON 1
#define Widget_Label  2
#define Widget_VGRAPH 3
#define Widget_ROUNDGAUGE 4
#define Widget_EDITOR_BUTTON 5
#define Widget_EDITOR_LABEL 6 //Differs from label becuase this is an editLabel which will spawn a editor on the screen.
#define Widget_CoordinatePlane 7
#define Widget_RPM 8
#define Widget_CONSOLE 9

class Widget{
public:
    int xpos;
    int ypos;
    int width;
    int height;
    char widgetName[10];
    int widgetType = -1;
    Widget(int XPOS, int YPOS, int WIDTH, int HEIGHT, char WIDGET_NAME[10], int WIDGET_TYPE)
    {
        xpos = XPOS;
        ypos = YPOS;
        width = WIDTH;
        height = HEIGHT;
        strcpy(widgetName, WIDGET_NAME);
        widgetType = WIDGET_TYPE;
    }
    void onDraw(SDL_Renderer* renderer);
};

class VerticalGraph : public Widget{
public:
    float value = 0.0f;
    float maxValue = 0.0f;// Used to determine the scale of the graph
    char unitName[10];
    int tickDivision;
    TTF_Font* Sans = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
    TTF_Font* SansOilGaugeTicks = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
    VerticalGraph(int XPOS, int YPOS, float MAX_Value, int tickDIVISION, char* graphName, char* UNITNAME): Widget(XPOS,YPOS,125,250,graphName,Widget_VGRAPH)
    {
        maxValue = MAX_Value;
        tickDivision = tickDIVISION;
        strcpy(unitName,UNITNAME);

    }

    void setValue(float newValue)
    {
        value = newValue;
    }

    void onDraw(SDL_Renderer* renderer)
    {
        SDL_Rect rect;
        rect.x = xpos;
        rect.y = ypos;
        rect.w = width;
        rect.h = height;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        //Draw Graph Name
        SDL_Color textColor = {255, 255, 0};
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, widgetName, textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
        SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
        SDL_Rect Message_rect; //create a rect
        Message_rect.x = xpos+(width/2)-25;
        Message_rect.y = ypos;
        Message_rect.w = 50; // controls the width of the rect
        Message_rect.h = 40; // controls the height of the rect
        SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
        SDL_DestroyTexture(Message);
        SDL_FreeSurface(surfaceMessage);
        //Draw inner rectangle for bar value
        rect.x = (xpos + (width/2)) - (25/2);
        rect.y = ypos + 50;
        rect.w = 25;
        rect.h = 150;
        SDL_RenderDrawRect(renderer,&rect);
        //Draw Bar (Vertical)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
        rect.x = ((xpos + (width/2)) - (25/2))+2;
        rect.y = ypos + 52;
        rect.w = 21;
        rect.h = ((150)/(maxValue))*value;
        SDL_RenderFillRect(renderer,&rect);
        //Draw Tick Numbers
        textColor = {0, 255, 0};
        for(int index = 0; index < 4; index++)
        {
            std::string tickValue = std::to_string(index*tickDivision);
            rect.x = ((xpos + (width/2)) - (25/2))-40;
            rect.y = ypos+(index*40)+45;
            rect.w = 25;
            rect.h = 30;
            SDL_Surface* surfaceMessage = TTF_RenderText_Solid(SansOilGaugeTicks, tickValue.c_str(), textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
            SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            SDL_RenderCopy(renderer, Message, NULL, &rect);
            SDL_DestroyTexture(Message);
            SDL_FreeSurface(surfaceMessage);
        }
        //Draw Value text
        textColor = {255,255,255};
        std::string valueText = std::to_string((int)value) + " " +std::string(unitName);
        rect.x = xpos+(50/2)-10;
        rect.y = ypos+height-45;
        rect.w = 100;
        rect.h = 40;
        surfaceMessage = TTF_RenderText_Solid(SansOilGaugeTicks, valueText.c_str(), textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
        Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
        SDL_RenderCopy(renderer, Message, NULL, &rect);
        SDL_DestroyTexture(Message);
        SDL_FreeSurface(surfaceMessage);
    }

};

class label : public Widget
{
public:
    char labelValue[20];
    int labelColor = 0;
    void* editPagePointer;
    TTF_Font* Sans = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
    label(char labelName[], int x, int y, int Color) : Widget(x,y,250,45,labelName,Widget_Label)
    {
        strcpy(labelValue,labelName);
        widgetType = Widget_Label;
        labelColor = Color;
    }
    void setValue(char* newValue)
    {
        strcpy(labelValue,newValue);
    }
};

class consoleWidget : public Widget
{

public:
    int selectedIndex = 0;
    std::vector<Widget*> menuWidgets;
    consoleWidget(int XPOS, int YPOS, int windowWidth, int windowHeight) : Widget(XPOS,YPOS,windowWidth,windowHeight,"CONSOLE",Widget_CONSOLE)
    {

    }

    void addWidget(Widget* widget)
    {
        menuWidgets.push_back(widget);
    }

    void incrementSelectedIndex()
    {
        selectedIndex++;
    }

    void decrementSelectedIndex()
    {
        selectedIndex--;
    }

    void onDraw(SDL_Renderer* renderer)
    {
        SDL_Rect rect;
        rect.x = xpos;
        rect.y = ypos;
        rect.w = width;
        rect.h = height;
        SDL_RenderDrawRect(renderer,&rect);//Draw the outer rectangle of the console window
        //Draw the widgets contained in the menuWidgets vector
        for(int index = 0; index < menuWidgets.size(); index++)
        {
            Widget* widPTR = (Widget*)menuWidgets[index];
            switch(widPTR->widgetType)
            {
                case Widget_Label:
                {
                    label* LABEL = (label*)widPTR;
                    //Draw Value Text
                    SDL_Color textColor = {0, 255, 0};
                    if(selectedIndex == index)
                    {
                        textColor = {255,0,0};
                    }
                    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(LABEL->Sans, LABEL->labelValue, textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
                    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
                    SDL_Rect Message_rect; //create a rect
                    Message_rect.x = xpos+LABEL->xpos;
                    Message_rect.y = ypos+LABEL->ypos;
                    Message_rect.w = LABEL->width; // controls the width of the rect
                    Message_rect.h = LABEL->height; // controls the height of the rect
                    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
                    SDL_DestroyTexture(Message);
                    SDL_FreeSurface(surfaceMessage);
                    break;
                }
            }
        }
    }
};


class menuPage
{
public:
    Widget* widget_array[5];
    char title[20];
    int widgetCount = 0;
    int selectedItem = 0;
    void selectMoveDown();
    void selectMoveUp();
    void setTitle(char newTitle[]);
    void addWidget(Widget* inputWidget)
    {
        widget_array[widgetCount] = inputWidget;
        widgetCount++;
    }
    Widget* selectMenuItem();//return a page when Widget is clicked
    menuPage()
    {
        memset(widget_array,0,sizeof(widget_array));
    }



};

class Renderer
{
private:
    //Define the SDL environment variables
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    TTF_Font* Sans = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
    TTF_Font* SansOilGaugeTicks = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
public:
    int xCursor = 0;
    int yCursor = 100;
    menuPage* PAGES[5];
    menuPage* currentPage = NULL;
    menuPage* previousPage = NULL;
//    editPage EDITOR = editPage();
    int pageCount = 0;
    Renderer(SDL_Window* WINDOW,SDL_Renderer * RENDERER)
    {
        window = WINDOW;
        renderer = RENDERER;
    }

    void addPage(menuPage* inputPage)
    {
        if(pageCount == 0)
        {
            currentPage = inputPage;
        }
        PAGES[pageCount] = inputPage;
        pageCount++;
    }
    void selectCurrentOption();
    void moveSelectedItem(int DIRECTION);
    void back();

    void render()
    {
        for(int index = 0; index < currentPage->widgetCount; index++)
        {
            Widget* widPTR = (Widget*)currentPage->widget_array[index];//Load the widget from the currentPage
            switch(widPTR->widgetType)
            {
                case Widget_VGRAPH: {
                    VerticalGraph *vGraph = (VerticalGraph *) currentPage->widget_array[index];
                    vGraph->onDraw(renderer);
                    break;
                }
                case Widget_CONSOLE: {
                    consoleWidget* cWidget = (consoleWidget*)currentPage->widget_array[index];
                    cWidget->onDraw(renderer);
                    break;
                }


            }
            //Call the widget's onDraw function to render the widget on the screen.


        }
    }
};

