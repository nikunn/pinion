#ifndef _FACTORY_H
#define _FACTORY_H

#include <map>

#include "Framework/Accessible.h"
#include "Framework/LuaBind.h"


namespace pno
{
//================================== Creator ===================================

// Base class from which will derive the factory creation of objects modules
class Creator
{
public:
  Creator() {};
        
  virtual Accessible* create(const LuaTable&) = 0;
};

// Template class to instatiate object of some defined type
template <class T>
class CreatorType : public Creator
{
public:
  CreatorType() : Creator() {}

  virtual Accessible* create(const LuaTable& cfg) { return new T(cfg); }
};


//================================== Factory ===================================

// A static factory class to create object dynamically
class Factory
{
public:

  //================================ Typedefs ================================
  using ClassContainer = std::map<std::string, Creator*>;
  using ObjectContainer = std::map<std::string, Accessible*>;

public:

  //============================ public functions ============================
  static Accessible* get(const std::string& object_name);
  static Accessible* create(const LuaTable&);
  static Accessible* getOrCreate(const LuaTable&);


  static void registerClass(const std::string& class_name, Creator* creator);
  static void registerObject(const std::string& object_name, Accessible* object);

  static bool existObject(const std::string& object_name);
  static void deleteObject(const std::string& object_name);

private:

  //=========================== private functions ============================
  static ClassContainer& getClassTable();
  static ObjectContainer& getObjectTable();
};


}
#endif
