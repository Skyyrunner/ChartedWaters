#include "town.h"
#include <cstdlib>
#include <limits>
#include <iostream>
#include <cmath>
#include "utility.h"

using namespace std;

Town::Town()
  :taxRate(0.10f), isLuxury(false), isOther(false), isIndustrial(false), isAgri(false), faction(0)
  {
  TownName = "null";
  spawncounter = rand()%30 + 1;

  }

Town::Town(const std::string& newName, const double& tax, const int& ffaction)
  : TownName(newName), taxRate(tax), faction(ffaction), isLuxury(false), isOther(false), isIndustrial(0), isAgri(0)
  {
  TownName[0] = toupper(TownName[0]);
  spawncounter = rand()%30 + 1;
  }

Town::~Town()
  {
  for each (ShipPart* part in partList)    
    if (!part)
      delete part;
    
  partList.clear();
  }

string Town::getName()
  {
  return TownName;
  }

vector<EconomyItemTuple> Town::returnListOfItems(bool isHometown)
  {
  vector<EconomyItemTuple> returnVal;
  double tax = getTaxRate(isHometown);
  for (auto it = itemlist.begin(); it != itemlist.end(); it++)
    {
    EconomyItemTuple buffer;
    double price;
    buffer.itemID = it->second.ID;
    buffer.ItemName = it->second.name;

    buffer.numberOfItems = it->second.howMany();
    price = getBuyPrice(it->second.ID) * (1 + tax);
    buffer.BuyPrice_s = rightAlignNumber(price);
    buffer.BuyPrice = price;

    price = getSellPrice(it->second.ID) * (1 - tax);
    buffer.SellPrice = price;
    buffer.SellPrice_s = rightAlignNumber(price);

    float percentage = (float)it->second.getPrice()/it->second.basePrice * 100;
    buffer.percentageOfBasePrice_s = to_string(percentage) + string("%%");
    buffer.fractionOfBasePrice = (double)it->second.getPrice() / it->second.basePrice;

    returnVal.push_back(buffer);
    }

  return returnVal;
  }

vector<AIEconomyItemTuple> Town::returnListOfItems_AI(bool isHometown)
  {
  vector<AIEconomyItemTuple> returnVal;
  double tax = getTaxRate(isHometown);
  for (auto it = itemlist.begin(); it != itemlist.end(); it++)
    {
    AIEconomyItemTuple buffer;
    buffer.itemID = it->second.ID;
    buffer.ItemName = it->second.name;

    buffer.numberofItems = it->second.howMany();

    buffer.BuyPrice = getBuyPrice(it->second.ID) * (1 + tax);
    buffer.SellPrice = getSellPrice(it->second.ID) * (1 - tax);

    int percentage = int((double)it->second.getPrice()/it->second.basePrice * 100);
    buffer.percentageOfBasePrice = percentage;

    returnVal.push_back(buffer);
    }

  return returnVal;
  }

void Town::addItems(const std::string& ID, const int& numberOf)
  {
  auto it = itemlist.find(ID);
  if (it != itemlist.end())
    {
    it->second.addItem(numberOf);
    return;
    }

  // since the item doesn't exist
  itemlist[ID] = EconomyItem(ID, numberOf, demandList[ID]);
  demandList.erase(ID);
  }

map<std::string, EconomyItem>::iterator Town::getItemIterator(const string& ID)
  {
  return itemlist.find(ID);
  }

