#include "Framework/Factory.h"
#include "Framework/Logger.h"


//================================== Factory ===================================

// Register class
void Factory::registerClass(const std::string& class_name, Creator* creator)
{
  // Add this class name and creator pointer in the class container
  getClassTable()[class_name] = creator;
}

// Register object
void Factory::registerObject(const std::string& object_name, Accessible* object)
{
  // Add this object in the object container
  getObjectTable()[object_name] = object;
}

// Return static container of all accessible classes
Factory::ClassContainer& Factory::getClassTable()
{
  // Declaration of static class container
  static ClassContainer _class_table;

  // Return static class container
  return _class_table;
}

// Return static container of all accessible objects
Factory::ObjectContainer& Factory::getObjectTable()
{
  // Declaration of static object container
  static ObjectContainer _object_table;

  // Return static object container
  return _object_table;
}

// Return an object from his name
Accessible* Factory::get(const std::string& object_name)
{
  // Find this object in the object container
  ObjectContainer::iterator object_it = getObjectTable().find(object_name);

  // Check if the object has already been created
  if (object_it != getObjectTable().end())
  {
    // Get the object pointer from the map
    Accessible* object = object_it->second;

    // Check object
    if (object)
    {
      // Return pointer to the object
      return object;
    }
  }

  // Object has not been created
  FATAL_PF("Object %s has not been created", object_name.c_str());

  // Return Null pointer
  return NULL;
}

// Function to create an instance of a class
Accessible* Factory::create(const LuaTable& cfg)
{
  // Get the class name
  std::string class_name = cfg.get<std::string>("class");

  // Find the creator associated with this class name if any
  ClassContainer::iterator class_it = getClassTable().find(class_name);

  // Check if there is a creator associated with this class name
  if (class_it != getClassTable().end())
  {
    // Get the object name
    std::string object_name = cfg.get<std::string>("name");

    // Check if the object has already been created
    if (existObject(object_name))
    {
      // Object has already been created
      FATAL_PF("Object %s has already been created", object_name.c_str());

      // Return Null pointer
      return NULL;
    }
    else
    {
      // Call the creator for this class
      Accessible* object = class_it->second->create(cfg);

      // Put this new object in the object container
      registerObject(object_name, object);

      // Return the pointer to this newly created object
      return object;
    }
  }
  else
  {
    // This class is not registered in the factory
    FATAL_PF("Class %s is not registered in the factory", class_name.c_str());

    // Return null pointer
    return NULL;
  }
}

// Get or create an instance of a class
Accessible* Factory::getOrCreate(const LuaTable& cfg)
{
  // Get the object name
  std::string object_name = cfg.get<std::string>("name");

  // Check if the object has already been created
  if (existObject(object_name))
  {
    // Object has already been created
    FATAL_PF("Object %s has already been created", object_name.c_str());

    // Return Null pointer
    return NULL;
  }
  else
  {
    // Call the creator for this class
    Accessible* object = create(cfg);

    // Return the pointer to this newly created object
    return object;
  }
}

// Check if this object has been registered
bool Factory::existObject(const std::string& name)
{
  // Find this object in the object container
  ObjectContainer::iterator object_it = getObjectTable().find(name);

  // Check if the object has already been created
  if (object_it != getObjectTable().end()) { return true; }
  else { return false; }
}

// Delete an object from his name
void Factory::deleteObject(const std::string& object_name)
{
  // Find this object in the object container
  ObjectContainer::iterator object_it = getObjectTable().find(object_name);

  // Check if the object has already been created
  if (object_it != getObjectTable().end())
  {
    // Get the object pointer from the map
    Accessible* object = object_it->second;

    // Check object
    if (object)
    {
      // Delete object
      delete object;
    }
  }
}
