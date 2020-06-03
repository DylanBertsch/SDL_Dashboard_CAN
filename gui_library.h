#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <math.h>
#include <SDL2/SDL_ttf.h>
#define ITEM_OPTION 0
#define ITEM_BUTTON 1
#define ITEM_Label  2
#define ITEM_VGRAPH 3
#define ITEM_ROUNDGAUGE 4
#define ITEM_EDITOR_BUTTON 5
#define ITEM_EDITOR_LABEL 6 //Differs from label becuase this is an editLabel which will spawn a editor on the screen.
#define ITEM_CoordinatePlane 7
#define ITEM_RPM 8

#define DIR_LEFT 0
#define DIR_RIGHT 1
#define DIR_UP 2
#define DIR_DOWN 3

/*
Gui library for SDL

Gui Elements:
1. Label
  Displays string to user.

2. Button
  Invokes function when pressed; function is defined as void funcName()

3. Horizontal Gauge
  Shows a value in bar graph format.
*/

class Widget{
public:
    int xpos;
    int ypos;
    char widgetName[10];
    int widgetType = -1;
    Widget(int XPOS, int YPOS)
    {
        xpos = XPOS;
        ypos = YPOS;
    }
};
class menuOption : public Widget
{
public:
    char optionText[20];
    void* linkedPagePTR;
    menuOption(char optionName[]): Widget(-1, -1)
    {
        strcpy(widgetName,"OKAY");
        strcpy(optionText,optionName);
        widgetType = ITEM_OPTION;
    }
};
using namespace std;
class CoordinatePlaneDisplay : public Widget
{
public:

    class Point{
    public:
        float x;
        float y;
        Point(float X, float Y) {
            x = X;
            y = Y;
        }
    };

    float xmin = 0.0;
    float xmax = 0.0;
    float ymin = 0.0;
    float ymax = 0.0;
    char x_axisString[25] = "";
    char y_axisString[25] = "";
    vector<Point> points;
    CoordinatePlaneDisplay(int xpos, int ypos, float XMIN, float XMAX, float YMIN, float YMAX,char x_axisName[],char y_axisName[]) : Widget(xpos,ypos)
    {
        xmin = XMIN;
        xmax = XMAX;
        ymin = YMIN;
        ymax = YMAX;
        strcpy(x_axisString,x_axisName);
        strcpy(y_axisString,y_axisName);
        widgetType = ITEM_CoordinatePlane;
    }
    void plotPoint(float x, float y);
};

class CircularGauge : public Widget{
public:
    float Value = 50.0f;
    char NAME[20] = "";
    CircularGauge(int xpos, int ypos, char name[10]) : Widget(xpos, ypos) {
        strcpy(NAME,name);
        widgetType = ITEM_ROUNDGAUGE;
    }
    void setValue(float newValue)
    {
        Value = newValue;
    }
};

class RPM_Widget : public Widget{
public:

    class Point{
    public:
        float x;
        float y;
        Point(float X, float Y) {
            x = X;
            y = Y;
        }
    };

    float Value = 100.0f;
    vector<Point> points;
    RPM_Widget(int xpos, int ypos) : Widget(xpos, ypos) {
        widgetType = ITEM_RPM;
    }
    void setValue(float newValue)
    {
        Value = newValue;
    }
};

class VerticalGraph : public Widget{
public:
    float value = 0.0;
    float min = 0.0;
    float max = 1.0;
    char unitName[10];
    VerticalGraph(char* graphName, char* UNITNAME, float MIN, float MAX, int XPOS, int YPOS) : Widget(XPOS, YPOS)
    {
        widgetType = ITEM_VGRAPH;
        min = MIN;
        max = MAX;
        strcpy(widgetName,graphName);
        strcpy(unitName, UNITNAME);
    }

    void setValue(float newValue)
    {
        value = newValue;
    }

};


class label : public Widget
{
public:
    char labelValue[20];
    int labelColor = 0;
    void* editPagePointer;
    label(char labelName[]) : Widget(-1, -1)
    {
        strcpy(labelValue,labelName);
        widgetType = ITEM_Label;
    }

    label(char labelName[], int x, int y, int Color) : Widget(x, y)
    {
        strcpy(labelValue,labelName);
        widgetType = ITEM_Label;
        labelColor = Color;
    }
    void setValue(char newValue[]);
};

class editorLabel : public Widget
{
public:
    char labelValue[20];
    int labelColor = 0;
    void* editPagePointer;
    float* editorValue = NULL;
    editorLabel(char labelName[], int x, int y, int Color, float* editorVALUE) : Widget(x, y)
    {
        strcpy(labelValue,labelName);
        widgetType = ITEM_EDITOR_LABEL;
        editorValue = editorVALUE;
        labelColor = Color;
    }
    void setValue(char newValue[]);
};

class button : public Widget{
public:
    void (*fun_ptr)(void);
    button(char buttonName[], void(*function_pointer)(void), int XPOS, int YPOS) : Widget(XPOS, YPOS)
    {
        widgetType = ITEM_BUTTON;
        memcpy(widgetName,buttonName,10);
        fun_ptr = function_pointer;
    }
    void executeHandler()
    {
        (*fun_ptr)();
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
    void addWidget(Widget* inputWidget);
    Widget* selectMenuItem();//return a page when Widget is clicked
    menuPage()
    {
        memset(widget_array,0,sizeof(widget_array));
    }



};

class editPage : public menuPage
{
public:
    class editorButton : public Widget
    {
        void(editPage::*PTR)() = NULL;
        editPage* PTR2 = NULL;
    public:
        editorButton(char buttonName[], editPage* EditPAGE_PTR,void(editPage::*ptr1)(), int XPOS, int YPOS) : Widget(XPOS, YPOS)
        {
            widgetType = ITEM_EDITOR_BUTTON;
            memcpy(widgetName,buttonName,10);
            PTR = ptr1;
            PTR2 = EditPAGE_PTR;
        }

        void executeHandler()
        {
            (*PTR2.*PTR)();
        }

    };


    float* editPageValue;//There is a value to which the page is linked
    label valueLabel = label("VALUE:",0,25,0);
    editorButton upValueButton = editorButton("UP",this,&editPage::editPageUPHandler,0,125);
    editorButton downValueButton = editorButton("DOWN",this,&editPage::editPageDOWNHandler,0,250);
    editPage() : menuPage()
    {
        setTitle("EDITOR");
        //Connect the buttons to their up and down value functions.
        addWidget(&valueLabel);
        addWidget(&upValueButton);
        addWidget(&downValueButton);
    }

    void setValuePTR(float* newPTR)
    {
        editPageValue = newPTR;
    }

    void editPageUPHandler()
    {
        *editPageValue = *editPageValue + 1;
        //Update the valueLabel
        char output[20];
        char fValue[16];
        sprintf(fValue,"%f",*editPageValue);
        strcpy(output,"VALUE:");
        strcat(output,fValue);
        valueLabel.setValue(output);
    }

    void editPageDOWNHandler()
    {
        *editPageValue = *editPageValue - 1;
        //Update the valueLabel
        char output[20];
        char fValue[16];
        sprintf(fValue,"%f",*editPageValue);
        strcpy(output,"VALUE:");
        strcat(output,fValue);
        valueLabel.setValue(output);
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
    editPage EDITOR = editPage();
    int pageCount = 0;
    Renderer(SDL_Window* WINDOW,SDL_Renderer * RENDERER)
    {
        window = WINDOW;
        renderer = RENDERER;
    }

    void addPage(menuPage* inputPage);
    void selectCurrentOption();
    void moveSelectedItem(int DIRECTION);
    void back();

    void render();
};
