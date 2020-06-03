#include <cstdio>
#include <SDL2/SDL_rect.h>
#include "gui_library.h"

void label::setValue(char newValue[])
{
    strcpy(labelValue,newValue);
}

void editorLabel::setValue(char newValue[])
{

}

void menuPage::selectMoveDown()
{
    selectedItem++;
}

void menuPage::selectMoveUp()
{
    selectedItem--;
}

void menuPage::setTitle(char newTitle[])
{
    strcpy(title, newTitle);
}

void menuPage::addWidget(Widget* inputWidget)
{
    widget_array[widgetCount] = inputWidget;
    widgetCount++;
}

Widget* menuPage::selectMenuItem()
{
    //Get Widget from Widget array storage
    Widget* wid = (Widget*)widget_array[selectedItem];
    return wid;
}

void Renderer::addPage(menuPage* inputPage)
{
    if(pageCount == 0)
    {
        currentPage = inputPage;
    }
    PAGES[pageCount] = inputPage;
    pageCount++;
}

void Renderer::selectCurrentOption()
{
    Widget* wid = currentPage->selectMenuItem();
    if(wid->widgetType == ITEM_EDITOR_LABEL)//Do edit label things... open edit page.
    {
        editorLabel* lbl = (editorLabel*)wid;
        float* val = lbl->editorValue;
        //Serial.println(*val);
        EDITOR.editPageValue = val;
        previousPage = currentPage;
        //Set the editPage float value; the value we are going to be editing....
        currentPage = &EDITOR;
        SDL_RenderClear(renderer);
        render();
    }
    if(wid->widgetType == ITEM_OPTION)
    {
        menuOption* option = (menuOption*)wid;
        previousPage = currentPage;
        currentPage = (menuPage*)option->linkedPagePTR;
        //tftInstance->fillScreen(BLACK);
        render();
    }
    if(wid->widgetType == ITEM_BUTTON)
    {
        button* btn = (button*)wid;
        btn->executeHandler();
        render();
    }
    if(wid->widgetType == ITEM_EDITOR_BUTTON)//In the editor context
    {
        //Run the handler function
        editPage::editorButton* editorButton = (editPage::editorButton*)wid;
        editorButton->executeHandler();
        render();
    }
}
void Renderer::back()
{
    currentPage = previousPage;
    render();
}

