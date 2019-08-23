#pragma once

#include <functional>
#include <vector>
#include <memory>
#include "Port.hpp"
#include "TypeBuilder.hpp"
#include "Instance.h"

namespace info {

  class Type;
  typedef std::shared_ptr<Type> TypeRef;

  class Type {
    public:

      template<class T>
      static std::shared_ptr<Type> create(const std::string &typeId, std::function<void(TypeBuilder<T>&)> func) {
        TypeBuilder<T> builder;

        // let caller configure our builder
        func(builder);

        // create interface and populate with port defs from builder
        auto type = std::make_shared<Type>();
        type->id = typeId;
        type->portDefRefs = builder.getPortDefs();

        return type;
      }

    public:

      // configure an instance with the given Cfg
      template<class T>
      std::shared_ptr<Instance> createInstance(T& instance) {
        std::vector<std::shared_ptr<Port>> ports;

        for(auto portDefRef : portDefRefs)
          ports.push_back(portDefRef->createPortFor(instance));

        auto instanceRef = std::make_shared<Instance>(ports);
        return instanceRef;
      }

    public:

      const std::string& getId() const { return id; }

      const std::vector<std::shared_ptr<PortDef>>& getPorts() const {
        return portDefRefs;
      }

    private:
      std::string id = "";
      std::vector<std::shared_ptr<PortDef>> portDefRefs;
  };

}
