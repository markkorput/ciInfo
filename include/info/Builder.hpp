#pragma once

#include <functional>
#include "Port.hpp"
#include "PortDef.h"
#include "PortBuilder.hpp"
#include "Void.h"

namespace info {

  template<typename T>
  class Builder {

    public:

      template<typename V>
      std::shared_ptr<PortBuilder<T,V>> attr(const std::string& id) { return addPort<V>(id, Port::FLAG_INOUT); }

      std::shared_ptr<PortBuilder<T,Void>> signal(const std::string& id) { return addPort<Void>(id, Port::FLAG_OUT); }

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

      const std::vector<std::shared_ptr<PortDef>>& getPortDefs() const {
        return portDefRefs;
      };

    private:
      std::vector<std::shared_ptr<PortDef>> portDefRefs;
  };
}