#pragma once
#include <string>
#include <map>
#include "shipPrototype.h"
//// Basically ship types that can be pasted over player/merchant ships to change their attributes.

class JSONToShip;

class ShipPrototype
  {
  public:
    std::string typeID;
    std::string typeName; //eg, 'Caravel', 'Carrack'
    std::string specialization;
    std::string desc;
    std::string size;
    int price;
    int maxstorage;
    int maxcargo, maxsailors, maxcannons;
    int minimumsailors;
    int baseSpeed(); // calc base speed from sails.
    int lateen, square; // sails
    int waveResistance;
    int baseArmor;
    int maxDurability;
  };

class ShipDictionary
  {
  friend class JSONToShip;
  public:
    ShipDictionary();
    ShipPrototype getShip(const std::string& ID);
    ShipPrototype getRandomShip();
  private:
    ShipPrototype null;
    std::map<std::string, ShipPrototype> ships;
  };

class JSONToShip
  {
  public:
    void readShips(ShipDictionary& dict);
  private:
    std::string slurp(const std::string& filename);
  };

extern ShipDictionary ShipDict;