int Town::buyItems(Ship& ship, const std::string& ID, int numberOf, bool hometown)
  {
  if (numberOf == 0)
    return 0;
  lastTransaction = 0;
  lastTransactionItemID.clear();
  auto it = itemlist.find(ID);

  if (it == itemlist.end())
    return twNO_SUCH_ITEM; // item doesn't exist!

  Skill& skill = ship.captain.skills[it->second.category];

  double currentTax = getTaxRate(hometown);
  int baseprice = int(getBuyPrice(it->second.ID) * (1 + currentTax));
  int actualprice = int(baseprice * (1 - skill.getLevel() / 100.0)); // Each level in the relevant skill equals a 1% discount.
  if (numberOf < 0)
  {
      if (actualprice == 0)
      {
          numberOf = 0;
          cout << "divbyzero";
      }
      else
      {
          numberOf = ship.getMoney() / actualprice; // FLOOR ( money / price ) = number of items buyable.
          numberOf = numberOf > it->second.howMany() ? it->second.howMany() : numberOf; // If the possible number of items buyable is more than the total.
      }
  }
  int price = numberOf * actualprice;

  if (price > ship.getMoney())
    return twNOT_ENOUGH_MONEY; // don't have money to buy all that
  if (numberOf > it->second.howMany())
    return twNOT_ENOUGH_ITEMS; // not enough items to buy!

  /// Finally, actually buy them items.
  ship.addItem(Item(ID), numberOf, int(getBuyPrice(ID) * (1 + currentTax)));
  ship.addMoney(-price);
  it->second.addItem(-numberOf);
  it->second.addDemand(numberOf);
  lastTransaction = price;
  numberOfLastTransaction = numberOf;
  unitPurchasePriceOfSell = 0;
  lastTransactionItemID = ID;
  return twSUCCESS;
  }

int Town::buyItems(Fleet& fleet, const std::string& ID, int numberOf, int target, bool hometown)
{
    if (numberOf == 0)
        return 0;

    lastTransaction = 0;
    lastTransactionItemID.clear();
    auto it = itemlist.find(ID);

    if (it == itemlist.end())
        return twNO_SUCH_ITEM; // item doesn't exist!

    if (target >= 0 && fleet.ships.find(target) == fleet.ships.end())
        throw std::out_of_range(("Ship number " + to_string(target) + " does not exist.").c_str());

    int actualprice = getActualBuyPrice(ID, hometown, fleet.captain);
    if (numberOf < 0)
    {
        numberOf = fleet.getMoney() / actualprice; // FLOOR ( money / price ) = number of items buyable.
        numberOf = numberOf > it->second.howMany() ? it->second.howMany() : numberOf; // If the possible number of items buyable is more than the total.
    }
    int price = numberOf * actualprice;

    if (price > fleet.getMoney())
        return twNOT_ENOUGH_MONEY; // don't have money to buy all that
    if (numberOf > it->second.howMany())
        return twNOT_ENOUGH_ITEMS; // not enough items to buy!

    /// Finally, actually buy them items.
    if (target < 0)
        fleet.addItem(Item(ID), numberOf, actualprice);
    else
        fleet.ships[target].addItem(Item(ID), numberOf, actualprice);
    fleet.addMoney(-price);
    it->second.addItem(-numberOf);
    it->second.addDemand(numberOf);
    lastTransaction = price;
    numberOfLastTransaction = numberOf;
    unitPurchasePriceOfSell = 0;
    lastTransactionItemID = ID;
    return twSUCCESS;
}

int Town::sellItems(Ship& ship, const std::string& ID, int numberOf, bool hometown)
  {
  lastTransaction = 0;
  if (numberOf == 0)
    return 0;
  if (numberOf < 0)
    numberOf = ship.getNumberOfItems(ID);
  lastTransactionItemID.clear();
  // Set tax rate to 0 if it's hometown.
  Skill& skill = ship.captain.skills[ItemDict.findItemCategory(ID)];

  double currentTax = getTaxRate(hometown);
  unitPurchasePriceOfSell = ship.getPurchasePriceOf(ID);
  bool success = ship.removeItem(ID, numberOf);
  if (!success)
    return twNOT_ENOUGH_ITEMS;
  int earned = int(numberOf * getSellPrice(ID, 1.0 + skill.getLevel() / 100.0) * (1 - currentTax)); // Each level in a skill increases price by 1%
  addItems(ID, numberOf);
  ship.addMoney(earned);
  lastTransaction = earned;

  // Add training points equal to profit. If less than 0, add nothing.
  int profit = earned - unitPurchasePriceOfSell * numberOf;
  if (profit < 0) profit = 0;
  skill.train(profit);
  cout << "Debug: Trained skill " << skill.name << " by " << profit << ", level " << skill.getLevel() <<", progress " << skill.getValue() << "\n";


  numberOfLastTransaction = numberOf;
  lastTransactionItemID = ID;
  
  return twSUCCESS;
  }

