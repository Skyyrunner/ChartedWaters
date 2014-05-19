#include "goods.h"
#include <cmath>
#include <fstream>
#include "json/json.h"

ItemDictionary ItemDict;
using namespace std;

///////////////////////
//////////
//////////  JSONToItem
//////////
///////////////////////

void JSONToItem::readItems(ItemDictionary& dict)
  {
  string index;
  Json::Value root;
  Json::Reader reader;
  index = slurp(string("resources/index.json"));
  bool parsingSuccess = reader.parse(index, root);
  if (!parsingSuccess)
    {
    cout << "Didn't succeed: " << reader.getFormattedErrorMessages();
    cin.ignore();
    return;
    }
  int counter = 0;

  Json::Value list = root["files"];
  while (!list[counter].isNull())
    {
    slurpItems(string("resources/") + list[counter].asString(), dict);
    cout << endl;
    counter++;
    }

  dict.sortIntoLists();
  }

void JSONToItem::readItems()
  {
  string index;
  Json::Value root;
  Json::Reader reader;
  index = slurp(string("resources/index.json"));
  bool parsingSuccess = reader.parse(index, root);
  if (!parsingSuccess)
    {
    cout << "Didn't succeed: " << reader.getFormattedErrorMessages();
    cin.ignore();
    return;
    }
  int counter = 0;

  Json::Value list = root["files"];
  while (!list[counter].isNull())
    {
    slurpItemsAndPrint(string("resources/") + list[counter].asString());
    cout << endl;
    counter++;
    }
  }

void JSONToItem::slurpItems(const std::string& filename, ItemDictionary& dict)
  {
  string itemlist;
  Json::Value root;
  Json::Reader reader;
  itemlist = slurp(filename);
  bool parsingSuccess = reader.parse(itemlist, root);
  if (!parsingSuccess)
    {
    cout << "Didn't succeed: " << reader.getFormattedErrorMessages();
    cin.ignore();
    return;
    }
  int counter = 0;

  while (!root[counter].isNull())
    {
    Json::Value item = root[counter];
    Item buffer;
    buffer.ID = item["ID"].asString();
    buffer.name = item["name"].asString();
    buffer.basePrice = item["base price"].asInt();
    if (!item["desc"].isNull())
      buffer.desc = item["desc"].asString();
    buffer.type = item["type"].asString();
    buffer.category = item["category"].asString();
    dict.ItemList[buffer.ID] = buffer;
 
    counter++;
    }
  }

void JSONToItem::slurpItemsAndPrint(const std::string& filename)
  {
  string itemlist;
  Json::Value root;
  Json::Reader reader;
  itemlist = slurp(filename);
  bool parsingSuccess = reader.parse(itemlist, root);
  if (!parsingSuccess)
    {
    cout << "Didn't succeed: " << reader.getFormattedErrorMessages();
    cin.ignore();
    return;
    }
  int counter = 0;
  cout << "In " << filename << "---------------------------\n";
  while (!root[counter].isNull())
    {
    Json::Value item = root[counter];
    
    cout << item["name"].asString() << " (" << item["ID"].asString() << ")";
    if (!item["desc"].isNull())
      cout << " : " << item["desc"].asString();

    cout << endl;
    counter++;
    }

  }

string JSONToItem::slurp(const string& filename)
  {
  fstream file(filename.c_str());
  string output; output.clear();
  string buffer; buffer.clear();
  while (!file.eof())
    {
    getline(file, buffer);
    output.append(buffer + string("\n"));
    }
  return output;
  }


///////////////////////
//////////
//////////  ItemDictionary
//////////
///////////////////////

