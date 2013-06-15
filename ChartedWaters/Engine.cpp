#include "Engine.h"
#include "TCODTools.h"
#include "World.h"
#include "State_ShipStatus.h"
#include <string>

Engine CursesEngine;

int mouseX, mouseY;

World TheWorld(128,64);
TCODConsole* cityscreen;
TCODConsole* mapscreen;
TCODConsole* ZOCscreen;
TCODConsole* tooltip;
TCODConsole* PlayerShip;
TCODConsole* PathScreen;

bool redo = true;
typedef pair<int, int> coord;
GameState* newState;
bool mouseClick = false;
bool pressedPeriod = false;

bool Engine::EngineInit()
{
TheWorld.regen();
TheWorld.regen();

PlayerShip = new TCODConsole(1, 1);
auto shippos = TheWorld.getPlayerShip().getPosition();
PlayerShip->putCharEx(0, 0, TheWorld.getPlayerShip().character, TCODColor::red, TCODColor::black);

PathScreen = new TCODConsole(128, 64);
PathScreen->clear();
PathScreen->setKeyColor(TCODColor::magenta);
PathScreen->setDefaultBackground(TCODColor::magenta);

mapscreen = new TCODConsole(128, 64);
mapscreen->clear();
mapscreen->setKeyColor(TCODColor::magenta);
mapscreen->setDefaultBackground(TCODColor::magenta);

cityscreen = new TCODConsole(128, 64);
cityscreen->clear();
cityscreen->setKeyColor(TCODColor::magenta);
cityscreen->setDefaultBackground(TCODColor::magenta);

Renderer::getTerrainBitmap(mapscreen, TheWorld);
Renderer::getCityBitmap(cityscreen, TheWorld);

ZOCscreen = new TCODConsole(128, 64);
tooltip = new TCODConsole(30, 1);

Ship& ship = TheWorld.getPlayerShip();
ship.addMoney(13370);
ship.addItem(Item(3), 90, 500);
ship.addItem(Item(1), 52, 200);
ship.addItem(Item(2), 110, 300);
ship.addItem(Item(3), 10, 350);


	return true;
}
void Engine::Update()
{
if (!newState)
  delete newState;

if (redo)
  {
  TheWorld.regen();
  ZOCscreen->clear();
  cityscreen->clear();
  mapscreen->clear();
  redo = false;
  Renderer::getTerrainBitmap(mapscreen, TheWorld);
  Renderer::getCityBitmap(cityscreen, TheWorld);
  }

std::string name = Renderer::findCityName(coord(mouseX, mouseY), TheWorld);
if (name.size() > 0)
  name.front() -= 32; 
tooltip->clear();
tooltip->print(0, 0, name.c_str());

if (mouseClick)
  {
  mouseClick = false;
  auto it = TheWorld.pathfinder->path(TheWorld.getPlayerShip().getPosition(), coord(mouseX, mouseY), 6);
  TheWorld.getPlayerShip().setPath(it);
  PathScreen->clear();
  for (auto iterator = it.begin(); iterator < it.end(); iterator++)
    PathScreen->putCharEx(iterator->first, iterator->second, 251, TCODColor::yellow, TCODColor::black);
  }
if (pressedPeriod)
  {
  TheWorld.getPlayerShip().updatePos();
  pressedPeriod = false;
  }

// Write the tooltip!
//tooltip->clear();
//tooltip->print(0, 0, to_string((long double)worldmap->ref(mouseX, mouseY).owner).c_str());
}

void Engine::Render(TCODConsole *root)
{
root->setKeyColor(TCODColor::magenta);
TCODConsole::blit(mapscreen, 0, 0, 0, 0, root, 0, 0, 1.0f, 1.0f);
//TCODConsole::blit(ZOCscreen, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.0f);
TCODConsole::blit(cityscreen, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.5f);
TCODConsole::blit(tooltip, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.0f);
coord playerpos = TheWorld.getPlayerShip().getPosition();
TCODConsole::blit(PathScreen, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.0f);
TCODConsole::blit(PlayerShip, 0, 0, 0, 0, root, playerpos.first, playerpos.second, 1.0f, 0.0f);
}

void Engine::EngineEnd()
{
}

//
void Engine::Resize(int new_w,int new_h)
{
}

void Engine::WindowActive()
{
}

void Engine::WindowInactive()
{
}

void Engine::KeyUp(const int &key,const int &unicode)
{
}

void Engine::KeyDown(const int &key,const int &unicode)
{
if (key == SDLK_RIGHT)
  redo = true;
if (key == SDLK_s)
  {
  newState = new State_ShipStatus(TheWorld.getPlayerShip());
  PushState(newState);
  }
if (unicode == '.')
  {
  pressedPeriod = true;
  }
}

void Engine::MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
{
mouseX = iX / 12;
mouseY = iY / 12;
}

void Engine::MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
{

}

void Engine::MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
{
if (iButton == SDL_BUTTON_LEFT)
  mouseClick = true;

}