int Town::sellItems(Fleet& fleet, const std::string& ID, int numberOf, int target, bool hometown)
{
    lastTransaction = 0;
    if (numberOf == 0)
        return 0;

    if (numberOf < 0)
        numberOf = fleet.getNumberOfItems(ID);

    if (target >= 0 && fleet.ships.find(target) == fleet.ships.end())
        throw std::out_of_range(("Ship number " + to_string(target) + " does not exist.").c_str());

    lastTransactionItemID.clear();
    // Set tax rate to 0 if it's hometown.
    Skill& skill = fleet.captain.skills[ItemDict.findItemCategory(ID)];

    double currentTax = getTaxRate(hometown);

    if (target < 0)
        unitPurchasePriceOfSell = fleet.getPurchasePriceOf(ID);
    else
        unitPurchasePriceOfSell = fleet.ships[target].getPurchasePriceOf(ID);

    bool success = false;
    if (target < 0)
        success = fleet.removeItem(ID, numberOf);
    else
        success = fleet.ships[target].removeItem(ID, numberOf);

    if (!success)
        return twNOT_ENOUGH_ITEMS;
    int earned = int(numberOf * getSellPrice(ID, 1.0 + skill.getLevel() / 100.0) * (1 - currentTax)); // Each level in a skill increases price by 1%
    addItems(ID, numberOf);
    fleet.addMoney(earned);
    lastTransaction = earned;

    // Add training points equal to profit. If less than 0, add nothing.
    int profit = earned - unitPurchasePriceOfSell * numberOf;
    if (profit < 0) profit = 0;
    skill.train(profit);
    cout << "Debug: Trained skill " << skill.name << " by " << profit << ", level " << skill.getLevel() << ", progress " << skill.getValue() << "\n";


    numberOfLastTransaction = numberOf;
    lastTransactionItemID = ID;

    return twSUCCESS;
}

int Town::buyRations(Ship& ship, const int& number)
  {
  const int rationPrice = 50;
  if (ship.captain.ducats < number * rationPrice)
    return -1;
  ship.rations += 10 * number;
  ship.captain.ducats -= 50 * number;
  return 0;
  }

int Town::getCost(const int& amount)
  {
  switch (amount)
      {
    case 1:
      return 200;
      break;
    case 20:
      return 3200;
      break;
    case 50:
      return 6400;
      break;
    default:
      return int(amount * pow(0.95f, amount/20));
      break;
      }
  }

int Town::recoverFatigue(Ship& ship, const int& number)
  {
    if (number < 0)
        return twNOT_ENOUGH_MONEY;
    if (getCost(number) > ship.captain.ducats)
        return twNOT_ENOUGH_MONEY;

    ship.fatigue -= number * 10;
    ship.captain.ducats -= getCost(number);
    if (ship.fatigue < 0)
        ship.fatigue = 0;
    return twSUCCESS;
  }

int Town::recoverFatigue(Fleet& fleet, const int& number, int index)
{
    if (number < 0)
        return twNOT_ENOUGH_MONEY;
    if (getCost(number) > fleet.captain.ducats)
        return twNOT_ENOUGH_MONEY;
    if (index == -1)
        fleet.removeFatigue(number * 10 / (int)fleet.ships.size());
    else
    {
        fleet.ships[index].fatigue -= 10 * number;
        if (fleet.ships[index].fatigue < 0)
            fleet.ships[index].fatigue = 0;
    }
    fleet.captain.ducats -= getCost(number);
    return twSUCCESS;
}

