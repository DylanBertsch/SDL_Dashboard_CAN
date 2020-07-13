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
#define Widget_BITMAP 10
#define Widget_PagePicker 11
//Forward Declarations
class Renderer;
class Widget;
class menuPage;
void loadPage(Renderer* renderer,menuPage* page);
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
    virtual void onDraw(SDL_Renderer* renderer) = 0;
    virtual void onClick(Renderer* RENDERER) = 0;
};

class menuPage
{
public:
    Widget* widget_array[5];
    char title[20];
    int widgetCount = 0;
    int selectedItem = 0;
    void* parentRenderer = nullptr;
    void selectMoveDown();
    void selectMoveUp();
    void addWidget(Widget* inputWidget);
    menuPage()
    {
        memset(widget_array,0,sizeof(widget_array));
    }

    void setTitle(char* newTitle);

};

void menuPage::addWidget(Widget *inputWidget) {
    widget_array[widgetCount] = inputWidget;
    widgetCount++;
}

void menuPage::setTitle(char *newTitle) {
    strcpy(title,newTitle);
}

//////////////////////End menuPage functions//////////////////////

//Widgets//
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

    virtual void onClick(Renderer* RENDERER)
    {

    }

    virtual void onDraw(SDL_Renderer* renderer)
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
    menuPage* onClick_NextPage = nullptr;
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

    void setOnClickPage(menuPage* nextPage)
    {
        onClick_NextPage = nextPage;
    }

    virtual void onClick(Renderer* RENDERER)
    {

    }

};

class BitmapWidget : public Widget{
private:
    char filePath[250];
    int startIndex = 0;
    int endIndex = 0;
    int value = 0;
public:
    BitmapWidget(int XPOS, int YPOS, int Width, int Height, char* FILEPATH, int START_INDEX, int END_INDEX) : Widget(XPOS,YPOS,Width,Height,"BITMAPW",Widget_BITMAP)
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

    virtual void onDraw(SDL_Renderer* renderer)
    {
        int index = startIndex+(value);
        if(index > endIndex)
        {

        } else {
            std::string Path = std::string(this->filePath) + std::to_string(index) + ".bmp";
            SDL_Surface *image = SDL_LoadBMP(Path.c_str());
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
            SDL_Rect r;
            r.x = xpos;
            r.y = ypos;
            r.w = width;
            r.h = height;
            SDL_RenderCopy(renderer, texture, NULL, &r);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(image);
            ////////////////////////////
            //SDL_RenderDrawRect(renderer,&r);
        }
    }

};

class pagePicker : public Widget
{
public:
    int selectedIconCount = 0;
    TTF_Font* Sans = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 50);

    struct iconStruct{
        char* iconPath;//File path for icon
        char* iconName;//Name to be displayed under icon
        menuPage* onClickPagePointer;//Page to load when button is clicked.
    };

    std::vector<struct iconStruct> iconVector;

    void addItem(struct iconStruct inputSTRUCT)
    {
        inputSTRUCT.iconName = "Settings";
        iconVector.push_back(inputSTRUCT);
    }
    void incrementSelectedIcon()
    {
        selectedIconCount++;
    }

    void decrementSelectedIcon()
    {
        if(selectedIconCount == 0)
        {

        } else
        {
            selectedIconCount--;
        }
    }

    virtual void onClick(Renderer* RENDERER)//When an icon is selected, go to that page
    {
        //Get the current icon
        iconStruct currentIcon = iconVector[selectedIconCount];
        loadPage(RENDERER,currentIcon.onClickPagePointer);
        //Somehow get the renderer to change current page
        //RENDERER->loadPage(currentIcon.onClickPagePointer);
    }

    pagePicker(int x, int y, int width, int height) : Widget(x,y,width,height,"Drawer",Widget_PagePicker)
    {

    }
    void onDraw(SDL_Renderer* renderer)
    {
        SDL_Rect rect;
        //Draw upper menu graphic
        rect.x = xpos;
        rect.y = ypos;
        rect.w = width;
        rect.h = 100;
        SDL_Surface *image = SDL_LoadBMP("/home/dylan/Desktop/Comp 10000.bmp");//Load Image, upper menu graphic
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(image);
        //Draw outer Rectangle
        rect.x = xpos;
        rect.y = ypos;
        rect.w = width;
        rect.h = height;
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer,&rect);//Draw the outer rectangle of the console window
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
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
            //Determine the rectangle for each grid square
            SDL_Rect gridRectangle;
            gridRectangle.x = colCount*200+50;
            gridRectangle.y = rowCount*200+rect.y+150;
            gridRectangle.w = 100;
            gridRectangle.h = 100;
            SDL_RenderCopy(renderer, texture, NULL, &gridRectangle);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(image);
            //*******Draw Text**********
            SDL_Color textColor = {255, 255, 0};
            if(index == selectedIconCount)
            {
                textColor = {255, 0, 0};
            }

            SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, tempStruct.iconName, textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
            SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            SDL_Rect Message_rect; //create a rect
            Message_rect.x = gridRectangle.x;
            Message_rect.y = gridRectangle.y + gridRectangle.h + 5;
            Message_rect.w = gridRectangle.w; // controls the width of the rect
            Message_rect.h = 40; // controls the height of the rect
            SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
            SDL_DestroyTexture(Message);
            SDL_FreeSurface(surfaceMessage);
            //SDL_RenderDrawRect(renderer,&gridRectangle);
        }
        //End of for loop

    }
};



////End Widgets/////
class Renderer {
private:
    //Define the SDL environment variables
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *Sans = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
    TTF_Font *SansOilGaugeTicks = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
public:
    int xCursor = 0;
    int yCursor = 100;
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
    void selectCurrentOption();
    void moveSelectedItem(int DIRECTION);
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
        Widget* w = inputPage->widget_array[index];

    }
    if(pageCount == 0)
    {
        currentPage = inputPage;
    }
    PAGES[pageCount] = inputPage;
    inputPage->parentRenderer = this;
    pageCount++;
}

void loadPage(Renderer* renderer,menuPage* page) {
    renderer->previousPage = renderer->currentPage;
    renderer->currentPage = page;
}

void Renderer::render() {
    for(int index = 0; index < currentPage->widgetCount; index++)
    {
        Widget* widPTR = (Widget*)currentPage->widget_array[index];//Load the widget from the currentPage
        widPTR->onDraw(renderer);

    }
}
///////////////////End Renderer Functions////////////////////////