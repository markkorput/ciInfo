#pragma once

#include <iostream>
#include <functional>
#include "Runtime.h"
#include "Schema.h"
#include "Port.hpp"
#include "Type.h"
#include "Instance.h"
#include "TypeBuilder.hpp"

namespace info {
  class Implementation {
    public:

      static InstanceRef instantiate(Runtime& runtime, Schema& schema, const Schema::Id& implementationId, TypeRef typeRef);
      static void build(TypeBuilder<Implementation>& builder, Schema& schema, const Schema::Id& implementationId);

    public: // deconstructor

      ~Implementation();

    private: // methods

      static void withEachPortConnectedToInput(const Schema::Id& inputId, Implementation& imp, Schema::ImplementationRef implRef, std::function<void(Port& p)> func);
      static void withEachPortConnectedToOutput(const Schema::Id& outputId, Implementation& imp, Schema::ImplementationRef implRef, std::function<void(Port& p)> func);
      static void applyInternalSchemaConnections(Implementation& implementation, Schema::ImplementationRef implRef);
      Instance* findInstanceForSchemaId(const Schema::Id& id);

    private:

      std::map<Schema::Id, Instance*> schemaIdToInstanceMap;
      std::vector<InstanceRef> instanceRefs;
      std::vector<cinder::signals::Connection> signalConnections;
      std::vector<std::function<void()>> cleanupFuncs;
  };
}