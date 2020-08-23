#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <cmath>
#include <linux/can.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "SDL2_gfxPrimitives.h"

using namespace std;
class sensorWarningRules
{
 public:
  TTF_Font* Sans = TTF_OpenFont("/home/dylan/Desktop/sans/OpenSans-Regular.ttf", 45);
  struct rule_struct {
    char ruleName[25];
    int inputChannel; //Refer to canBus_Comms::SensorData struct for channels.
    float warning_low_value;
    float warning_high_value;
    float error_low_value;
    float error_high_value;
  };
  ifstream ruleFile;
  vector<struct rule_struct> rules;
  vector<std::string> renderQueue;//Warning/error strings to be rendered
  sensorWarningRules(string rule_filepath)
  {
    ruleFile = ifstream(rule_filepath);
    processRuleFile();
  }

  void processRuleFile()
  {
    rules.clear();
    string line;
    if (ruleFile.is_open())
    {
      while (getline(ruleFile, line))
      {
        //Verify the first character in the line is '<', and the last character is '>'

        struct rule_struct tempRule;
        if (line[0] == '<' && line[line.length() - 1] == '>')
        {
          int indexCount = 0;
          line = line.substr(1, line.length() - 2);
          //Begin splitting the string
          char* str = (char*)line.c_str();
          char* pch;
          pch = strtok(str, ",");
          while (pch != NULL)
          {
            switch (indexCount)
            {
              case 0:
              {
                strcpy(tempRule.ruleName, pch);
                break;
              }
              case 1:
              {
                int channel = atoi(pch);
                tempRule.inputChannel = channel;
                break;
              }
              case 2:
              {
                //convert string to float
                float f = atof(pch);
                tempRule.warning_low_value = f;
                break;
              }
              case 3:
              {
                float f = atof(pch);
                tempRule.warning_high_value = f;
                break;
              }
              case 4:
              {
                float f = atof(pch);
                tempRule.error_low_value = f;
                break;
              }
              case 5:
              {
                float f = atof(pch);
                tempRule.error_high_value = f;
                break;
              }
            }
            printf("%s\n", pch);
            pch = strtok(NULL, ",");
            indexCount++;
          }
          rules.push_back(tempRule);
        }
      }
    }
  }

  struct rule_struct getRule(std::string ruleName)
  {
    for (auto item : rules)
    {
      if (strcmp(item.ruleName, ruleName.c_str()) == 0)
      {
        return item;
      }
    }
  }

  void checkRules(SDL_Renderer* renderer, CanBus_Comms can_bus_comms)//Check sensor values against warning and error conditions. Render the warning message if applicable
  {
    SensorData sensorData = can_bus_comms.sensorData;
    for(auto item : rules)
    {
      switch(item.inputChannel)
      {
        case 0: //RPM
        {
          break;
        }
        case 1: //CTS
        {
          if(can_bus_comms.sensorData.CTS >= item.warning_low_value && can_bus_comms.sensorData.CTS <= item.warning_high_value)//If the current value is within the warning range.
          {
            std::string warningString = "Warning: " + std::string(item.ruleName);
            renderQueue.push_back(warningString);
          }
          break;
        }
        case 2: //BATT
        {
          if(can_bus_comms.sensorData.BATT >= item.warning_low_value && can_bus_comms.sensorData.BATT <= item.warning_high_value)//If the current value is within the warning range.
          {
            std::string warningString = "Warning: " + std::string(item.ruleName);
            renderQueue.push_back(warningString);
          }
          break;
        }
      }
    }
    //Render the warning strings to the screen
    SDL_Rect warningRectangle;
    if(renderQueue.size() > 0)
    {
      warningRectangle.x = (Window_Width/2)-(250/2);
      warningRectangle.y = Window_Height/2-(250/2);
      warningRectangle.w = 250;
      warningRectangle.h = 250;
      SDL_RenderFillRect(renderer,&warningRectangle);
      for(int index = 0; index < renderQueue.size(); index++)
      {
        SDL_Color textColor = {255, 0, 0};
        SDL_Surface *surfaceMessage;
        surfaceMessage = TTF_RenderText_Solid(Sans,
                                              renderQueue[index].c_str(),
                                              textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
        SDL_Texture *Message;
        Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
        SDL_Rect Message_rect; //create a rect
        Message_rect.x = warningRectangle.x;
        Message_rect.y = warningRectangle.y + 50*index;
        Message_rect.w = 150; // controls the width of the rect
        Message_rect.h = 40; // controls the height of the rect
        SDL_RenderCopy(renderer, Message, nullptr, &Message_rect);
        SDL_DestroyTexture(Message);
        SDL_FreeSurface(surfaceMessage);
      }
      renderQueue.clear();
    }

  }

};
