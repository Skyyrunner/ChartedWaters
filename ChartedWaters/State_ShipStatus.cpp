#include "State_ShipStatus.h"
#include "State_stringIn.h"
#include <SDL.h>
#include <string>
#include "utility.h"

using namespace std;

State_ShipStatus::State_ShipStatus(Fleet& fleet)
  : refToFleet(fleet), selector(5), redraw(false), showDebug(false)
  {
  console = new TCODConsole(64, 46);
  debug = new TCODConsole(64, 23);
  pages = getKeys(fleet.ships);
  page = pages[0];
  pageit = 0;
  }

State_ShipStatus::~State_ShipStatus()
  {
    
  }

string State_ShipStatus::header() // The one that says ID ... name ... price ... numberof
  {
  string returnval;
  string blank(" ");

  returnval += string("Item name                      Price #");
  return returnval;
  }

string State_ShipStatus::assembleOutput(const LedgerItemTuple& tuple)
  {
  /// i: item id, _: space, n:name, p: purchase price, m: how many
  /// ID
  /// iiii_n{30}_ppppp_xmmm~
  string blank(" ");

  string returnval;

  string purchaseprice = to_string(tuple.averagePurchasePrice);

  returnval += tuple.ItemName.substr(0, 30);
  if(tuple.ItemName.size() < 30)
    for (size_t counter = 0; counter < 30 - tuple.ItemName.size(); counter++)
      returnval += blank;

  returnval += blank;

  if (purchaseprice.size() > 5)
    returnval += string("xxxx");
  else returnval += purchaseprice.substr(0, 5);

  if (purchaseprice.size() < 5)
  for (size_t counter = 0; counter < 5 - purchaseprice.size(); counter++)
      returnval += blank;
  
  returnval += string(" x");
  returnval += tuple.numberOfItems;

  return returnval;
  }

