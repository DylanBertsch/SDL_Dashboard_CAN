#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <math.h>
#include <linux/can.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#define Widget_BUTTON 1
#define Widget_Label  2
#define Widget_VGRAPH 3
#define Widget_BITMAP 10
#define Widget_PagePicker 11
#define Widget_TextView 12
#define Widget_Table 13

#define Window_Width 850
#define Window_Height 850

#define DASHBOARD_WIDGET 0

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
class menuPage
{
protected:

public:
    struct widget_array_entry{
        int widgetType;
        void* widgetPTR;
    };
    std::vector<widget_array_entry> widgetVector;
    char title[20];
    int selectedItem = 0;
    void* parentRenderer = nullptr;
    void incrementSelectedWidget();
    void decrementSelectedWidget();
    void addWidget(DashboardWidget* inputWidget);
    menuPage(SDL_Window* pwindow)
    {

    }

    void setTitle(char* newTitle);
};

void menuPage::addWidget(DashboardWidget *inputWidget) {
    widget_array_entry entry;
    entry.widgetType = DASHBOARD_WIDGET;
    entry.widgetPTR = (void*)inputWidget;
    widgetVector.push_back(entry);
}

void menuPage::setTitle(char *newTitle) {
    strcpy(title,newTitle);
}

void menuPage::incrementSelectedWidget() {
    //If the page has a pagePicker direct these "increment requests" into the pagepicker widget so icons are selected accordingly
    DashboardWidget* widget = (DashboardWidget*)widgetVector[0].widgetPTR;
    if(widgetVector.size() == 1 && widget->widgetType == Widget_PagePicker)
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
    DashboardWidget* widget = (DashboardWidget*)widgetVector[0].widgetPTR;
    if(widgetVector.size() == 1 && widget->widgetType == Widget_PagePicker)
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
        ////////return addr.can_addr;////////////////////
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
/*
 * Simple table for displaying information
 */
class Table : public DashboardWidget{
public:
    struct item{
        char name[10];
        char value[10];
    };
    TTF_Font *Font_Sizes[50];//from 1-49 point size
    std::vector<struct item> items;

    Table(int x, int y,int width, int height) : DashboardWidget(x,y,width,height,"Table",Widget_Table)
    {
        //initialize Fonts
        for(int i = 0; i <50; i++)
        {
            Font_Sizes[i] = TTF_OpenFont("/home/dylan/Desktop/sans/Orbitron-Black.ttf", i+1);
        }
        //initialize items
        struct item emptyStruct;
        strcpy(emptyStruct.name,"");
        for(int i = 0; i < 5; i++)
        {
            items.push_back(emptyStruct);
        }
    }

    void setValue(int index, char* name, char* value)
    {
        strcpy(items[index].name,name);
        strcpy(items[index].value,value);

    }

    TTF_Font* getFont(int pointSize)
    {
        return Font_Sizes[pointSize+1];
    }

    void onDraw(Renderer *RENDERER) {
        Context *context = getContext(RENDERER);
        SDL_Rect rect;
        rect.x = xpos;
        rect.y = ypos;
        rect.w = width;
        rect.h = height;
        SDL_SetRenderDrawColor(context->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        //SDL_RenderDrawRect(context->renderer,&rect);
        //Draw values
        int y = ypos;
        int index = 0;
        int w,h;
        while(y < ypos+height)
        {
            SDL_Color textColor = {255,255,0};
            SDL_Surface* surfaceMessage = TTF_RenderText_Solid(getFont(20), items[index].name, textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
            SDL_Texture* Message = SDL_CreateTextureFromSurface(context->renderer, surfaceMessage);
            SDL_Rect Message_rect; //create a rect
            Message_rect.x = xpos+(width*.05);
            Message_rect.y = y;
            Message_rect.w = width*.40;
            Message_rect.h = 45;
            SDL_RenderCopy(context->renderer, Message, NULL, &Message_rect);
            SDL_DestroyTexture(Message);
            SDL_FreeSurface(surfaceMessage);

            textColor = {255,255,0};
            surfaceMessage = TTF_RenderText_Solid(getFont(20), items[index].value, textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
            Message = SDL_CreateTextureFromSurface(context->renderer, surfaceMessage);
            Message_rect.x = xpos+width*.55;
            Message_rect.y = y;
            TTF_SizeText(getFont(20),items[index].value,&w,&h);
            Message_rect.w = w;//width*.40;
            Message_rect.h = 45;
            SDL_RenderCopy(context->renderer, Message, NULL, &Message_rect);
            SDL_DestroyTexture(Message);
            SDL_FreeSurface(surfaceMessage);

            y = y + 50;
            index++;
        }


        //SDL_RenderDrawLine(context->renderer,xpos+(width/2),ypos,xpos+(width/2),ypos+height);

    }

    virtual void onClick(Renderer* RENDERER)//When button is selected, run the click handler
    {

    }

};

class Button : public DashboardWidget{
public:
    TTF_Font* Sans = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 75);
    void (*onClick_functionPTR)(void) = nullptr;
    Button(int x, int y, char* buttonName) : DashboardWidget(x,y,width,height,"Button",Widget_BUTTON)
    {
        strcpy(this->widgetName,buttonName);
        width = 200;
        height = 45;
    }

    void onDraw(Renderer *RENDERER)
    {
        Context* context = getContext(RENDERER);
        int count = 0;
        for(int i = 0; i < height; i++)//Draw the background for the button
        {
            SDL_RenderDrawLine(context->renderer, xpos, ypos + i, xpos + width, ypos + i);
            SDL_SetRenderDrawColor(context->renderer, count, count, count, SDL_ALPHA_OPAQUE);
            count+= 5;
        }
        //Draw Button Rectangle
        SDL_Rect outline;
        outline.x = xpos;
        outline.y = ypos;
        outline.h = height;
        outline.w = width;
        SDL_SetRenderDrawColor(context->renderer, 51, 51, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(context->renderer, &outline);
        //Draw the Button name
        //Determine if button is selected
        SDL_Color textColor;
        if(this->isSelected == true)
        {
            textColor = {0, 255, 0};
        } else
        {
            textColor = {255, 255, 255};
        }

        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, this->widgetName, textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
        SDL_Texture* Message = SDL_CreateTextureFromSurface(context->renderer, surfaceMessage);
        SDL_Rect Message_rect; //create a rect
        Message_rect.x = xpos+(width/2)-(100/2);
        Message_rect.y = ypos+5;
        Message_rect.w = 100; // controls the width of the rect
        Message_rect.h = 35; // controls the height of the rect
        SDL_RenderCopy(context->renderer, Message, NULL, &Message_rect);
        SDL_DestroyTexture(Message);
        SDL_FreeSurface(surfaceMessage);
        //SDL_RenderDrawRect(renderer,&gridRectangle);
    }

    void setOnClickHandler(void(*function_pointer)(void))
    {
        onClick_functionPTR = function_pointer;
    }

    virtual void onClick(Renderer* RENDERER)//When button is selected, run the click handler
    {
        if(onClick_functionPTR != nullptr) {
            (*onClick_functionPTR)();//Do what needs to be done. TODO: have onClick return the value from onClick_functionPTR
        }
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
    void onClick();//Run the currently selected widgets onClick;
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
    Message_rect.x = 0+(Window_Width/2)-(200/2);
    Message_rect.y = 25;
    Message_rect.w = 200; // controls the width of the rect
    Message_rect.h = 45; // controls the height of the rect
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_DestroyTexture(Message);
    SDL_FreeSurface(surfaceMessage);
    //SDL_RenderDrawRect(renderer,&gridRectangle);
    //End page title drawing
    for(int index = 0; index < currentPage->widgetVector.size(); index++)
    {
        if(currentPage->widgetVector[index].widgetType == DASHBOARD_WIDGET)
        {
            DashboardWidget* widPTR = (DashboardWidget*)currentPage->widgetVector[index].widgetPTR;//Load the widget from the currentPage
            if(currentPage->selectedItem == index)
            {
                widPTR->isSelected = true;
            }
            else {
                widPTR->isSelected = false;
            }
            widPTR->onDraw(this);
        }

    }
}
///////////////////End Renderer Functions////////////////////////