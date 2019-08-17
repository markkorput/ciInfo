#pragma once

#include <stdlib.h>
#include <string>
#include "Port.hpp"

namespace info {

  class PortDef {
    public:

      const static int FLAG_IN = 1;
      const static int FLAG_OUT = (1 >> 1);
      const static int FLAG_INOUT = (FLAG_IN & FLAG_OUT);

      typedef std::function<void(void*, Port*)> ConnectorFunc;

    public:

      PortDef(const std::string& id, const std::string& type, int flags = FLAG_INOUT) : id(id), type(type), flags(flags) {
      }
  
    public:

      const std::string& getId() const { return id; }
      const std::string& getType() const { return type; }
      int getFlags() const { return flags; }

      void addConnector(ConnectorFunc func) {
        connectorFuncs.push_back(func);
      }

      template<class T>
      std::shared_ptr<Port> createPortFor(T& instance) {
        auto portRef = std::make_shared<Port>(id, type, flags);
        
        for(auto func : connectorFuncs)
          func((void*)&instance, portRef.get());

        return portRef;
      }

    private:
      std::string id;
      std::string type;
      int flags;
      std::vector<ConnectorFunc> connectorFuncs;
      // ::ctree::Signal<void(void)> signal;
  };

}
