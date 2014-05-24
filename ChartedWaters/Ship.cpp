#include "Ship.h"
#include "Fleet.h"
#include <algorithm>
#include <cmath>

using namespace std;

Ship::Ship()
  : storage(0), character(127), waveResistance(6), rations(50), sailors(5), fatigue(0), training(500), durability(0), movementcounter(0)
  , wrecked(false), invisible (false)
  {
  captain.faction = 0;
  captain.ducats = 1000;
  maxstorage = 212;
  turning = 0;
  }

Ship::Ship(const ShipPrototype& prototype)
  : storage(0), character(127), rations(50), sailors(5), fatigue(0), training(500), durability(0), movementcounter(0)
  , wrecked(false), invisible(false)
  {
  captain.faction = 0;
  captain.ducats = 1000;
  changeShip(prototype);
  }

void Ship::changeShip(const ShipPrototype& prototype)
  {
  typeID = prototype.typeID;
  typeName = prototype.typeName;
  specialization = prototype.specialization;
  desc = prototype.desc;
  size = prototype.size;

  if (size == std::string("small"))
    character = 9;
  else if (size == std::string("medium"))
    character = 127;
  else if (size == std::string("large"))
    character = 30;
  else character = 63;

  price = prototype.price;
  maxstorage = prototype.maxstorage;
  maxcargo = prototype.maxcargo;
  maxsailors = prototype.maxsailors;
  maxcannons = prototype.maxcannons;
  minimumsailors = prototype.minimumsailors;
  lateen = prototype.lateen;
  square = prototype.square;
  waveResistance = prototype.waveResistance;
  turning = prototype.turning;
  baseArmor = prototype.baseArmor;
  maxDurability = prototype.maxDurability;
  sailSlots = prototype.sailSlots;
  cannonSlots = prototype.cannonSlots;
  armorSlots = prototype.armorSlots;
  forecannonSlots = prototype.forecannonSlots;
  aftcannonSlots = prototype.aftcannonSlots;
  durability = maxDurability;
  }

string Ship::getName()
  {
  return shipName;
  }

string Ship::getType()
  {
  return typeName;
  }

void Ship::setName(const string& newName)
  {
  shipName = newName;
  shipName[0] = toupper(shipName[0]);
  }

bool Ship::addMoney(const int& amount)
  {
  if (amount + captain.ducats < 0)
    return false;
  else
    captain.ducats += amount;
  return true;
  }

void Ship::addItem(const Item& item, const int& numberOf, const int& averagePrice)
  {
    if (itemList.find(item.ID) != itemList.end())
    {
        itemList[item.ID].addItem(numberOf, averagePrice);
    }
    else
    {
        itemList[item.ID] = LedgerItem(item, numberOf, averagePrice);
    }

  storage += numberOf;
  }

int Ship::getMoney()
  {
  return captain.ducats;
  }

int Ship::getNumberOfItems(const std::string& ID)
  {
    if (itemList.find(ID) != itemList.end())
      return itemList[ID].howMany();
    return 0; // if it's not in the list.
  }

vector<LedgerItemTuple> Ship::returnListOfItems()
  {
  vector<LedgerItemTuple> returnVal;
  LedgerItemTuple buffer;
  for (auto it = itemList.begin(); it != itemList.end(); it++)
    {
    buffer.itemID = it->second.ID;
    buffer.ItemName = it->second.name;
    buffer.averagePurchasePrice = it->second.getAveragePrice();
    buffer.numberOfItems = it->second.howMany();
    returnVal.push_back(buffer);
    }

  return returnVal;
  }

bool Ship::removeItem(const std::string& ItemID, const int& numberOf)
  {
    if (itemList.find(ItemID) != itemList.end())
    {
        bool result = itemList[ItemID].removeItems(numberOf); // result returns false if the specified number of items could not be removed.
        if (itemList[ItemID].howMany() == 0)
            itemList.erase(ItemID);
        if (result)
            storage -= numberOf;
        return result;
    }
   return false;
  }

int Ship::getPurchasePriceOf(const std::string& ID)
  {
    if (itemList.find(ID) != itemList.end())
        return itemList[ID].getAveragePrice();
    else return 0;
  }

int Ship::getTotalGoods()
  {
  return (int)(storage + ceil((float)rations/10.0f));
  }

int Ship::getMaxGoods()
  {
  return maxcargo;
  }

void Ship::setPosition(const std::pair<int, int>& newPos)
  {
  position = newPos;
  }

