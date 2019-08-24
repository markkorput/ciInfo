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

      static InstanceRef instantiate(Runtime& runtime, Schema& schema, const Schema::Id& implementationId, TypeRef typeRef) {

        Schema::ImplementationRef implRef = schema.getImplementation(implementationId);
        if (!implRef) return nullptr;

        //
        // Create Implementation instance
        //

        auto pImp = new Implementation();

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


      static void build(TypeBuilder<Implementation>& builder, Runtime& runtime, Schema& schema, const Schema::Id& implementationId) {
        Schema::ImplementationRef implRef = schema.getImplementation(implementationId);
        if (!implRef) return;

        auto schemaTypeRef = schema.getType(implRef->typeId);

        // create an input on the Runtime Type for every input in the schema's Type
        for(auto input : schemaTypeRef->inputs) {
          builder.addInput(input->id)->apply([implRef, input](Implementation& imp, Port& inputPort){
            // establish all connection made to this port according to the schema's implementation
            withEachPortConnectedToInput(input->id, imp, implRef, [&inputPort](Port& p){
              Port::connect(inputPort.inSignal, p.inSignal);
            });
          });
        }

        // create an output on the Runtime Type for every output in the schema's Type
        for(auto output : schemaTypeRef->outputs) {
          builder.addOutput(output->id)->apply([output,implRef](Implementation& imp, Port& outputPort){
            // establish all connection made to this port according to the schema's implementation
            withEachPortConnectedToOutput(output->id, imp, implRef, [&outputPort](Port& p){
              Port::connect(p.outSignal, outputPort.outSignal);
            });
          });
        }
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

      static void withEachPortConnectedToInput(const Schema::Id& inputId, Implementation imp, Schema::ImplementationRef implRef, std::function<void(Port& p)> func) {
        for(auto connRef : implRef->connections) {
          if (connRef->output.instanceId == implRef->id && connRef->output.portId == inputId) {

            auto pReceiver = imp.findInstanceForSchemaId(connRef->input.instanceId);

            if (!pReceiver) {
              std::cerr << "Implementation::withEachPortConnectedToInput could not find receiving instance for id: " << connRef->input.instanceId << std::endl;
              continue;
            }

            auto portRef = pReceiver->getPort(connRef->input.portId);

            if (!portRef) {
              std::cerr << "Implementation::withEachPortConnectedToInput could not find port: " << connRef->input.portId << "(for instance: " << connRef->input.instanceId << ")" << std::endl;
              continue;
            }

            func(*portRef);
          }
        }
      }

      static void withEachPortConnectedToOutput(const Schema::Id& outputId, Implementation imp, Schema::ImplementationRef implRef, std::function<void(Port& p)> func) {
        for(auto connRef : implRef->connections) {
          if (connRef->input.instanceId == implRef->id && connRef->input.portId == outputId) {

            // find the port that receives from this connection 
            auto pInstance = imp.findInstanceForSchemaId(connRef->output.instanceId);

            if (!pInstance) {
              std::cerr << "Implementation::withEachPortConnectedToOutput could not find sending instance for id: " << connRef->output.instanceId << std::endl;
              continue;
            }

            auto portRef = pInstance->getPort(connRef->output.portId);

            if (!portRef) {
              std::cerr << "Implementation::withEachPortConnectedToOutput could not find port: " << connRef->output.portId << "(for instance: " << connRef->output.instanceId << ")" << std::endl;
              continue;
            }

            func(*portRef);
          }
        }
      }

      // static PortRef getPort(Implementation& implementation, Instance& instance, Schema::ImplementationRef implRef, Schema::ConnectionPoint& connectionPoint, bool isInput) {
      //   if (connectionPoint.instanceId == implRef->id) {
      //     return instance.getPort(connectionPoint.portId);
      //   }

      //   auto pOutputInstance = implementation.findInstanceForSchemaId(connectionPoint.instanceId);
      //   if (pOutputInstance) return pOutputInstance->getPort(connectionPoint.portId);

      //   std::cout << "Could not find port \"" << connectionPoint.portId
      //     << "\" for instance \"" << connectionPoint.instanceId << "\"" << std::endl;
      //   return nullptr;
      // }

      // static void applySchemaConnections(Implementation& implementation, Instance& instance, Schema::ImplementationRef implRef) {
      //   auto pImp = &implementation;

      //   //
      //   // Apply all connections in this implementation
      //   //

      //   for(auto connectionRef : implRef->connections) {
      //     std::cout << " ---- connn: " << connectionRef->input.portId << "  --- " << std::endl;
      //     auto outputPort = getPort(implementation, instance, implRef, connectionRef->output);
      //     auto inputPort = getPort(implementation, instance, implRef, connectionRef->input);

      //     if (!outputPort) {
      //       std::cout << "Could not connect schema instance because of missing output port: " << connectionRef->output.portId << std::endl;
      //       continue;
      //     }

      //     if (!inputPort) {
      //       std::cout << "Could not connect schema instance because of missing input port: " << connectionRef->input.portId << std::endl;
      //       continue;
      //     }

      //     //
      //     // Connect Ports
      //     //

      //     auto signalConnection = Port::connect(outputPort->outSignal, outputPort->outputTo(*inputPort, true /* check type */);
      //     pImp->signalConnections.push_back(signalConnection);
      //   }
      // }

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
  };
}