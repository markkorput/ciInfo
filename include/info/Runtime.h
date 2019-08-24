#pragma once

#include <functional>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include "TypeBuilder.hpp"
#include "Type.h"
#include "Instance.h"

namespace info {
  class Runtime;
  typedef std::shared_ptr<Runtime> RuntimeRef;

  class Runtime {
    public:

      typedef std::function<InstanceRef(TypeRef)> InstantiatorFunc;

      template<class T>
      std::shared_ptr<Type> addType(const std::string& typeId, std::function<void(TypeBuilder<T>&)> builderFunc) {
        InstantiatorFunc instantiatorFunc = [](TypeRef typeRef){
          auto object = new T();
          return typeRef->template createInstance<T>(*object, true /* make sure object is deleted when instance expires */);
        };

        return this->addType<T>(typeId, instantiatorFunc, builderFunc);
      }

      template<class T>
      std::shared_ptr<Type> addType(const std::string& typeId, InstantiatorFunc instantiatorFunc, std::function<void(TypeBuilder<T>&)> builderFunc) {
        auto typeRef = Type::create(typeId, builderFunc);
        typeRefs.push_back(typeRef);

        typeInstantiatorFuncs[typeRef.get()] = instantiatorFunc;
        return typeRef;
      }

      const std::vector<TypeRef>& getTypes() const { return typeRefs; }
      const std::vector<InstanceRef> getInstances() const { return instanceRefs; } 

      InstanceRef createInstance(const std::string& typeId) {
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

      bool removeInstance(InstanceRef instance) {
        auto it = std::find(instanceRefs.begin(), instanceRefs.end(), instance);
        if (it == instanceRefs.end()) return false;
        instanceRefs.erase(it);
        return true;
      }

      static void copyTypes(Runtime& from, Runtime& to) {
        for(auto typeRef : from.typeRefs)
          to.typeRefs.push_back(typeRef);

        for(auto pair : from.typeInstantiatorFuncs)
          to.typeInstantiatorFuncs[pair.first] = pair.second;
      }
    protected:

      std::shared_ptr<Type> getType(const std::string& typeId) {
        for(auto typeRef : typeRefs)
          if (typeRef->getId() == typeId)
            return typeRef;
        return nullptr;
      }

    private:

      std::vector<TypeRef> typeRefs;
      std::map<Type*, InstantiatorFunc> typeInstantiatorFuncs;
      std::vector<InstanceRef> instanceRefs;
  };
}