#pragma once

#include <stdlib.h>
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
      std::vector<std::function<void(void*, std::function<void(const void*)>)>> applyFuncs;
      
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
            port->emit<V>(val);
          });
        });

        // // convert into void (typeless) function
        // this->applyFuncs.push_back([func](void* instance, std::function<void(const void*)> voidvaloutfunc){
        //   // func(*(T*)instance, [voidvaloutfunc](const V& val){
        //   //     voidvaloutfunc((void*)&val);
        //   // });
        // });
      }

      void apply(std::function<void(T&, TypedPort<V>&)> logic) {
        // connectors connect an runtime object to an info port using custom caller-provided logic
        this->portDefRef->addConnector([logic](void* instance, Port* port){
          logic(*(T*)instance, *(TypedPort<V>*)port);
        });
      }
  };
}