ItemDictionary::ItemDictionary()
  : sorted(false)
  {
  vector<string> types;
  types.push_back(string("Alcohol"));
  types.push_back(string("Foodstuffs"));
  types.push_back(string("Seasonings"));
  types.push_back(string("Livestock"));
  types.push_back(string("Luxury food"));
  categories[string("Food")] = types;
  types.clear();

  types.push_back(string("Fibers"));
  types.push_back(string("Fabric"));
  types.push_back(string("Dyes"));
  //types.push_back(string("Ores"));
  types.push_back(string("Industrial goods"));
  categories[string("Raw materials")] = types;
  types.clear();

  types.push_back(string("Medicine"));
  types.push_back(string("Sundries"));
  types.push_back(string("Weapons"));
  types.push_back(string("Firearms"));
  types.push_back(string("Crafts"));
  categories[string("Other")] = types;
  types.clear();

  types.push_back(string("Artwork"));
  types.push_back(string("Spices"));
  types.push_back(string("Precious metals"));
  types.push_back(string("Fragrances"));
  types.push_back(string("Jewellery"));
  types.push_back(string("Precious stones"));
  categories[string("Luxury")] = types;

  Item buffer;
  buffer.ID = string("null");
  buffer.category = string("null");
  buffer.basePrice = 0;
  buffer.decayRateNegative = buffer.decayRatePositive = 0;
  buffer.desc = string("No description.");
  buffer.name = string("invalid item");
  ItemList[string("null")] = buffer;

  ///////// now let's do the initials stuff
  InitialList[string("Alcohol")] = string("Alc");
  InitialList[string("Foodstuffs")] = string("Fds");
  InitialList[string("Seasonings")] = string("Sea");
  InitialList[string("Livestock")] = string("Liv");
  InitialList[string("Luxury food")] = string("Lux");
  InitialList[string("Fiber")] = string("Fib");
  InitialList[string("Fabric")] = string("Fab");
  InitialList[string("Dyes")] = string("Dye");
  InitialList[string("Industrial goods")] = string("Ind");
  InitialList[string("Medicine")] = string("Med");
  InitialList[string("Sundries")] = string("Etc");
  InitialList[string("Weapons")] = string("Wea");
  InitialList[string("Firearms")] = string("Fir");
  InitialList[string("Crafts")] = string("Cra");
  InitialList[string("Artwork")] = string("Art");
  InitialList[string("Spices")] = string("Spi");
  InitialList[string("Precious metals")] = string("P.M");
  InitialList[string("Fragrances")] = string("Fra");
  InitialList[string("Jewellery")] = string("Jwl");
  InitialList[string("Precious stones")] = string("P.S");
  InitialList[string("null")] = string("!!!");
  }

string ItemDictionary::findItemName(const std::string& ID)
  {
  auto it = ItemList.find(ID);
  if (it == ItemList.end())
    return ItemList[string("null")].name;
  else
    return it->second.name;
  }

string ItemDictionary::findItemTypeInitials(const std::string& ID)
  {
  auto temp = ItemList.find(ID);
  if (temp == ItemList.end())
    return string("nul");
  string& type = temp->second.type;
  auto it = InitialList.find(type);
  if (it == InitialList.end())
    return string("nul");
  return it->second;
  }

string ItemDictionary::findItemType(const std::string& ID)
  {
  auto temp = ItemList.find(ID);
  if (temp == ItemList.end())
    return string("null");
  return temp->second.type;
  }

int ItemDictionary::findBasePrice(const std::string& ID)
  {
  auto it = ItemList.find(ID);
  if (it == ItemList.end())
    return ItemList[string("null")].basePrice;
  else
    return it->second.basePrice;
  }

string ItemDictionary::findItemCategory(const string& ID)
  {
  auto it = ItemList.find(ID);
  if (it == ItemList.end())
    return ItemList[string("null")].category;
  else
    return it->second.category;
  }

pair<double, double> ItemDictionary::findDecayRates(const std::string& ID)
  {
  auto it = ItemList.find(ID);
  if (it == ItemList.end())
    return pair<double, double>(ItemList[string("null")].decayRateNegative, ItemList[string("null")].decayRatePositive);
  else
    return pair<double,double>(it->second.decayRateNegative, it->second.decayRatePositive);
  }

Item& ItemDictionary::getItemTemplate(const string& ID)
  {
  auto it = ItemList.find(ID);
  if (it == ItemList.end())
    return ItemList[string("null")];
  else
    return it->second;
  }

void ItemDictionary::sortIntoLists()
  {
  // Clear existing.
  itemsPerCategory.clear();
  for (auto it = categories.begin(); it != categories.end(); it++) // Make buffers for each.
    {
    itemsPerCategory[it->first] = vector<string>();
    }
  for (auto it = ItemList.begin(); it != ItemList.end(); it++)
    {
    itemsPerCategory[it->second.category].push_back(it->second.ID);
    }
  sorted = true;
  }

vector<string>& ItemDictionary::getItemsPerCategory(const string& category)
  {
  if (!sorted)
    sortIntoLists();
  return itemsPerCategory.find(category)->second;
  }

void ItemDictionary::addCityToItem(const string& itemID, const pair<int,int>& city)
  {
  citiesPerItem[itemID].push_back(city);
  }

vector<pair<int,int>> ItemDictionary::getCitiesForItem(const string& itemID)
  {
  return citiesPerItem[itemID]; // this implicitly creates an empty vector if the key didn't exist before the function call.
  }

void ItemDictionary::clearCitiesList()
  {
  citiesPerItem.clear();
  }