void State_ShipStatus::printStats(TCODConsole* con, int& line)
  {
    Ship& current = refToFleet.ships[page];

  con->setColorControl(TCOD_COLCTRL_1, TCODColor::grey, TCODColor::black);
  con->setColorControl(TCOD_COLCTRL_2, TCODColor::red, TCODColor::black);
  con->setColorControl(TCOD_COLCTRL_3, TCODColor::lighterYellow, TCODColor::black);
  TCOD_colctrl_t typeToUse = (TCOD_colctrl_t)8;

  swapLineColors(con, line);
  con->print(1, line++, "Type : %s", current.getType().c_str()); swapLineColors(con, line);
  con->print(1, line++, "Size : %s", current.getSize().c_str()); swapLineColors(con, line);
  con->print(1, line++, "Total storage: %d", current.getMaxStorage()); swapLineColors(con, line);
  con->print(1, line++, "Goods: %d/%d    Sailors: %d%c(%d)%c/%d    Cannons: %d%c/%d", current.getTotalGoods(), current.getMaxGoods(),
      current.sailors, TCOD_COLCTRL_1, current.getMinSailors(), TCOD_COLCTRL_STOP, current.getMaxSailors(),
      current.getCannons(), TCOD_COLCTRL_1, current.getMaxCannons()); swapLineColors(con, line);

  con->print(1, line++, "Lateen sails: %d    Square sails: %d", current.getLateen(), current.getSquare()); swapLineColors(con, line);
  con->print(1, line++, "Speed: %.1f%c/%d.0", current.getSpeed(), TCOD_COLCTRL_1, current.getBaseSpeed()); swapLineColors(con, line);
  con->print(1, line++, "Wave resistance: %d", current.getWaveResistance()); swapLineColors(con, line);
  con->print(1, line++, "Turning: %d", current.getTurning()); swapLineColors(con, line);
  con->print(1, line++, "Armor: %d", current.getArmor()); swapLineColors(con, line);
  line++;

  if (current.fatigue > 900)
    typeToUse = (TCOD_colctrl_t)2;
  else if (current.fatigue > 500)
    typeToUse = (TCOD_colctrl_t)3;
  
  con->print(1, line++, "Fatigue: %c%.1f%c/100%c", typeToUse, current.fatigue / 10.0f, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  con->print(1, line++, "Training: %.1f%c/100%c", current.training / 10.0f, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);

  typeToUse = (TCOD_colctrl_t)8;
  if (current.rations < 50)
    typeToUse = (TCOD_colctrl_t)2;

  con->print(1, line++, "Rations: %c%.1f%c", typeToUse, current.rations / 10.0f, TCOD_COLCTRL_STOP);
  
  typeToUse = (TCOD_colctrl_t)8;
  if (current.durability / (double)current.getMaxDurability() < 0.1)
    typeToUse = (TCOD_colctrl_t)2;
  else if (current.durability / (double)current.getMaxDurability() < 0.5)
    typeToUse = (TCOD_colctrl_t)3;
  con->print(1, line++, "Durability: %c%d%c/%d", typeToUse, current.durability, TCOD_COLCTRL_STOP, current.getMaxDurability()); swapLineColors(con, line);
  line+=2;
  }

void State_ShipStatus::redrawList()
  {
    Ship& current = refToFleet.ships[page];

  console->clear();
  console->setDefaultForeground(TCODColor::white);
  console->setColorControl(TCOD_COLCTRL_1, TCODColor::silver, TCODColor::black);

  int line = 1;

  console->print(1, line++, ("The " + current.getType() + " " + current.getName()).c_str());
  line++;
  printStats(console, line);
  line++; // skip a line
  auto list = current.returnListOfItems();
  /// 
  console->setDefaultForeground(TCODColor::yellow);
  console->print(1, line++, header().c_str());
  console->setDefaultForeground(TCODColor::white);

  for (auto it = list.begin(); it < list.end(); it++)
    {
    swapLineColors(console, line);
    console->print(1, line++, assembleOutput(*it).c_str());
    }

  drawPageDots(console, 1, console->getHeight() - 2, pageit, pages.size());

  console->setDefaultForeground(MabinogiBrown);
  console->printFrame(0, 0, 64, 46, false);
  console->setDefaultForeground(TCODColor::white);
  console->print(20, 45, "Press D to view equipment");
  }

void State_ShipStatus::drawDebug()
{
    Ship& current = refToFleet.ships[page];

    debug->clear();
    int line = 1; 
    line = printEquipment(debug, current, line);    

    debug->setDefaultForeground(TCODColor(96, 71, 64));
    debug->printFrame(0, 0, 64, 23, false);
}

bool State_ShipStatus::Init()
  {
  console->clear();
  redrawList();
  drawDebug();

  return true;
  }
void State_ShipStatus::Update()
  {
  
  }

void State_ShipStatus::Render(TCODConsole *root)
{
    if (redraw)
    {
        redrawList();
        drawDebug();
        invertLine(console, selector);
        redraw = false;
    }
    TCODConsole::blit(console, 0, 0, 0, 0, root, root->getWidth() / 2 - 31, 1, 1.0f, 0.7f);
    if (showDebug)
        TCODConsole::blit(debug, 0, 0, 0, 0, root, root->getWidth() / 2 - 31, 24, 1.0f, 0.78f);
}
void State_ShipStatus::End()
  {
  delete console;
  delete debug;
  }
  //
void State_ShipStatus::Resize(int new_w,int new_h){}
void State_ShipStatus::WindowActive(){}
void State_ShipStatus::WindowInactive(){}
void State_ShipStatus::KeyUp(const int &key,const int &unicode){}
void State_ShipStatus::RecoverFromPush()
{
    if (newname != string(""))
    {
        refToFleet.ships[page].setName(newname);
        Init();
        newname.clear();
    }
    if (nextState)
        delete nextState;
}

void State_ShipStatus::KeyDown(const int &key, const int &unicode)
{
    switch (key)
    {
    case SDLK_ESCAPE:
        if (showDebug)
            showDebug = false;
        else
            popMe = true;
        break;
    case SDLK_r:
        newname.clear();
        nextState = new state_StringIn(30, newname);
        pushSomething = true;
        break;
    case SDLK_DOWN:
        if (selector < 60)
        {
            selector++;
            redraw = true;
        }
        break;
    case SDLK_UP:
        if (selector > 6)
        {
            selector--;
            redraw = true;
        }
        break;
    case SDLK_LEFT:
        if (pageit > 0)
        {
            pageit--;
            page = pages[pageit];
            redraw = true;
        }
        break;
    case SDLK_RIGHT:
        if ((size_t)pageit < pages.size() - 1)
        {
            pageit++;
            page = pages[pageit];
            redraw = true;
        }
        break;
    case SDLK_d:
        showDebug = !showDebug;
        break;
    default:
        break;
    }
}

void State_ShipStatus::MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
void State_ShipStatus::MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
void State_ShipStatus::MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}