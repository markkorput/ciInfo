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

      // void inputTo(Port::Signal& signal) {
      //   this->portDefRef->addConnector([&signal](void* instance, Port* port){
      //     port->inputTo(signal);
      //   });
      // }

      // void outputFrom(Port::Signal& signal) {
      //   this->portDefRef->addConnector([&signal](void* instance, Port* port){
      //     port->outputFrom(signal);
      //   });
      // }

    protected:
      std::shared_ptr<PortDef> portDefRef;
  };

  template<class T, typename V>
  class PortBuilder : public PortBuilderBase {

    public:

      using Signal = Port::Signal;

      PortBuilder(const std::string& id, int flags)
        : PortBuilderBase(
            std::make_shared<PortDef>(id, typeid(V).name(), flags)) {}

    public:

      void apply(std::function<void(T&, std::function<void(const V&)>)> logic) {
        // connectors connect an runtime object to an info port using custom caller-provided logic
        this->portDefRef->addConnector([logic](void* instance, Port* port){
          logic(*(T*)instance, [port](const V& val){
            port->sendData<V>(val);
          });
        });
      }

      // void apply(std::function<void(T&, Port&)> logic) {
      //   this->apply([logic](T& instance, TypedPort<V>& port){
      //     logic(instance, port);
      //   });
      // }

      void apply(std::function<void(T&, TypedPort<V>&)> logic) {
        // connectors connect an runtime object to an info port using custom caller-provided logic
        this->portDefRef->addConnector([logic](void* instance, Port* port){
          logic(*(T*)instance, *(TypedPort<V>*)port);
        });
      }

      void onData(std::function<void(const V& v)> func) {
        this->apply([func](T& instance, info::TypedPort<V>& port) {
          port.onData(func);
        });
      }

      void apply(Port::Signal& signal) {
        if (portDefRef->isInput())
          this->apply([](T& instance, Port& port) {
              port.outputTo(instance.fireSignal);
          });

        if (portDefRef->isOutput())
          this->apply([](T& instance, Port& port) {
              port.inputFrom(instance.fireSignal);
          });
      }

      // member signal
      template<class TT>
      void apply(Signal TT::*memberSignal) {
        this->apply([memberSignal](TT& instance, Port& port) {
          if (port.isInput())
            port.outputTo((instance.*memberSignal));

          if (port.isOutput())
            port.inputFrom((instance.*memberSignal));
        });
      }

      // argument-less members function
      template<class TT>
      void apply(void (TT::*memberFunction) (void)) {
        this->apply([memberFunction](TT& instance, Port& port) {
          if (port.isOutput())
            std::cout << "WARNING; binding output port to member function, this might give unexpected results." << std::endl; 
          
          port.onSignal([&instance, memberFunction](){ (instance.*memberFunction)(); });
        });
      }

      // member function WITH const reference argument
      template<class TT>
      void apply(void (TT::*memberFunction) (const V&)) {
        this->apply([memberFunction](TT& instance, Port& port) {
          if (port.isOutput())
            std::cout << "WARNING; binding output port to member function, this might give unexpected results." << std::endl; 
          
          port.onData<V>([&instance, memberFunction](const V& val){ (instance.*memberFunction)(val); });
        });
      }

      // member function WITH argument
      template<class TT>
      void apply(void (TT::*memberFunction) (V)) {
        this->apply([memberFunction](TT& instance, Port& port) {
          if (port.isOutput())
            std::cout << "WARNING; binding output port to member function, this might give unexpected results." << std::endl; 
          
          port.onData<V>([&instance, memberFunction](const V& val){ (instance.*memberFunction)(val); });
        });
      }

  };
}