double Ship::getArmorSlowing()
  {
  double armorSlowing = 0;
  for (auto it = armorList.begin(); it != armorList.end(); it++)
    armorSlowing += it->second.speed;
  return armorSlowing;
  }

double Ship::getSpeed()
  {
  double weight, crew, starved, trained, shipwrecked;
  crew = starved = trained = shipwrecked = 1;
  weight = (1-(double)getTotalStorageUsed()/getMaxStorage()) + 0.5; // if total == max, 0.5f. 
  weight = weight > 1 ? 1 : weight;
  double basespeed = baseSpeed_d(getLateen(), getSquare()) + getArmorSlowing();

  if (sailors < getMinSailors())
    {
    crew = pow((double)sailors / getMinSailors(), (double)1.0f/3.0f);
    crew = crew > 1 ? 1 : crew;
    }

  if (starving)
    starved = 0.8;
  trained = 1 + training / 1000.0f;

  if (wrecked)
    shipwrecked = 0;

  double result = weight * crew * basespeed * starved * shipwrecked;

  if (result < 0)
    return 0;
  else return result;
  }

pair<int, int> Ship::getPosition()
  {
  return position;
  }

void Ship::setPath(const std::vector<std::pair<int, int>>& ppath)
  {
  if (ppath.size() > 0)
    {
    path.clear();
    path.reserve(sizeof(pair<int, int>) * ppath.size());

    for (auto it = ppath.rbegin(); it < ppath.rend()-1; it++) // Skip the last, as it's 0,0.
      {
      path.push_back(*it);
      }
    }
  }

void Ship::updatePos()
  {
  if (path.size() > 0){
    setPosition(path.back());
    path.pop_back();
    }
  }

void Ship::step()
  {
  if (position != lastVisitedCityCoords)
    {
    starving = false;
    sailorsDied = 0;
    fatigue++;
    training += 5;
    training = training > 100 ? 100 : training;
    rations -=  sailors;
    if (rations < 0)
      {
      rations = 0;
      starving = true;
      }
    if (starving)
      fatigue+=100;
    if (fatigue >= 1000)
      {
      fatigue = 1000;
      sailors -= int(sailors * 0.1 + 0.5);
      sailors = sailors < 0 ? 0 : sailors;
      }
    if (durability <= 0)
      {
      durability = 0;
      wrecked = true;
      }
    }
  }

void Ship::fleetStep(Fleet* fleet)
{
    if (position != lastVisitedCityCoords)
    {
        starving = false;
        sailorsDied = 0;
        fatigue++;
        training += 5;
        training = training > 100 ? 100 : training;
        rations -= sailors;
        if (rations < 0)
        {
            int neededRations = -rations;
            rations = 0; 
            int fleetRations = fleet->takeRations(neededRations); // If there are not enough rations on board, use the fleet's rations.
            if (fleetRations < neededRations) // not enough rations
                starving = true;
            // otherwise, we have enough food for now.
        }
        if (starving)
            fatigue += 100;
        if (fatigue >= 1000)
        {
            fatigue = 1000;
            sailors -= int(sailors * 0.1 + 0.5);
            sailors = sailors < 1 ? 1 : sailors;
        }
        if (durability <= 0)
        {
            durability = 0;
            wrecked = true;
        }
    }
}

int Ship::getShipPrice()
  {
  return price;
  }

int Ship::getMaxCannons()
  {
  return maxcannons;
  }

int Ship::getCannons()
  {
  int counter = 0;
  for (auto it = cannonList.begin(); it != cannonList.end(); it++)
    counter += it->second.pairs * 2;
  return counter;
  }

int Ship::getTotalStorageUsed()
  {
  return storage + (int)ceil((float)rations/10.0f) + getCannons();
  }

int Ship::getMaxStorage()
  {
  return maxstorage;
  }

int Ship::getMinSailors()
  {
  return minimumsailors;
  }

int Ship::getMaxSailors()
  {
  return maxsailors;
  }

int Ship::getLateen()
  {
    return lateen + getAddedLateen();
  }

int Ship::getBaseLateen()
{
    return lateen;
}

int Ship::getAddedLateen()
{
    int addedLateen = 0;
    for (auto it = sailList.begin(); it != sailList.end(); it++)
        addedLateen += it->second.lateen;
    return addedLateen;
}

int Ship::getSquare()
{
    return square + getAddedSquare();
}

int Ship::getAddedSquare()
{
    int addedSquare = 0;
    for (auto it = sailList.begin(); it != sailList.end(); it++)
        addedSquare += it->second.square;
    return addedSquare;
}

