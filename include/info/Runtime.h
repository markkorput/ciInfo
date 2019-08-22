#pragma once

#include <functional>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include "TypeBuilder.hpp"
#include "Type.h"
#include "Instance.h"

namespace info {
  class Runtime {
    public:

      template<class T>
      std::shared_ptr<Type> addType(const std::string& typeId, std::function<void(TypeBuilder<T>&)> builderFunc) {
        auto typeRef = Type::create(typeId, builderFunc);
        typeRefs.push_back(typeRef);

        typeInstantiatorFuncs[typeRef.get()] = [typeRef](){
          auto object = new T();
          return typeRef->template createInstance<T>(*object);
        };

        std::cout << "TODO: also register an on destroy func" << std::endl;

        return typeRef;
      }

      const std::vector<std::shared_ptr<Type>>& getTypes() const {
        return typeRefs;
      }

      std::shared_ptr<Instance> createInstance(const std::string& typeId) {
        // find registred type
        auto typeRef = getType(typeId);
        if (!typeRef) return nullptr;

        // find instatiator func that belongs to the found type
        auto pair = typeInstantiatorFuncs.find(typeRef.get());
        if (pair == typeInstantiatorFuncs.end()) return nullptr; // no instantiator?

        // create, save and return
        auto intantiatorFunc = pair->second;
        auto instanceRef = intantiatorFunc();
        instanceRefs.push_back(instanceRef);
        return instanceRef;
      }

    protected:

      std::shared_ptr<Type> getType(const std::string& typeId) {
        for(auto typeRef : typeRefs)
          if (typeRef->getId() == typeId)
            return typeRef;
        return nullptr;
      }

    private:
      std::vector<std::shared_ptr<Type>> typeRefs;
      std::map<Type*, std::function<std::shared_ptr<Instance>()>> typeInstantiatorFuncs;
      std::vector<std::shared_ptr<Instance>> instanceRefs;
  };
}