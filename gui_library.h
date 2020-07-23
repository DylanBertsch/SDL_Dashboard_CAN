#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <math.h>
#include <linux/can.h>
#include <SDL2/SDL_ttf.h>

#include <nanogui-sdl/sdlgui/screen.h>
#include <nanogui-sdl/sdlgui/window.h>
#include <nanogui-sdl/sdlgui/layout.h>
#include <nanogui-sdl/sdlgui/label.h>
#include <nanogui-sdl/sdlgui/checkbox.h>
#include <nanogui-sdl/sdlgui/button.h>
#include <nanogui-sdl/sdlgui/toolbutton.h>
#include <nanogui-sdl/sdlgui/popupbutton.h>
#include <nanogui-sdl/sdlgui/combobox.h>
#include <nanogui-sdl/sdlgui/dropdownbox.h>
#include <nanogui-sdl/sdlgui/progressbar.h>
#include <nanogui-sdl/sdlgui/entypo.h>
#include <nanogui-sdl/sdlgui/messagedialog.h>
#include <nanogui-sdl/sdlgui/textbox.h>
#include <nanogui-sdl/sdlgui/slider.h>
#include <nanogui-sdl/sdlgui/imagepanel.h>
#include <nanogui-sdl/sdlgui/imageview.h>
#include <nanogui-sdl/sdlgui/vscrollpanel.h>
#include <nanogui-sdl/sdlgui/colorwheel.h>
#include <nanogui-sdl/sdlgui/graph.h>
#include <nanogui-sdl/sdlgui/tabwidget.h>
#include <nanogui-sdl/sdlgui/switchbox.h>
#include <nanogui-sdl/sdlgui/formhelper.h>
#include <iostream>


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
#define Widget_BITMAP 10
#define Widget_PagePicker 11
#define Widget_TextView 12

#define Window_Width 850
#define Window_Height 850

#define DASHBOARD_WIDGET 0
#define SDL_WIDGET 1

//Forward Declarations
class Renderer;
class DashboardWidget;
class menuPage;

struct Context{
    Renderer* RENDERER;//Renderer being used to draw to the screen
    SDL_Renderer* renderer;
    menuPage* currentPage;//Page to which the widget belongs.
};

struct pagePickerData{
    int selectedIconIndex = 0;
};

void loadPage(Renderer* renderer,menuPage* page);
struct Context* getContext(Renderer* RENDERER);

class DashboardWidget{
public:
    int xpos;
    int ypos;
    int width;
    int height;
    char widgetName[10];
    int widgetType = -1;
    bool isSelected = false;
    char auxillaryData[250];// Storage for widgets that need extra information; such as the pagepicker for icon selection
    DashboardWidget(int XPOS, int YPOS, int WIDTH, int HEIGHT, char WIDGET_NAME[10], int WIDGET_TYPE)
    {
        xpos = XPOS;
        ypos = YPOS;
        width = WIDTH;
        height = HEIGHT;
        strcpy(widgetName, WIDGET_NAME);
        widgetType = WIDGET_TYPE;
    }
    virtual void onDraw(Renderer *RENDERER) = 0;
    virtual void onClick(Renderer* RENDERER) = 0;
};
/*In order to integrate the sdlgui library with the dashboard library, each menupage must have a "screen" associated to it
 * this will allow the mixture between sdlgui and dashboard widgets.
 */
using namespace sdlgui;
class menuPage : public Screen
{
    struct widget_array_entry{
        int widgetType;
        void* widgetPTR;
    };
public:
    widget_array_entry widget_array[10];
    char title[20];
    int widgetCount = 0;
    int selectedItem = 0;
    void* parentRenderer = nullptr;
    void incrementSelectedWidget();
    void decrementSelectedWidget();
    void addWidget(DashboardWidget* inputWidget);
    void addWidget(sdlgui::Widget* inputWidget);
    menuPage(SDL_Window* pwindow) : Screen(pwindow,Vector2i(0,0),"SDL_GUI")
    {
        memset(widget_array,0,sizeof(widget_array));
    }

    void setTitle(char* newTitle);
    virtual void draw(SDL_Renderer* renderer)
    {

        Screen::draw(renderer);
        //Do SDL calls here
    }
};

void menuPage::addWidget(DashboardWidget *inputWidget) {
    struct widget_array_entry entry;
    entry.widgetType = DASHBOARD_WIDGET;
    entry.widgetPTR = inputWidget;
    memcpy(&widget_array[widgetCount],&entry,sizeof(struct widget_array_entry));
    widgetCount++;
}

