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

      Instance(std::vector<std::shared_ptr<Port>> ports, std::function<void()> cleanupFunc = nullptr) : portRefs(ports), cleanupFunc(cleanupFunc) {
      }

      ~Instance() {
        if (this->cleanupFunc) {
          cleanupFunc();
          cleanupFunc = nullptr;
        }
      }

      PortRef getPort(const std::string& portname, bool input=true) {
        return input ? getInput(portname) : getOutput(portname);
      }

      PortRef getInput(const std::string& portname) {
        for(auto portRef : portRefs)
          if (portRef->isInput() && portRef->getId() == portname)
            return portRef;
        return nullptr;
      }

      PortRef getOutput(const std::string& portname) {
        for(auto portRef : portRefs) {
          if (portRef->isOutput() && portRef->getId() == portname)
            return portRef;
        }
        return nullptr;
      }
  
    public:

      std::vector<PortRef> portRefs;
      std::function<void()> cleanupFunc;
  };

}
