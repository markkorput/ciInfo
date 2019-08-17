#pragma once

#include <stdlib.h>
#include <iostream>
#include <functional>
#include "Port.hpp"

namespace info {

  class PortBuilderBase {
    public:
      PortBuilderBase(std::shared_ptr<PortDef> portDefRef) : portDefRef(portDefRef) {
      }

      std::shared_ptr<PortDef> getPortDef() {
        return portDefRef;
      }

    protected:
      std::shared_ptr<PortDef> portDefRef;
  };

  template<class T, typename V>
  class PortBuilder : public PortBuilderBase {

    public:

      PortBuilder(const std::string& id, int flags)
        : PortBuilderBase(
            std::make_shared<PortDef>(id, typeid(V).name(), flags)) {}

    public:

      void apply(std::function<void(T&, std::function<void(const V&)>)> logic) {
        // connectors connect an runtime object to an info port using custom caller-provided logic
        this->portDefRef->addConnector([logic](void* instance, Port* port){
          logic(*(T*)instance, [port](const V& val){
            port->dataOut<V>(val);
          });
        });
      }

      void apply(std::function<void(T&, TypedPort<V>&)> logic) {
        // connectors connect an runtime object to an info port using custom caller-provided logic
        this->portDefRef->addConnector([logic](void* instance, Port* port){
          logic(*(T*)instance, *(TypedPort<V>*)port);
        });
      }
  };
}