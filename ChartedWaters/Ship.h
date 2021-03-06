#pragma once
#include <string>
#include "goods.h"
#include "shipPrototype.h"
#include "shipparts.h"
#include "player.h"

struct LedgerItemTuple
  {
  std::string itemID;
  std::string ItemName;
  int numberOfItems;
  int averagePurchasePrice;
  };

// success: delete original. swapped: don't delete original.
enum ShipErrors {shipSUCCESS = 1, shipSWAPPED = 2, shipNOTENOUGHSPACE};

class Fleet; // forward declare class, then include it in the cpp file.
class Ship : protected ShipPrototype
  {
  public:
    Ship();
    Ship(const ShipPrototype& prototype);
    void changeShip(const ShipPrototype& prototype);
    void returnParts(Player& player); // unequip all ship parts and return to inventory
    Player captain;

    void setName(const std::string& newName);
    std::string getName();
    std::string getType();
    bool addMoney(const int& amount); // false if the total money would go under 0.
    void addItem(const Item& item, const int& numberOf, const int& averagePrice);
    bool removeItem(const std::string& ItemID, const int& numberOf); // return false in case there are not enough items to remove.

    int getEstimatedRationsNeeded();
    int getETA();
    int getNumberOfItems(const std::string& ID);
    int getPurchasePriceOf(const std::string& ID);
    int getShipPrice();
    int getMoney();
    int getTotalGoods();
    int getMaxGoods();
    int getTotalStorageUsed(); // cannons + sailors + goods
    int getMaxStorage();
    int getMaxCannons();
    int getCannons();
    int getMinSailors();
    int getMaxSailors();
    int getWaveResistance();
    int getMaxDurability();

    int getArmor();
    int getBaseArmor();
    int getAddedArmor();

    int getLateen();
    int getBaseLateen();
    int getAddedLateen(); // sum up item effects
    int getSquare();
    int getBaseSquare();
    int getAddedSquare(); // same
    double getSpeed(); // Finds the lowest speed of the entire fleet.
    int getBaseSpeed();
    int getMovementCounters(); // This one keeps the decimals. Used for movement, not stat display. Alters the class.
    std::string getDescription();

    int getTurning();
    int getBaseTurning();
    int getAddedTurning(); // Should be negative.

    std::string getSize();


    std::vector<LedgerItemTuple> returnListOfItems();
    
    void setPosition(const std::pair<int, int>& newPos);
    void setPath(const std::vector<std::pair<int, int>>& ppath);
    void updatePos(); // move 1 step along the path
    virtual void step(); // run the simulations for 1 day
    virtual void fleetStep(Fleet* fleet); // Utilizing the fleet's resources, run simulations for 1 day.

    int addSail(int pos, ShipSails& sail);
    int removeSail(int pos, ShipSails& sail);
    int addArmor(int pos, ShipArmor& armor);
    int removeArmor(int pos, ShipArmor& armor);
    int addCannons(int pos, ShipCannons& cannons);
    int removeCannons(int pos, ShipCannons& cannons);

    int addFigurehead(ShipArmor& figurehead);
    void addSailors(const int& num, const int& addedtraining);
    void removeSailors(const int& num);
    int getSailSlots();
    int getCannonSlots();
    int getArmorSlots();

    std::pair<int, int> getPosition();

    int character; // default 127, a triangle
    int waveResistance;
    int rations;
    int sailors;
    int fatigue; // divide by 10
    int sailorsDied;
    int training; // divide by 10
    int durability;
    std::string lastVisitedCity;
    std::pair<int, int> lastVisitedCityCoords;

    bool starving;
    bool unpaid;
    bool wrecked;

    double movementcounter;

    std::vector<std::pair<int,int>> path;
    std::map<int, ShipSails> sailList;
    std::map<int, ShipArmor> armorList;
    std::map<int, ShipCannons> cannonList;
    
    ShipStatue figurehead;

    bool invisible;

  protected:
    double getArmorSlowing();
    bool removeFromList(const std::string& itemID);
    std::string shipName;
    int storage; // counting only goods
    std::map<std::string, LedgerItem> itemList; // no idea why I didn't use a map in the first place...
    std::pair<int, int> position;
  };