double Town::getDistanceFromNearestSource(const std::string& ID)
  {
  auto list = ItemDict.getCitiesForItem(ID);
  if (list.size() == 0)
    return 99999; // max :D
  double distance = numeric_limits<double>::max(); // Squared at first.
  for (auto it = list.begin()+1; it < list.end(); it++)
    {
    double potentialDistance = pow((double)myPosition.first - it->first, 2) + pow((double)myPosition.second - it->second, 2);
    if (potentialDistance <= distance)
      distance = potentialDistance;
    }

  return sqrt(distance);
  }

int Town::getPriceOf(const std::string& ID)
  {
  auto itemIt = itemlist.find(ID);
  if (itemIt != itemlist.end())
    return itemIt->second.getPrice();

  // If it doesn't exist, return the base price influenced by demand
  // and the items of the same type or category already in the city.
  int supply = 0;
  int demand = 0;
  auto item = ItemDict.getItemTemplate(ID);
  for (auto it = itemlist.begin(); it != itemlist.end(); it++)
    {
    if (it->second.category == item.category)
      {
      supply += 0.01 * it->second.getSupply();
      demand += 0.01 * it->second.getDemand();
      }
    if (it->second.type == item.type)
      {
      supply += 0.05 * it->second.getSupply();
      demand += 0.05 * it->second.getDemand();
      }
    }
  EconomyItem temp(ID, supply, 1 + demand + demandList[ID]);
  return temp.getPrice();
  } 

// Must consider: (1) if it's produced in that city (2) supply vs demand for said item (3) if the city is in a 'zone' (4) closest city that produces said good
double Town::getSellPrice(const std::string& ID, const double& bonus)
  {
  double producedHere = 1.0f;
  double produceZone = 1.0f;
  double distanceMult = 1.0f;

  // Produced here -> half
  auto spawnlistIt = spawnList.begin();
  while (spawnlistIt < spawnList.end() && *spawnlistIt != ID)
    spawnlistIt++;
  if (spawnlistIt != spawnList.end()) 
    producedHere = 0.5f;
  
  // Is in a zone -> 90%
  string category = ItemDict.findItemCategory(ID);

  if (isAgri && category == string("Food"))
    produceZone = 0.9f;
  else if (isIndustrial && category == string("Raw materials"))
    produceZone = 0.9f;
  else if (isOther && category == string("Other"))
    produceZone = 0.9f;
  else if (isLuxury && category == string("Luxury"))
    produceZone = 0.9f;

  // Distance bonus
  double distance = getDistanceFromNearestSource(ID);
  distance = distance > 200 ? 200 : distance;
  distance = distance < 0 ? 0 : distance;
  distanceMult = distance / 150 + 0.8f;

  return getPriceOf(ID) * distanceMult * produceZone * producedHere * bonus;
  }

double Town::getBuyPrice(const std::string& ID)
  {
  // If not sold here, return -1
  auto itemIt = itemlist.find(ID);
  if (itemIt == itemlist.end())
    return -1;

  double produceZone = 1.0f;
  double distanceMult = 1.0f;
  
  // Is in a zone -> 90%
  string category = ItemDict.findItemCategory(ID);

  if (isAgri && category == string("Food"))
    produceZone = 0.9f;
  else if (isIndustrial && category == string("Raw materials"))
    produceZone = 0.9f;
  else if (isOther && category == string("Other"))
    produceZone = 0.9f;
  else if (isLuxury && category == string("Luxury"))
    produceZone = 0.9f;

  // Distance bonus
  
  double distance = getDistanceFromNearestSource(ID);
  if (getNumberOf(ID) == -1)
    distanceMult = 1;
  else
    {
    distance = distance > 200 ? 200 : distance;
    distance = distance < 0 ? 0 : distance;
    distanceMult = distance / 80 + 0.7f;
    }

  return getPriceOf(ID) * distanceMult * produceZone;
  }