std::vector<std::string> ItemDictionary::getCategories()
{
    vector<string> keys;
    for (auto it = categories.begin(); it != categories.end(); it++)
        keys.push_back(it->first);
    return keys;
}



///////////////////////
//////////
//////////  Item
//////////
///////////////////////

Item::Item()
  : name(string("Null")), basePrice(0), decayRatePositive(0), decayRateNegative(0)
  {
  }

Item::Item(const Item& item)
  :ID(item.ID),name(item.name), desc(item.desc), basePrice(item.basePrice), type(item.type), category(item.category)
  {

  }

Item::Item(const std::string& newID)
  :ID(newID)
  {
  auto item = ItemDict.getItemTemplate(newID);
  name = item.name;
  desc = item.desc;
  basePrice = item.basePrice;
  }


bool Item::operator==(const Item& right) const
  {
  if (right.ID == ID)
    return true;
  else return false;
  }

bool Item::operator<(const Item& right) const
  {
  if (name < right.name)
    return true;
  else return false;
  }


///////////////////////
//////////
//////////  EconomyItem
//////////
///////////////////////

EconomyItem::EconomyItem(const std::string& ID, const int& newSupply, const int& newDemand)
  : Item(ID), supply(newSupply), demand(newDemand)
  {

  }

EconomyItem::EconomyItem()
  : Item(string("null")), supply(5), demand(5)
  {

  }

void EconomyItem::decayDemand()
  {
  if (demand > supply)
    demand = (int)(demand * (1 - decayRatePositive));
  else if (demand < supply)
      demand = (int)(demand * (1 + decayRatePositive));
  }

int EconomyItem::getPrice()
  {
  const double VERTICAL_SHIFT = 0.5f;
  const double HORIZONTAL_SHIFT = 500.0f;
  const double HORIZONTAL_SCALE = 0.01f;
  const double PRICE_RANGE = 1.0f;
  double adjustedDemand = (1500 + 500 * 0.5 * log(getSupply() / (double)demand)) / 3.0f;
  double multiplier = VERTICAL_SHIFT + PRICE_RANGE / (1 + exp(HORIZONTAL_SCALE * (adjustedDemand - HORIZONTAL_SHIFT)));
  return (int)(multiplier * basePrice);
  }

int EconomyItem::howMany()
  {
  return supply;
  }

void EconomyItem::addItem(const int& howMany)
  {
  supply += howMany;
  }

void EconomyItem::addDemand(const int& howMuch)
  {
  demand += howMuch;
  if (demand <= 0)
    demand = 1;
  }

int EconomyItem::getDemand()
  {
  return demand;
  }

int EconomyItem::getSupply()
  {
  if (supply < 100)
    return 100;
  return supply;
  }

///////////////////////
//////////
//////////  LedgerItem
//////////
///////////////////////

LedgerItem::LedgerItem()
:Item("null"), totalItems(0)
{

}

LedgerItem::LedgerItem(const std::string& ID)
  :Item(ID), totalItems(0)
  {

  }

LedgerItem::LedgerItem(const Item& item, const int& howMany, const int& averagePrice)
  :Item(item), totalItems(howMany)
  {
  ledger.push_back(pair<int, int>(howMany, averagePrice));
  }

void LedgerItem::addItem(const int& howMany, const int& averagePrice)
  {
  if (howMany == 0)
    return;
  ledger.push_back(pair<int, int>(howMany, averagePrice));
  totalItems += howMany;
  }

bool LedgerItem::removeItems(const int& howMany)
  {
  if (howMany == 0)
    return true;
  if (totalItems < howMany)
    return false;
  int counter = howMany;
  for (auto it = ledger.rbegin(); it < ledger.rend(); it++)
    {
    if (counter - it->first <= 0) // If the contents of it can handle the need.
      {
      it->first -= counter;
      break;
      }
    else
      {
      counter -= it->first;
      it->first = 0;
      }
    }

  // Finally, pop the back members that are 0.
  while (ledger.size() != 0 && ledger.at(ledger.size()-1).first == 0)
    ledger.pop_back();
  totalItems -= howMany;
  return true;
  }

int LedgerItem::getAveragePrice()
  {
  int total = 0;
  for (auto it = ledger.begin(); it < ledger.end(); it++)
    {
    total += it->first * it->second;
    }
  return total / (int)totalItems;
  }

void LedgerItem::countItems()
  {
  int total = 0;
  for (auto it = ledger.begin(); it < ledger.end(); it++)
    {
    total += it->first;
    }
  totalItems = total;
  }

int LedgerItem::howMany()
  {
  return totalItems;
  }