void menuPage::addWidget(sdlgui::Widget *inputWidget) {
    struct widget_array_entry entry;
    entry.widgetType = SDL_WIDGET;
    entry.widgetPTR = inputWidget;
    memcpy(&widget_array[widgetCount],&entry,sizeof(struct widget_array_entry));
    widgetCount++;
}

void menuPage::setTitle(char *newTitle) {
    strcpy(title,newTitle);
}

void menuPage::incrementSelectedWidget() {
    //If the page has a pagePicker direct these "increment requests" into the pagepicker widget so icons are selected accordingly
    DashboardWidget* widget = (DashboardWidget*)widget_array[0].widgetPTR;
    if(widgetCount == 1 && widget->widgetType == Widget_PagePicker)
    {
        DashboardWidget* pagePicker = widget;
        struct pagePickerData* pickerData = (struct pagePickerData*)(pagePicker->auxillaryData);
        pickerData->selectedIconIndex++;
    } else
    {
        selectedItem++;
    }
}

void menuPage::decrementSelectedWidget() {
    //If the page has a pagePicker direct these "decrement" requests" into the pagepicker widget so icons are selected accordingly
    DashboardWidget* widget = (DashboardWidget*)widget_array[0].widgetPTR;
    if(widgetCount == 1 && widget->widgetType == Widget_PagePicker)
    {
        DashboardWidget* pagePicker = widget;
        struct pagePickerData* pickerData = (struct pagePickerData*)(pagePicker->auxillaryData);
        if(pickerData->selectedIconIndex == 0)
        {

        } else
        {
            pickerData->selectedIconIndex--;
        }
    }
    else
    {
        selectedItem--;
    }
}

//////////////////////End menuPage functions//////////////////////

//Widgets//
class VerticalGraph : public DashboardWidget{
public:
    float value = 0.0f;
    float maxValue = 0.0f;// Used to determine the scale of the graph
    char unitName[10];
    int tickDivision;
    TTF_Font* Sans = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
    TTF_Font* SansOilGaugeTicks = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
    VerticalGraph(int XPOS, int YPOS, float MAX_Value, int tickDIVISION, char* graphName, char* UNITNAME): DashboardWidget(XPOS, YPOS, 125, 250, graphName, Widget_VGRAPH)
    {
        maxValue = MAX_Value;
        tickDivision = tickDIVISION;
        strcpy(unitName,UNITNAME);

    }

    void setValue(float newValue)
    {
        value = newValue;
    }

    virtual void onClick(Renderer* RENDERER)
    {

    }

