#pragma once

#include <functional>
#include <vector>
#include <memory>
#include "Port.hpp"
#include "Builder.hpp"
#include "Instance.h"

namespace info {

  class Interface {
    public:

      template<class T>
      static std::shared_ptr<Interface> create(std::function<void(Builder<T>&)> func) {
        Builder<T> builder;

        // let caller configure our builder
        func(builder);

        // create interface and populate with port defs from builder
        auto interface = std::make_shared<Interface>();
        interface->portDefRefs = builder.getPortDefs();

        return interface;
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

      const std::vector<std::shared_ptr<PortDef>>& getPorts() const {
        return portDefRefs;
      }

    private:
      std::vector<std::shared_ptr<PortDef>> portDefRefs;
  };

}