int Ship::getBaseSquare()
{
    return square;
}

int Ship::getArmor()
  {

  return baseArmor + getAddedArmor();
  }

int Ship::getBaseArmor()
{
    return baseArmor;
}

int Ship::getAddedArmor()
{
    int addedArmor = 0;
    for (auto it = armorList.begin(); it != armorList.end(); it++)
        addedArmor += it->second.armor;
    return addedArmor;
}

int Ship::getWaveResistance()
  {
  return waveResistance;
  }

int Ship::getMaxDurability()
  {
  return maxDurability;
  }

std::string Ship::getDescription()
  {
  return desc;
  }

int Ship::getTurning()
{
    int addedTurning = getAddedTurning();
    if (addedTurning + turning < 1)
        return 1;
    else return addedTurning + turning;
}

int Ship::getBaseTurning()
{
    return turning;
}

int Ship::getAddedTurning()
{
    int addedTurning = 0;
    for (auto it = sailList.begin(); it != sailList.end(); it++)
        addedTurning += it->second.turning;
    return addedTurning;
}

string Ship::getSize()
  {
  return size;
  }

int Ship::getBaseSpeed()
  {
  return baseSpeed(lateen, square);
  }

int Ship::addSail(int pos, ShipSails& sail)
  {
  auto it = sailList.find(pos);
  if (it == sailList.end())
    {
    sailList[pos] = sail;
    return shipSUCCESS;
    }
  // since it already exists...
  ShipSails temp = it->second;
  sailList[pos] = sail;
  sail = temp;
  return shipSWAPPED;
  }

int Ship::removeSail(int pos, ShipSails& sail)
{
    auto it = sailList.find(pos);
    if (it == sailList.end())
    {
        return 0;
    }
    sail = it->second;
    sailList.erase(it);
    return shipSUCCESS;
}

int Ship::addArmor(int pos, ShipArmor& armor)
{
    auto it = armorList.find(pos);
    if (it == armorList.end())
    {
        armorList[pos] = armor;
        return shipSUCCESS;
    }
    // since it already exists...
    ShipArmor temp = it->second;
    armorList[pos] = armor;
    armor = temp;
    return shipSWAPPED;
}

int Ship::removeArmor(int pos, ShipArmor& armor)
{
    auto it = armorList.find(pos);
    if (it == armorList.end())
    {
        return 0;
    }
    armor = it->second;
    armorList.erase(it);
    return shipSUCCESS;
}

int Ship::addCannons(int pos, ShipCannons& cannons)
{
    auto it = cannonList.find(pos);
    if (it == cannonList.end())
    {
        cannonList[pos] = cannons;
        return shipSUCCESS;
    }
    // since it already exists...
    ShipCannons temp = it->second;
    cannonList[pos] = cannons;
    cannons = temp;
    return shipSWAPPED;
}

int Ship::removeCannons(int pos, ShipCannons& cannons)
{
    auto it = cannonList.find(pos);
    if (it == cannonList.end())
    {
        return 0;
    }
    cannons = it->second;
    cannonList.erase(it);
    return shipSUCCESS;
}


int Ship::getCannonSlots()
{
    return cannonSlots;
}

int Ship::getSailSlots()
{
    return sailSlots;
}

int Ship::getArmorSlots()
{
    return armorSlots;
}

void Ship::addSailors(const int& num, const int& addedtraining)
  {
    if (num > 0)
    {
        int totalFatigue = fatigue * sailors;
        int totalTraining = sailors * training;
        sailors += num;
        totalTraining += addedtraining * num;
        training = totalTraining / sailors;
        fatigue = totalFatigue / sailors;
    }
    else
    {
        if (sailors > -num)
            sailors += num; // Removing sailors doesn't affect the fatigue or training because those are averaged.
        else
            sailors = 0;
    }
  }

void Ship::removeSailors(const int& num)
  {
  sailors -= num;
  if (sailors < 0)
      sailors = 0;
  }

int Ship::getEstimatedRationsNeeded()
  {
  if (getSpeed() < 1)
    return 99999999;
  int daysNeeded = (int)(ceil(path.size() / floor((double)getSpeed())));
  return daysNeeded * sailors;
  }

int Ship::getETA()
  {
  if (getSpeed() < 1)
    return 99999999;
  return (int)(ceil(path.size() / floor((double)getSpeed())));;
  }

int Ship::getMovementCounters()
  {
  movementcounter += getSpeed();
  int used = (int)movementcounter;
  movementcounter -= used;
  return used;
  }