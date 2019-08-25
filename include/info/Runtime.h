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

    public: // static methods

      static void copyTypes(Runtime& from, Runtime& to);

      // add type with ID and builder func
      template<class T>
      std::shared_ptr<Type> addType(const std::string& typeId, std::function<void(TypeBuilder<T>&)> builderFunc);

      // add type with ID and builder func and with custom object instantiator
      template<class T>
      std::shared_ptr<Type> addType(const std::string& typeId, std::function<T*()> objectInstatiatorFunc, std::function<void(TypeBuilder<T>&)> builderFunc);

      // add type with ID and builder func and with custom instance instantiator
      template<class T>
      std::shared_ptr<Type> addType(const std::string& typeId, InstantiatorFunc instantiatorFunc, std::function<void(TypeBuilder<T>&)> builderFunc);

      inline const std::vector<TypeRef>& getTypes() const { return typeRefs; }
      inline const std::vector<InstanceRef> getInstances() const { return instanceRefs; } 

      InstanceRef createInstance(const std::string& typeId);
      bool removeInstance(InstanceRef instance);

    protected:

      std::shared_ptr<Type> getType(const std::string& typeId);

    private:

      std::vector<TypeRef> typeRefs;
      std::map<Type*, InstantiatorFunc> typeInstantiatorFuncs;
      std::vector<InstanceRef> instanceRefs;
  };


  // add type with ID and builder func
  template<class T>
  std::shared_ptr<Type> Runtime::addType(const std::string& typeId, std::function<void(TypeBuilder<T>&)> builderFunc) {
    InstantiatorFunc instantiatorFunc = [](TypeRef typeRef){
      auto object = new T();
      return typeRef->template createInstance<T>(*object, true /* make sure object is deleted when instance expires */);
    };

    return this->addType<T>(typeId, instantiatorFunc, builderFunc);
  }

  // add type with ID and builder func and with custom object instantiator
  template<class T>
  std::shared_ptr<Type> Runtime::addType(const std::string& typeId, std::function<T*()> objectInstatiatorFunc, std::function<void(TypeBuilder<T>&)> builderFunc) {
    InstantiatorFunc instantiatorFunc = [objectInstatiatorFunc](TypeRef typeRef){
      return typeRef->template createInstance<T>(*objectInstatiatorFunc(), true /* make sure object is deleted when instance expires */);
    };

    return this->addType<T>(typeId, instantiatorFunc, builderFunc);
  }

  // add type with ID and builder func and with custom instance instantiator
  template<class T>
  std::shared_ptr<Type> Runtime::addType(const std::string& typeId, InstantiatorFunc instantiatorFunc, std::function<void(TypeBuilder<T>&)> builderFunc) {
    auto typeRef = Type::create(typeId, builderFunc);
    typeRefs.push_back(typeRef);

    typeInstantiatorFuncs[typeRef.get()] = instantiatorFunc;
    return typeRef;
  }
}