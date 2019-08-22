#pragma once

#include <memory>
#include <vector>
#include "Port.hpp"
#include "Void.h"
// #include "ciCMS/cfg/Cfg.h"

namespace info {

  class Instance;
  typedef std::shared_ptr<Instance> InstanceRef;

  class Instance {

    public:

      Instance(std::vector<std::shared_ptr<Port>> ports) : portRefs(ports) {
      }

      template<typename V>
      TypedPort<V>* port(const std::string& portname) {
        for(auto portRef : portRefs)
          if (portRef->getId() == portname)
            return (TypedPort<V>*)portRef.get();

        return NULL;
      }

      inline TypedPort<Void>* signalPort(const std::string& portname) {
        return port<Void>(portname);
      }

    public:

      std::vector<std::shared_ptr<Port>> portRefs;
  };

}