double Town::getActualBuyPrice(const std::string& ID, bool isHome, Player& player)
{
    int base = getBuyPrice(ID);
    double tax = getTaxRate(isHome);
    auto it = itemlist.find(ID);
    if (it == itemlist.end())
        return -1; // item doesn't exist!

    Skill& skill = player.skills[it->second.category];
    double discount = 1 - skill.getLevel() / 100.0;  // Each level in the relevant skill equals a 1% discount.

    return base * (1 + tax) * discount;
}

double Town::getTaxRate(bool hometown)
  {
  if (hometown)
    return 0.06;
  return taxRate;
  }

void Town::setTaxRate(const double& newTaxRate)
  {
  taxRate = newTaxRate;
  }

int Town::getFactionID()
  {
  return faction;
  }

int Town::getNumberOf(const std::string& itemID)
  {
  auto it = itemlist.find(itemID);
  if (it != itemlist.end())
    return it->second.howMany();
  else return -1;
  }

void Town::addDemandToItem(const std::string& itemID, const int& add)
  {
  auto it = itemlist.find(itemID);
  if (it != itemlist.end())
    it->second.addDemand(add);
  // since by this point we haven't found it in the list.
  demandList[itemID] += add;
  }

void Town::spawnItems()
  {
  for (auto it = spawnList.begin(); it < spawnList.end(); it++)
    {
    if ((*it)[0] == 'l') // If it's a luxury item.
      addItems(*it, population + 500);
    else addItems(*it, rand()%200 + population * 2 + 1000);
    }
  }

void Town::step()
  {
  spawncounter++;
  for (auto it = spawnList.begin(); it < spawnList.end(); it++)
    {
    if ((*it)[0] == 'l') // If it's a luxury item.
      {if (getNumberOf(*it) < 8192)
        addItems(*it, int(population * 0.05));}
    else 
      if (getNumberOf(*it) < 8192)
        addItems(*it, int(population * 0.1));
    }

  // Remove items by demand & if there is less than one of that item. 
  for (auto it = itemlist.begin(); it != itemlist.end(); it++)
    {
    int toRemove = int(it->second.getDemand() * 0.1) + 1;
    it->second.addItem(toRemove * -1);
    it->second.addDemand(int(toRemove * -0.5f));
    }

  vector<string> toRemove;
  for (auto it = itemlist.begin(); it != itemlist.end(); it++)
    {
    if (it->second.howMany() <= 0)
      toRemove.push_back(it->first);
    }
  for (auto it = toRemove.begin(); it < toRemove.end(); it++)
    itemlist.erase(*it);
  toRemove.clear();

  for (auto it = demandList.begin(); it != demandList.end(); it++)
    {
    it->second *= 0.9f;
    if (it->second <= 0)
      toRemove.push_back(it->first);
    }
  for (auto it = toRemove.begin(); it < toRemove.end(); it++)
    demandList.erase(*it);

  // Randomly spawn demand
  if (spawncounter > 30)
    {
    spawncounter = 0;
    map<string, vector<string>>& itemlist = ItemDict.itemsPerCategory;
    for (int counter = 0; counter < 5; counter++) // make 5 items in demand.
      {
      int first = rand()%(itemlist.size()-1);
      auto outerIt = itemlist.begin();
      for (int counter = 0; counter < first; counter++)
        outerIt++;
      int position = rand()%(outerIt->second.size());
      string ID = outerIt->second.at(position);
      auto item = ItemDict.getItemTemplate(ID);
      auto category = itemlist.find(item.category);

      addDemandToItem(item.ID, 200);
      //cout << "Spawned demand for " << item.ID << " in city " << TownName << endl;
      for (auto it = category->second.begin(); it < category->second.end(); it++)
        {
        addDemandToItem(*it, 50);
        if (ItemDict.findItemType(*it) == item.type)
          addDemandToItem(*it, 100);
        }      
      }
    }
  }

