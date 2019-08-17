#pragma once

#include <functional>
#include <vector>
#include "Port.hpp"
#include "Builder.hpp"
#include "Instance.h"

namespace info {

  class Interface {
    public:

      template<class T>
      static Interface* create(std::function<void(Builder<T>&)> func) {
        Builder<T> builder;
        // let caller configure our builder
        func(builder);

        auto interface = new Interface();
        interface->portDefRefs = builder.getPortDefs();

      //   for(auto& builderOutput : builder->outputs) {
      //     auto output = std::shared_ptr<Port>(builderOutput->create());

      //     /// add output to the interface based on the output definitions added to the builder
      //     interface->outputs.push_back(output);

      //     /// add instance configuration logic to our interface based on definitions in the builder
      //     for(auto& func : builderOutput->applyFuncs) {
      //       interface->instanceFuncs.push_back([func, output](void* instance){
      //         func(instance, [output](const void* arg){
      //           output->invokeMethod(arg);
      //         });
      //       });
      //     }
      //   }

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
