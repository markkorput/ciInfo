#include <iostream>
#include <memory>
#include "info/Runtime.h"

using namespace info;

InstanceRef Runtime::createInstance(const std::string& typeId) {
  // find registred type
  auto typeRef = getType(typeId);
  if (!typeRef) {
    std::cout << "Runtime::createInstance failed, could not find Type: " << typeId << std::endl;
    return nullptr;
  }

  // find instatiator func that belongs to the found type
  auto pair = typeInstantiatorFuncs.find(typeRef.get());
  if (pair == typeInstantiatorFuncs.end()) {
    std::cout << "Runtime::createInstance failed, could not find instantiator for Type: " << typeId << std::endl;
    return nullptr;
  }

  // create, save and return
  auto intantiatorFunc = pair->second;
  auto instanceRef = intantiatorFunc(typeRef);
  instanceRefs.push_back(instanceRef);
  return instanceRef;
}

bool Runtime::removeInstance(InstanceRef instance) {
  auto it = std::find(instanceRefs.begin(), instanceRefs.end(), instance);
  if (it == instanceRefs.end()) return false;
  instanceRefs.erase(it);
  return true;
}

void Runtime::copyTypes(Runtime& from, Runtime& to) {
  for(auto typeRef : from.typeRefs)
    to.typeRefs.push_back(typeRef);

  for(auto pair : from.typeInstantiatorFuncs)
    to.typeInstantiatorFuncs[pair.first] = pair.second;
}

std::shared_ptr<Type> Runtime::getType(const std::string& typeId) {
  for(auto typeRef : typeRefs)
    if (typeRef->getId() == typeId)
      return typeRef;
  return nullptr;
}