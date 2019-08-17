#pragma once

#include <memory>
#include <vector>
#include "Port.hpp"
// #include "ciCMS/cfg/Cfg.h"

namespace info {

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

    public:

      std::vector<std::shared_ptr<Port>> portRefs;
  };

}
