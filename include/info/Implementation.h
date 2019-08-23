#pragma once

#include <iostream>
#include "Runtime.h"
#include "Schema.h"
#include "Type.h"
#include "Instance.h"
#include "TypeBuilder.hpp"

namespace info {
  class Implementation {
    public:
      static InstanceRef instantiate(Runtime& runtime, Schema& schema, const std::string& implementationId) {
        Schema::ImplementationRef implRef = schema.getImplementation(implementationId);
        if (!implRef) return nullptr;

        auto pImp = new Implementation();

        // instantiate all instances in the implementation
        for(auto schemaInstanceRef : implRef->instances) {
          // or fetch Schema::Type by typeId and grab??
          std::string typ = schemaInstanceRef->type;
          // create runtime instance for the type specified in the schema instance
          auto instanceRef = runtime.createInstance(typ);
          // skip the rest if this type is not supported by the runtime
          if (!instanceRef) {
            std::cout << "Failed to instantiate Schema instance "
              << schemaInstanceRef->name << ", Runtime does not support type: "
              << typ << std::endl;

            continue;
          }
          // add the created instance to our implementation
          pImp->instanceRefs.push_back(instanceRef);
          // add mapping from schema instance-ID to actual created instance for future reference
          pImp->schemaIdToInstanceMap[schemaInstanceRef->id] = instanceRef.get();
        }

        // [TODO; apply all initial value]

        // apply all connections in this implementation
        for(auto connectionRef : implRef->connections) {
        }

        return nullptr;
      }

      static void build(TypeBuilder<Implementation>& builder) {

      }

    private:
      std::map<Schema::Id, Instance*> schemaIdToInstanceMap;
      std::vector<InstanceRef> instanceRefs;
  };
}