#include "utility.h"
#include <regex>
#include <iostream>

using namespace std;

TCODColor MabinogiBrown = TCODColor(95, 71, 61);
TCODColor DarkMabinogiBrown = TCODColor(71, 53, 46);

string rightAlignNumber(const int& input, const int& size)
  {
  char price_cstr[50];
  _snprintf_s(price_cstr, sizeof(price_cstr), "%d", input);
  string buffer(price_cstr);
  if (size == -1) // default
    return buffer;

  size_t sizet = (size_t)size;

  string returnval;
  if (buffer.size() < sizet)
  for (size_t counter = 0; counter < sizet - buffer.size(); counter++)
  {
      returnval += " ";
      if (returnval.size() > 100)
          cout << "?!"; // Sometimes this enters an infinite loop. Must fix.
  }

  if (buffer.size() > sizet)
    for (size_t counter = 0; counter < sizet; counter++)
      returnval += "x";
  else returnval += buffer.substr(0, sizet);
  return returnval;
  }

string rightAlignNumber(const double& input, const int& precision)
{
    char price_cstr[50];
    _snprintf_s(price_cstr, sizeof(price_cstr), ("%." + to_string(precision) + "f").c_str(), input);
    string buffer(price_cstr);

    return buffer;
}

std::string rightAlign(const string& input, const size_t& size)
  {
  string returnval;
  if(input.size() > size)
    {
    for (size_t counter = 0; counter < size; counter++)
      returnval += "x";
    return returnval;
    }
  if(input.size() < size)
    for (size_t counter = 0; counter < size - input.size(); counter++)
      returnval += " ";
  returnval += input.substr(0, size);
  return returnval;
  }

std::string leftAlign(const std::string& input, const size_t& size)
  {
  string returnval;
  returnval += input.substr(0, size);
  if(input.size() < size)
    for (size_t counter = 0; counter < size - input.size(); counter++)
      returnval += " ";
  return returnval;
  }



std::string changeToDecimal(const std::string& input)
  {
  regex rgx;
  rgx.assign("\\.\\d");
  if(!regex_search(input, rgx)) // If the regex wasn't matched -> if there is no decimal point in it.
    return input + string(".0");
  else
    return input;
  }

int stringToDecimal(const std::string& input)
  {
  int result = -555555;
  try {
    result = stoi(input);
    
    }
  catch (invalid_argument e)
    {
    cout << "<debug> Invalid argument in utility.cpp: " << e.what() << endl;
    }

  return result;
  }

void swapLineColors(TCODConsole* con, const int& line)
{
    if (line % 2)
        con->setDefaultForeground(TCODColor::lightestGreen);
    else con->setDefaultForeground(TCODColor::lightestBlue);
}

void drawPageDots(TCODConsole* console, const int& X, const int& Y, const int& picked, const size_t& length)
{
    if (picked < 0)
    {
        for (size_t c = 0; c < length; c++)
            console->putCharEx(X + c, Y, 7, TCODColor::white, TCODColor::black);
    }
    else
    {
        for (size_t c = 0; c < length; c++)
            console->putCharEx(X + c, Y, 7, TCODColor::lightGrey, TCODColor::black);
        console->putCharEx(X + picked, Y, 7, TCODColor::white, TCODColor::black);
    }
}

void invertLine(TCODConsole* console, const int& line)
{
    TCODColor fore = console->getCharBackground(1, line);
    TCODColor back = console->getCharForeground(1, line);
    for (int i = 1; i < console->getWidth() - 1; i++)
    {
        console->setCharBackground(i, line, back);
        console->setCharForeground(i, line, fore);
    }
}