void Renderer::render()
{
    //Render the pageTitle
    //SDL_RenderClear(renderer);
    //int titleWidth = XTextWidth(pageFont,currentPage->title,strlen(currentPage->title));
    //XDrawString(display, *window, *gc, 500 - (titleWidth / 2), 35, currentPage->title, strlen(currentPage->title));
    yCursor = 100;

    for(int index = 0; index < currentPage->widgetCount; index++)
    {
        Widget* widPTR = (Widget*)currentPage->widget_array[index];
        if(widPTR->widgetType == ITEM_OPTION)
        {
            if(index == currentPage->selectedItem)//Set the text color green to indicate selection
            {
                //setForegroundColor(0,1000000000,0);
            }
            else// Paint white if text is not selected.
            {
                //XSetForeground(display, *gc, WhitePixel(display, s));
            }
            //Print the Text
            menuOption* option = (menuOption*)currentPage->widget_array[index];
            //XDrawString(display, *window, *gc, xCursor, yCursor, option->optionText, strlen(option->optionText));
            //XSetForeground(display, *gc, WhitePixel(display, s));//Draw a white line
            //XDrawLine(display, *window, *gc, 0, yCursor + 10, 1000, yCursor + 10);
            yCursor = yCursor + 65;

        }

        if(widPTR->widgetType == ITEM_Label)
        {
            label* lbl1 = (label*)currentPage->widget_array[index];
            if(index == currentPage->selectedItem)//Set the text color green to indicate selection
            {
                //setForegroundColor(0,1000000000,0);
            }
            else// Paint white if text is not selected.
            {
                //XSetForeground(display, *gc, WhitePixel(display, s));
            }
            //int textWidth = XTextWidth(pageFont,lbl1->labelValue,strlen(lbl1->labelValue));
            //XDrawRectangle(display,*window,*gc,lbl1->xpos,lbl1->ypos,textWidth,50);
            //XDrawString(display, *window, *gc, lbl1->xpos, lbl1->ypos + 35, lbl1->labelValue, strlen(lbl1->labelValue));
        }

        if(widPTR->widgetType == ITEM_EDITOR_LABEL)
        {
            editorLabel* lbl1 = (editorLabel*)currentPage->widget_array[index];
            if(index == currentPage->selectedItem)//Set the text color green to indicate selection
            {
                //setForegroundColor(0,1000000000,0);
            }
            else// Paint white if text is not selected.
            {
                //XSetForeground(display, *gc, WhitePixel(display, s));
            }
            char copyBuffer[50];
            char fValue[15];
            sprintf(fValue,"%f",*lbl1->editorValue);
            strcpy(copyBuffer,lbl1->labelValue);
            strcat(copyBuffer,fValue);
            //int textWidth = XTextWidth(pageFont,copyBuffer,strlen(copyBuffer));
            //XDrawRectangle(display,*window,*gc,lbl1->xpos,lbl1->ypos,textWidth,50);
            //XDrawString(display, *window, *gc, lbl1->xpos, lbl1->ypos + 35, copyBuffer, strlen(copyBuffer));
        }

        if(widPTR->widgetType == ITEM_BUTTON)
        {
            button* btn = (button*)currentPage->widget_array[index];
            if(index == currentPage->selectedItem)//Set the text color green to indicate selection
            {
              //  setForegroundColor(0,1000000000,0);
            }
            else// Paint white if text is not selected.
            {
                //XSetForeground(display, *gc, WhitePixel(display, s));
            }
            if(btn->xpos > 0 || btn->ypos > 0)//Forced Position bypass vertical layout.
            {
                //int textWidth = XTextWidth(pageFont,btn->widgetName,strlen(btn->widgetName));
                //XDrawRectangle(display, *window, *gc, btn->xpos, btn->ypos, textWidth, 50);
                //XDrawString(display, *window, *gc, btn->xpos, btn->ypos + 35, btn->widgetName, strlen(btn->widgetName));
            }
            else
            {

            }
        }

        if(widPTR->widgetType == ITEM_EDITOR_BUTTON) {
            editPage::editorButton *btn = (editPage::editorButton *) currentPage->widget_array[index];
            if (index == currentPage->selectedItem)//Set the text color green to indicate selection
            {
                //setForegroundColor(0, 1000000000, 0);
            } else// Paint white if text is not selected.
            {
                //XSetForeground(display, *gc, WhitePixel(display, s));
            }
            if (btn->xpos > 0 || btn->ypos > 0)//Forced Position bypass vertical layout.
            {
                //int textWidth = XTextWidth(pageFont, btn->widgetName, strlen(btn->widgetName));
                //XDrawRectangle(display, *window, *gc, btn->xpos, btn->ypos, textWidth, 50);
                //XDrawString(display, *window, *gc, btn->xpos, btn->ypos + 35, btn->widgetName, strlen(btn->widgetName));
            } else {

            }
        }

        if(widPTR->widgetType == ITEM_ROUNDGAUGE)
        {
            CircularGauge* circularGaugeWidget = (CircularGauge*)widPTR;
            int width = 250;
            int height = 250;
            //XDrawRectangle(display, *window,*gc,circularGaugeWidget->xpos,circularGaugeWidget->ypos,250,250);
            //Draw Rings
            for(float theta = 0.0f; theta >= -3.14f; theta = theta - 0.01f)
            {
                float radius = 70.0f;
                float x = radius*cos(theta) + (circularGaugeWidget->xpos+width/2);
                float y = radius*sin(theta) + (circularGaugeWidget->ypos+height/2);
                SDL_RenderDrawPoint(renderer,x,y);
                //XFillArc(display,*window,*gc,x,y,5,5,0,360*64);
                radius = 120.0f;
                x = radius*cos(theta) + (circularGaugeWidget->xpos+width/2);
                y = radius*sin(theta) + (circularGaugeWidget->ypos+height/2);
                SDL_RenderDrawPoint(renderer,x,y);
                //XFillArc(display,*window,*gc,x,y,5,5,0,360*64);
                radius = 69.0f;
                x = radius*cos(theta) + (circularGaugeWidget->xpos+width/2);
                y = radius*sin(theta) + (circularGaugeWidget->ypos+height/2);
                SDL_RenderDrawPoint(renderer,x,y);
                //XFillArc(display,*window,*gc,x,y,5,5,0,360*64);
                radius = 121.0f;
                x = radius*cos(theta) + (circularGaugeWidget->xpos+width/2);
                y = radius*sin(theta) + (circularGaugeWidget->ypos+height/2);
                SDL_RenderDrawPoint(renderer,x,y);
                //XFillArc(display,*window,*gc,x,y,5,5,0,360*64);
                radius = 68.0f;
                x = radius*cos(theta) + (circularGaugeWidget->xpos+width/2);
                y = radius*sin(theta) + (circularGaugeWidget->ypos+height/2);
                SDL_RenderDrawPoint(renderer,x,y);
                //XFillArc(display,*window,*gc,x,y,5,5,0,360*64);
                radius = 122.0f;
                x = radius*cos(theta) + (circularGaugeWidget->xpos+width/2);
                y = radius*sin(theta) + (circularGaugeWidget->ypos+height/2);
                SDL_RenderDrawPoint(renderer,x,y);
                //XFillArc(display,*window,*gc,x,y,5,5,0,360*64);
            }
            //Draw Name Values, first set font
            //XDrawString(display,*window,*gc,circularGaugeWidget->xpos,circularGaugeWidget->ypos+(height/2)+60,nameText.c_str(),strlen(nameText.c_str()));

            //Draw Shaded line for value
            bool colorChanged = false;
            for(float theta = 0.0f; theta <= circularGaugeWidget->Value*0.01744533; theta = theta + 0.05f)
            {

                if(circularGaugeWidget->Value > 90 && colorChanged == false)
                {
                    //setForegroundColor(100000000,10000000000,0);
                    colorChanged = true;
                }

                float radius = 80.0f;
                float x1 = radius*cos(theta+3.14159) + (circularGaugeWidget->xpos+width/2);
                float y1 = radius*sin(theta+3.14159) + (circularGaugeWidget->ypos+height/2);
                radius = 112.0f;
                float x2 = radius*cos(theta+3.14159) + (circularGaugeWidget->xpos+width/2);
                float y2 = radius*sin(theta+3.14159) + (circularGaugeWidget->ypos+height/2);
                //XFillArc(display,*window,*gc,x1,y1,15,15,0,360*64);
                SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
            }
            //XDrawString(display,*window,*gc,circularGaugeWidget->xpos+(width/2)-(textWidth/2),circularGaugeWidget->ypos+(height/2),valueText.c_str(),strlen(valueText.c_str()));

        }
        if(widPTR->widgetType == ITEM_CoordinatePlane)
        {
            CoordinatePlaneDisplay* coordinateWidget = (CoordinatePlaneDisplay*)widPTR;
            float coordinateWidget_Width = 450;
            float coordinateWidget_Height = 450;
            //Draw the bounding rectangle
            //setForegroundColor(100000000,0,0);
            //XDrawRectangle(display,*window,*gc,coordinateWidget->xpos,coordinateWidget->ypos,coordinateWidget_Width,coordinateWidget_Height);
            //XDrawRectangle(display,*window,*gc,coordinateWidget->xpos+1,coordinateWidget->ypos+1,coordinateWidget_Width,coordinateWidget_Height);
            //XDrawRectangle(display,*window,*gc,coordinateWidget->xpos-1,coordinateWidget->ypos-1,coordinateWidget_Width,coordinateWidget_Height);
            //Draw the axis lines
            //setForegroundColor(10000000000,100000000000,100000000000);
            SDL_RenderDrawLine(renderer,coordinateWidget->xpos+75,coordinateWidget->ypos+coordinateWidget_Height-75,coordinateWidget->xpos+coordinateWidget_Width-75,coordinateWidget->ypos+coordinateWidget_Height-75);
            SDL_RenderDrawLine(renderer,coordinateWidget->xpos+75,coordinateWidget->ypos+coordinateWidget_Height-75,coordinateWidget->xpos+75,coordinateWidget->ypos+75);

            //Draw axis names
            //int textWidth_xAxis = XTextWidth(pageFont, coordinateWidget->x_axisString, strlen(coordinateWidget->x_axisString));
            //XDrawString(display,*window,*gc,((coordinateWidget->xpos+coordinateWidget_Width-75)/2.0)+textWidth_xAxis/2,coordinateWidget->ypos+coordinateWidget_Height-25,coordinateWidget->x_axisString,strlen(coordinateWidget->x_axisString));
            //int textWidth_yAxis = XTextWidth(pageFont, coordinateWidget->y_axisString, strlen(coordinateWidget->y_axisString));
            //XDrawString(display,*window,*gc,(coordinateWidget->xpos-textWidth_yAxis/2)-45,(coordinateWidget->ypos+coordinateWidget_Height/2),coordinateWidget->y_axisString,strlen(coordinateWidget->y_axisString));
            //Draw tick marks on axes
            int tickDivision = 25;
            for(int tick_index = 1; tick_index * tickDivision < coordinateWidget->xmax; tick_index++)
            {

                float x1 = coordinateWidget->xpos+70+ (tickDivision * tick_index) * ((coordinateWidget_Width - 75 * 2) / coordinateWidget->xmax);
                float y1 = coordinateWidget->ypos-80+coordinateWidget_Height-3*((coordinateWidget_Height-75*2)/coordinateWidget->ymax);
                float x2 = coordinateWidget->xpos+70+ (tickDivision * tick_index) * ((coordinateWidget_Width - 75 * 2) / coordinateWidget->xmax);
                float y2 = coordinateWidget->ypos-80+coordinateWidget_Height+6*((coordinateWidget_Height-75*2)/coordinateWidget->ymax);
                SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
                x1 = coordinateWidget->xpos+70-3*((coordinateWidget_Width-75*2)/coordinateWidget->xmax);
                y1 = coordinateWidget->ypos-80+coordinateWidget_Height- (tick_index * tickDivision) * ((coordinateWidget_Height - 75 * 2) / coordinateWidget->ymax);
                x2 = coordinateWidget->xpos+70+6*((coordinateWidget_Width-75*2)/coordinateWidget->xmax);
                y2 = coordinateWidget->ypos-80+coordinateWidget_Height- (tick_index * tickDivision) * ((coordinateWidget_Height - 75 * 2) / coordinateWidget->ymax);
                SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
                //Draw Tick Numbers
                //Set the font
                //Font font;
                //font = XLoadFont(display, "-b&h-lucidabright-medium-r-normal--34-240-100-100-p-193-iso8859-1");//xlsfonts
                //pageFont = XQueryFont(display, font);
                //XSetFont(display, *gc, font);
                //std::string tickNumber = std::to_string(tick_index*tickDivision);
                //int textWidth = XTextWidth(pageFont, tickNumber.c_str(), strlen(tickNumber.c_str()));
                //XDrawString(display,*window,*gc,x1-45,y1+9,tickNumber.c_str(),strlen(tickNumber.c_str()));
            }
            //Plot points in vector
            for(CoordinatePlaneDisplay::Point point : coordinateWidget->points)
            {
                /* Scale factor
                 * for example if x max = 100, then when x is 100 the x pixel position should be the width of the coordinate box
                 */
                float x = coordinateWidget->xpos+70+point.x*((coordinateWidget_Width-75*2)/coordinateWidget->xmax);
                float y = coordinateWidget->ypos-80+coordinateWidget_Height-point.y*((coordinateWidget_Height-75*2)/coordinateWidget->ymax);
                SDL_RenderDrawPoint(renderer,x,y);
                //XFillArc(display,*window,*gc,x,y,10,10,0,360*64);
            }


        }
        if(widPTR->widgetType == ITEM_RPM)
        {
            RPM_Widget* rpmWidget = (RPM_Widget*)widPTR;
            float coordinateWidget_Width = 450;
            float coordinateWidget_Height = 450;
            //Draw the bounding rectangle
            //setForegroundColor(100000000,0,0);
            //XDrawRectangle(display,*window,*gc,coordinateWidget->xpos,coordinateWidget->ypos,coordinateWidget_Width,coordinateWidget_Height);
            //XDrawRectangle(display,*window,*gc,coordinateWidget->xpos+1,coordinateWidget->ypos+1,coordinateWidget_Width,coordinateWidget_Height);
            //XDrawRectangle(display,*window,*gc,coordinateWidget->xpos-1,coordinateWidget->ypos-1,coordinateWidget_Width,coordinateWidget_Height);
            //Draw the axis lines
            //setForegroundColor(10000000000,100000000000,100000000000);
            //XDrawLine(display,*window,*gc,rpmWidget->xpos+75,rpmWidget->ypos+coordinateWidget_Height-75,rpmWidget->xpos+coordinateWidget_Width-75,rpmWidget->ypos+coordinateWidget_Height-75);//X-axis
            //XDrawLine(display,*window,*gc,rpmWidget->xpos+75,rpmWidget->ypos+coordinateWidget_Height-75,rpmWidget->xpos+75,rpmWidget->ypos+75);//Y-Axis

            //Plot points in vector
            //XSetLineAttributes(display,*gc,4,0,1,1);
            //setForegroundColor(0,100000000000,0);
            //generate parabolic arcs
            for(float x = 0.0f; x <= 250; x = x + 0.01f)
            {
                RPM_Widget::Point point1 = RPM_Widget::Point(x,5*sqrt(x-10));
                rpmWidget->points.push_back(point1);
                RPM_Widget::Point point2 = RPM_Widget::Point(x,8*sqrt(x));
                rpmWidget->points.push_back(point2);


            }
            //Fill in contour for value
            for(float x = 1.0f; x < 10.0f; x = x + 4.0f)
            {
                float x1 = rpmWidget->xpos+70+x*((coordinateWidget_Width-75*2)/100);
                float y1= rpmWidget->ypos-80+coordinateWidget_Height-(8*sqrt(x))*((coordinateWidget_Height-75*2)/100);
                float x2 = rpmWidget->xpos+70+x*((coordinateWidget_Width-75*2)/100);
                float y2 = rpmWidget->ypos-80+coordinateWidget_Height-5*((coordinateWidget_Height-75*2)/100);
                SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
            }
            for(float x = 13.0f; x <=rpmWidget->Value; x = x + 4.0f)
            {
                float x1 = rpmWidget->xpos+70+x*((coordinateWidget_Width-75*2)/100);
                float y1= rpmWidget->ypos-80+coordinateWidget_Height-(5*sqrt(x-10))*((coordinateWidget_Height-75*2)/100);
                float x2 = rpmWidget->xpos+70+x*((coordinateWidget_Width-75*2)/100);
                float y2 = rpmWidget->ypos-80+coordinateWidget_Height-(8*sqrt(x))*((coordinateWidget_Height-75*2)/100);
                SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
            }
            for(RPM_Widget::Point point : rpmWidget->points)
            {
                /* Scale factor
                 * for example if x max = 100, then when x is 100 the x pixel position should be the width of the coordinate box
                 */
                float x = rpmWidget->xpos+70+point.x*((coordinateWidget_Width-75*2)/100);
                float y = rpmWidget->ypos-80+coordinateWidget_Height-point.y*((coordinateWidget_Height-75*2)/100);
                SDL_RenderDrawPoint(renderer,x,y);
                //XFillArc(display,*window,*gc,x,y,10,10,0,360*64);
            }

        }
        if(widPTR->widgetType == ITEM_VGRAPH)
        {
            VerticalGraph* verticalGraph = (VerticalGraph*)widPTR;
            int width = 125;
            int height = 250;
            SDL_Rect rect;
            rect.x = verticalGraph->xpos;
            rect.y = verticalGraph->ypos;
            rect.w = width;
            rect.h = height;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(renderer,&rect);
            //Draw Graph Name
            SDL_Color textColor = {255, 255, 0};
            SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, verticalGraph->widgetName, textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
            SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            SDL_Rect Message_rect; //create a rect
            Message_rect.x = verticalGraph->xpos+(width/2)-25;
            Message_rect.y = verticalGraph->ypos;
            Message_rect.w = 50; // controls the width of the rect
            Message_rect.h = 40; // controls the height of the rect
            SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
            SDL_DestroyTexture(Message);
            SDL_FreeSurface(surfaceMessage);
            //Draw inner rectangle for bar value
            rect.x = (verticalGraph->xpos + (width/2)) - (25/2);
            rect.y = verticalGraph->ypos + 50;
            rect.w = 25;
            rect.h = 150;
            SDL_RenderDrawRect(renderer,&rect);
            //Draw Bar (Vertical)
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
            rect.x = ((verticalGraph->xpos + (width/2)) - (25/2))+2;
            rect.y = verticalGraph->ypos + 52;
            rect.w = 21;
            rect.h = ((150)/(verticalGraph->max))*verticalGraph->value;
            SDL_RenderFillRect(renderer,&rect);
            //Draw Tick Numbers
            textColor = {0, 255, 0};
            int tickDivision = 25;
            for(int index = 0; index*tickDivision <= verticalGraph->max; index++)
            {
                std::string tickValue = std::to_string(index*tickDivision);
                rect.x = ((verticalGraph->xpos + (width/2)) - (25/2))-40;
                rect.y = verticalGraph->ypos+(index*40)+45;
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
            std::string valueText = std::to_string((int)verticalGraph->value) + " " +std::string(verticalGraph->unitName);
            rect.x = verticalGraph->xpos+(50/2)-10;
            rect.y = verticalGraph->ypos+height-45;
            rect.w = 100;
            rect.h = 40;
            surfaceMessage = TTF_RenderText_Solid(SansOilGaugeTicks, valueText.c_str(), textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
            Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            SDL_RenderCopy(renderer, Message, NULL, &rect);
            SDL_DestroyTexture(Message);
            SDL_FreeSurface(surfaceMessage);
        }

    }//End of for loop
}

void Renderer::moveSelectedItem(int DIRECTION)
{
    if(DIRECTION == DIR_DOWN)
    {
        currentPage->selectedItem++;
        render();
    }
    if(DIRECTION == DIR_UP)
    {
        currentPage->selectedItem--;
        render();
    }
}

void CoordinatePlaneDisplay::plotPoint(float x, float y)
{
    Point point = Point(x,y);
    points.push_back(point);
}