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

      static InstanceRef instantiate(Runtime& runtime, Schema& schema, const std::string& implementationId, TypeRef typeRef) {

        Schema::ImplementationRef implRef = schema.getImplementation(implementationId);
        if (!implRef) return nullptr;

        //
        // Create Implementation instance
        //

        auto pImp = new Implementation();
        pImp->schemaImplementationRef = implRef;

        //
        // Instantiate all instances in the implementation
        //

        for(auto schemaInstanceRef : implRef->instances) {
          // or fetch Schema::Type by typeId and grab??
          std::string typ = schemaInstanceRef->typeId;
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

          // register cleanup routine that removes the created instance
          // the runtime when this implementation deconstructs
          pImp->cleanupFuncs.push_back([&runtime, instanceRef](){
            runtime.removeInstance(instanceRef);
          });
        }

        //
        // [TODO; apply all initial values]
        //

        // ...


        return typeRef->template createInstance<Implementation>(*pImp, true /* delete pImp when instance expires */);
      }

      static void build(TypeBuilder<Implementation>& builder) {
        // TODO create ports as defined in schema
        // builder.signalIn("start");

        builder.connect([](Implementation& imp, Instance& instance) {
          applySchemaConnections(imp, instance, imp.schemaImplementationRef);
        });
      }

    public:

      ~Implementation() {
        for(auto conn : signalConnections)
          conn.disconnect();
        signalConnections.clear();

        for(auto func : cleanupFuncs) {
          func();
        }
        cleanupFuncs.clear();

        schemaIdToInstanceMap.clear();
        instanceRefs.clear();
      }

    private:


      static PortRef getPort(Implementation& implementation, Instance& instance, Schema::ImplementationRef implRef, Schema::ConnectionPoint& connectionPoint) {
        if (connectionPoint.instanceId == implRef->id) {
          return instance.getPort(connectionPoint.portId);
        }

        auto pOutputInstance = implementation.findInstanceForSchemaId(connectionPoint.instanceId);
        if (pOutputInstance) return pOutputInstance->getPort(connectionPoint.portId);

        std::cout << "Could not find port \"" << connectionPoint.portId
          << "\" for instance \"" << connectionPoint.instanceId << "\"" << std::endl;
        return nullptr;
      }

      static void applySchemaConnections(Implementation& implementation, Instance& instance, Schema::ImplementationRef implRef) {
        auto pImp = &implementation;

        //
        // Apply all connections in this implementation
        //

        for(auto connectionRef : implRef->connections) {
          auto outputPort = getPort(implementation, instance, implRef, connectionRef->output);
          auto inputPort = getPort(implementation, instance, implRef, connectionRef->input);

          if (!outputPort) {
            std::cout << "Could not connect schema instance because of missing output port: " << connectionRef->output.portId << std::endl;
            continue;
          }

          if (!inputPort) {
            std::cout << "Could not connect schema instance because of missing input port: " << connectionRef->input.portId << std::endl;
            continue;
          }

          //
          // Connect Ports
          //

          auto signalConnection = outputPort->outputTo(*inputPort, true /* check type */);
          pImp->signalConnections.push_back(signalConnection);
        }
      }
      Instance* findInstanceForSchemaId(const Schema::Id& id) {
        auto it = schemaIdToInstanceMap.find(id);
        if (it != schemaIdToInstanceMap.end()) return it->second;
        return NULL;
      }

    private:
      std::map<Schema::Id, Instance*> schemaIdToInstanceMap;
      std::vector<InstanceRef> instanceRefs;
      std::vector<cinder::signals::Connection> signalConnections;
      std::vector<std::function<void()>> cleanupFuncs;

      Schema::ImplementationRef schemaImplementationRef = nullptr;
  };
}