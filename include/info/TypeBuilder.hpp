#pragma once

#include <functional>
#include "Port.hpp"
#include "PortDef.h"
#include "PortBuilder.hpp"
#include "Instance.h"
#include "Void.h"

namespace info {

  template<typename T>
  class TypeBuilder {

    public:

      typedef std::function<void(void*, Instance&)> InstanceConnectFunc;

      template<typename V>
      std::shared_ptr<PortBuilder<T,V>> attr(const std::string& id) { return addPort<V>(id, Port::FLAG_INOUT); }

      template<typename V>
      std::shared_ptr<PortBuilder<T,V>> input(const std::string& id) { return addPort<V>(id, Port::FLAG_IN); }

      template<typename V>
      std::shared_ptr<PortBuilder<T,V>> output(const std::string& id) { return addPort<V>(id, Port::FLAG_OUT); }

      template<typename V>
      std::shared_ptr<PortBuilder<T,V>> addPort(const std::string& id, int flags) {
        // create port builder
        auto ref = std::make_shared<PortBuilder<T,V>>(id, flags);
        // grab the PortDef from the builder and save it
        this->portDefRefs.push_back(ref->getPortDef());
        // give the PortBuilder to the caller
        return ref;
      }

      std::shared_ptr<PortBuilder<T,Void>> addInput(const std::string& id) { return addPort<Void>(id, Port::FLAG_IN); }
      std::shared_ptr<PortBuilder<T,Void>> addOutput(const std::string& id) { return addPort<Void>(id, Port::FLAG_OUT); }

      std::shared_ptr<PortBuilder<T,Void>> signalOut(const std::string& id) { return addPort<Void>(id, Port::FLAG_OUT); }
      std::shared_ptr<PortBuilder<T,Void>> signalIn(const std::string& id) { return addPort<Void>(id, Port::FLAG_IN); }
      std::shared_ptr<PortBuilder<T,Void>> signal(const std::string& id) { return addPort<Void>(id, Port::FLAG_INOUT); }

      const std::vector<std::shared_ptr<PortDef>>& getPortDefs() const {
        return portDefRefs;
      };

      const std::vector<InstanceConnectFunc> getInstanceConnectFuncs() const {
        return instanceConnectFuncs;
      }

      void connect(std::function<void(T& obj, Instance&)> func) {
        instanceConnectFuncs.push_back([func](void* obj, Instance& inst){
          func(*(T*)obj, inst);
        });
      }


    private:
      std::vector<std::shared_ptr<PortDef>> portDefRefs;
      std::vector<InstanceConnectFunc> instanceConnectFuncs;
  };
}