    virtual void onDraw(Renderer *RENDERER)
    {
        Context* context = getContext(RENDERER);
        SDL_Rect rect;
        rect.x = xpos;
        rect.y = ypos;
        rect.w = width;
        rect.h = height;
        SDL_SetRenderDrawColor(context->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        //Draw Graph Name
        SDL_Color textColor = {255, 255, 0};
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, widgetName, textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
        SDL_Texture* Message = SDL_CreateTextureFromSurface(context->renderer, surfaceMessage);
        SDL_Rect Message_rect; //create a rect
        Message_rect.x = xpos+(width/2)-25;
        Message_rect.y = ypos;
        Message_rect.w = 50; // controls the width of the rect
        Message_rect.h = 40; // controls the height of the rect
        SDL_RenderCopy(context->renderer, Message, NULL, &Message_rect);
        SDL_DestroyTexture(Message);
        SDL_FreeSurface(surfaceMessage);
        //Draw inner rectangle for bar value
        rect.x = (xpos + (width/2)) - (25/2);
        rect.y = ypos + 50;
        rect.w = 25;
        rect.h = 150;
        SDL_RenderDrawRect(context->renderer, &rect);
        //Draw Bar (Vertical)
        SDL_SetRenderDrawColor(context->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
        rect.x = ((xpos + (width/2)) - (25/2))+2;
        rect.y = ypos + 52;
        rect.w = 21;
        rect.h = ((150)/(maxValue))*value;
        SDL_RenderFillRect(context->renderer, &rect);
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
            SDL_Texture* Message = SDL_CreateTextureFromSurface(context->renderer, surfaceMessage);
            SDL_RenderCopy(context->renderer, Message, NULL, &rect);
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
        Message = SDL_CreateTextureFromSurface(context->renderer, surfaceMessage);
        SDL_RenderCopy(context->renderer, Message, NULL, &rect);
        SDL_DestroyTexture(Message);
        SDL_FreeSurface(surfaceMessage);
    }

};

class label : public DashboardWidget
{
public:
    char labelValue[20];
    int labelColor = 0;
    menuPage* onClick_NextPage = nullptr;
    TTF_Font* Sans = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
    label(char labelName[], int x, int y, int Color) : DashboardWidget(x, y, 250, 45, labelName, Widget_Label)
    {
        strcpy(labelValue,labelName);
        widgetType = Widget_Label;
        labelColor = Color;
    }
    void setValue(char* newValue)
    {
        strcpy(labelValue,newValue);
    }

    void setOnClickPage(menuPage* nextPage)
    {
        onClick_NextPage = nextPage;
    }

    virtual void onClick(Renderer* RENDERER)
    {

    }

};

class BitmapWidget : public DashboardWidget{
private:
    char filePath[250];
    int startIndex = 0;
    int endIndex = 0;
    int value = 0;
public:
    BitmapWidget(int XPOS, int YPOS, int Width, int Height, char* FILEPATH, int START_INDEX, int END_INDEX) : DashboardWidget(XPOS, YPOS, Width, Height, "BITMAPW", Widget_BITMAP)
    {
        strcpy(filePath,FILEPATH);
        startIndex = START_INDEX;
        endIndex = END_INDEX;
    }

    void setValue(int newValue)
    {
        value = newValue;
    }

    virtual void onClick(Renderer* RENDERER)
    {
        int i = 0;

    }

    virtual void onDraw(Renderer *RENDERER)
    {
        Context* context = getContext(RENDERER);
        int index = startIndex+(value);
        if(index > endIndex)
        {
            index = endIndex;
        } else {

        }
        std::string Path = std::string(this->filePath) + std::to_string(index) + ".bmp";
        SDL_Surface *image = SDL_LoadBMP(Path.c_str());
        SDL_Texture *texture = SDL_CreateTextureFromSurface(context->renderer, image);
        SDL_Rect r;
        r.x = xpos;
        r.y = ypos;
        r.w = width;
        r.h = height;
        SDL_RenderCopy(context->renderer, texture, NULL, &r);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(image);
        ////////////////////////////
        //SDL_RenderDrawRect(renderer,&r);
    }

};

class pagePicker : public DashboardWidget
{
public:
    int selectedIconCount = 0;
    TTF_Font* Sans = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 50);

    struct iconStruct{
        char* iconPath;//File path for icon
        char iconName[50];//Name to be displayed under icon
        menuPage* onClickPagePointer;//Page to load when button is clicked.
    };
    std::vector<struct iconStruct> iconVector;

    pagePicker(int x, int y, int width, int height) : DashboardWidget(x, y, width, height, "Drawer", Widget_PagePicker)
    {
        struct pagePickerData pickerData;
        pickerData.selectedIconIndex = 0;
        memcpy(auxillaryData,&pickerData,sizeof(struct pagePickerData));
    }


    void addItem(struct iconStruct inputSTRUCT)
    {
        iconVector.push_back(inputSTRUCT);
    }

    virtual void onClick(Renderer* RENDERER)//When an icon is selected, go to that page
    {
        //Get the current icon
        struct pagePickerData* pickerData = (struct pagePickerData*)(&auxillaryData[0]);
        iconStruct currentIcon = iconVector[pickerData->selectedIconIndex];
        loadPage(RENDERER,currentIcon.onClickPagePointer);
        //Somehow get the renderer to change current page
        //RENDERER->loadPage(currentIcon.onClickPagePointer);
    }

    void onDraw(Renderer *RENDERER)
    {
        Context* context = (Context*)getContext(RENDERER);
        SDL_Rect rect;
        //Draw upper menu graphic
        rect.x = xpos;
        rect.y = ypos;
        rect.w = width;
        rect.h = 100;
        SDL_Surface *image = SDL_LoadBMP("/home/dylan/Desktop/Comp 10000.bmp");//Load Image, upper menu graphic
        SDL_Texture *texture = SDL_CreateTextureFromSurface(context->renderer, image);
        SDL_RenderCopy(context->renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(image);
        //Draw outer Rectangle
        rect.x = xpos;
        rect.y = ypos;
        rect.w = width;
        rect.h = height;
        SDL_SetRenderDrawColor(context->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(context->renderer, &rect);//Draw the outer rectangle of the console window
        /*
        //Draw the 2x3 grid
        for(int i = 0; i < 3; i++)//Vertical Lines
        {
            SDL_RenderDrawLine(renderer,i*200,rect.y+100,i*200,rect.y+height);
        }
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        for(int i = 0; i < 3; i++)//Horizontal Lines
        {
            SDL_RenderDrawLine(renderer,0,i*200+rect.y+100,rect.x+width,i*200+rect.y+100);
        }
         */
        //Index the iconStruct vector
        int rowCount = 0;
        int colCount = 0;
        for(int index = 0; index < iconVector.size(); index++)
        {
            if(index % 3 == 0 && index != 0)//Logic(s) for drawing the icons on the grid.
            {
                rowCount++;
            }
            colCount = index % 3;
            struct iconStruct tempStruct = iconVector[index];
            //Draw the icons onto the grid
            SDL_Surface *image = SDL_LoadBMP(tempStruct.iconPath);//Load Image
            SDL_Texture *texture = SDL_CreateTextureFromSurface(context->renderer, image);
            //Determine the rectangle for each grid square
            SDL_Rect gridRectangle;
            gridRectangle.x = colCount*200+50;
            gridRectangle.y = rowCount*200+rect.y+150;
            gridRectangle.w = 100;
            gridRectangle.h = 100;
            SDL_RenderCopy(context->renderer, texture, NULL, &gridRectangle);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(image);
            //*******Draw Text**********
            SDL_Color textColor = {255, 255, 0};
            struct pagePickerData* pickerData = (struct pagePickerData*)(&auxillaryData[0]);
            if(index == pickerData->selectedIconIndex)
            {
                textColor = {255, 0, 0};
            }

            SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, tempStruct.iconName, textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
            SDL_Texture* Message = SDL_CreateTextureFromSurface(context->renderer, surfaceMessage);
            SDL_Rect Message_rect; //create a rect
            Message_rect.x = gridRectangle.x;
            Message_rect.y = gridRectangle.y + gridRectangle.h + 5;
            Message_rect.w = gridRectangle.w; // controls the width of the rect
            Message_rect.h = 40; // controls the height of the rect
            SDL_RenderCopy(context->renderer, Message, NULL, &Message_rect);
            SDL_DestroyTexture(Message);
            SDL_FreeSurface(surfaceMessage);
            //SDL_RenderDrawRect(renderer,&gridRectangle);
        }
        //End of for loop

    }
};

class TextView : public DashboardWidget {
public:
    TTF_Font *Sans = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 40);
    int stringCount = 0;
    std::vector<std::string> textStrings;
    TTF_Font *Font_Sizes[50];//from 1-49 point size
    TextView(int x, int y, int width, int height) : DashboardWidget(x, y, width, height, "Button", Widget_TextView)
    {
        //initilize stringVector
        for(int i = 0; i < 100; i++)
        {
            textStrings.push_back("");
        }
        //initialize Fonts
        for(int i = 0; i <50; i++)
        {
            Font_Sizes[i] = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", i+1);
        }
    }

    TTF_Font* getFont(int pointSize)
    {
        return Font_Sizes[pointSize+1];
    }

    void insertString(std::string str)//Add String to textview. Text will loop over.
    {
        textStrings[stringCount] = str;
        stringCount++;
        stringCount = stringCount % (height/40);
    }

    void onDraw(Renderer *RENDERER) {
    Context* context = getContext(RENDERER);
    //Create the background rectangle (white)
    SDL_Rect widgetRect;
    widgetRect.x = xpos;
    widgetRect.y = ypos;
    widgetRect.w = width;
    widgetRect.h = height;
    SDL_SetRenderDrawColor(context->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(context->renderer,&widgetRect);
        SDL_SetRenderDrawColor(context->renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
    //Draw grid (debugging)
    SDL_Rect textRect;
    SDL_Color textColor = {255,255,255};
    int y = ypos;
    int size = 40;
    Sans = getFont(size);
    for(int i = 0; i < textStrings.size(); i++)
    {
        int w,h;
        TTF_SizeText(Sans,textStrings[i].c_str(),&w,&h);
        //if the text width exceeds the width of the textview scale down.
        if(w > width)
        {
            std::string str = textStrings[i];
            while(w > width)
            {
                Sans = getFont(size);
                TTF_SizeText(Sans,textStrings[i].c_str(),&w,&h);
                size--;
            }
        }
        textRect.x = xpos;
        textRect.y = y;
        textRect.w = w;
        textRect.h = size+7;
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, textStrings[i].c_str(), textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
        SDL_Texture* Message = SDL_CreateTextureFromSurface(context->renderer, surfaceMessage);
        SDL_RenderCopy(context->renderer, Message, NULL, &textRect);
        SDL_DestroyTexture(Message);
        SDL_FreeSurface(surfaceMessage);
        //SDL_RenderDrawRect(renderer,&gridRectangle);
        //SDL_RenderDrawLine(context->renderer,xpos,y,xpos+width-1,y);
        //SDL_RenderDrawRect(context->renderer, &textRect);
        y = y + size + 5;
    }

    }

    virtual void onClick(Renderer* RENDERER)//When button is selected, run the click handler
    {

    }
};






////End Widgets/////
class Renderer {
private:
    //Define the SDL environment variables
    SDL_Window *window = NULL;

    TTF_Font *Sans = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
    TTF_Font *SansOilGaugeTicks = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
public:
    int xCursor = 0;
    int yCursor = 100;
    SDL_Renderer *renderer = NULL;
    menuPage *PAGES[5];
    menuPage *currentPage = NULL;
    menuPage *previousPage = NULL;
//    editPage EDITOR = editPage();
    int pageCount = 0;

    Renderer(SDL_Window *WINDOW, SDL_Renderer *RENDERER) {
        window = WINDOW;
        renderer = RENDERER;
    }
    void addPage(menuPage* inputPage);
    void incrementSelectedWidget()
    {
        currentPage->incrementSelectedWidget();
    }
    void decrementSelectedWidget()
    {
        currentPage->decrementSelectedWidget();
    }
    void back()
    {
        currentPage = previousPage;
    }
    void render();
    friend void loadPage(menuPage);
};

void Renderer::addPage(menuPage *inputPage) {
    //set the inputPage widgets parentRenderer property
    for(int index = 0; index < inputPage->widgetCount; index++)
    {
        DashboardWidget* w = (DashboardWidget*)inputPage->widget_array[index].widgetPTR;

    }
    if(pageCount == 0)
    {
        currentPage = inputPage;
    }
    PAGES[pageCount] = inputPage;
    inputPage->parentRenderer = this;
    pageCount++;
}

void loadPage(Renderer* renderer,menuPage* page) {//Helper function, loads and changes renderer current page.
    renderer->previousPage = renderer->currentPage;
    renderer->currentPage = page;
}

struct Context* getContext(Renderer* RENDERER)
{
    struct Context* tempContext = (struct Context*)malloc(sizeof(struct Context));
    tempContext->RENDERER = RENDERER;
    tempContext->currentPage = RENDERER->currentPage;
    tempContext->renderer = RENDERER->renderer;
    return tempContext;
}

void Renderer::render() {
    //Draw the page Title
    SDL_Color textColor = {255, 255, 255};
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, currentPage->title, textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 0+(Window_Width/2)-(150/2);
    Message_rect.y = 25;
    Message_rect.w = 150; // controls the width of the rect
    Message_rect.h = 45; // controls the height of the rect
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_DestroyTexture(Message);
    SDL_FreeSurface(surfaceMessage);
    //SDL_RenderDrawRect(renderer,&gridRectangle);
    //End page title drawing

    for(int index = 0; index < currentPage->widgetCount; index++)
    {
        DashboardWidget* widPTR = (DashboardWidget*)currentPage->widget_array[index].widgetPTR;//Load the widget from the currentPage
        if(currentPage->selectedItem == index)
        {
            widPTR->isSelected = true; 
        }
        else
        {
            widPTR->isSelected = false;
        }

        widPTR->onDraw(this);
    }
    int index = 0;
    std::cout << currentPage->selectedItem<< std::endl;
    //Draw sdlWidgets
    for (auto child : currentPage->mChildren)
    {
        if (child->visible())
        {
            if(currentPage->selectedItem == index)
            {
                child->isSelected = true;
            }
            else
            {
                child->isSelected = false;
            }
            child->draw(renderer);

        }
        index++;
    }

    //currentPage->draw(renderer);

}
///////////////////End Renderer Functions////////////////////////