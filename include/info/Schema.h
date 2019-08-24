#pragma once

#include <stdlib.h>
#include <memory>

namespace info {
  class Schema;
  typedef std::shared_ptr<Schema> SchemaRef;

  class Schema {

    public:

      typedef std::string Id;

      class Port {
        public:
          Id id;
          Id typeId;
      };

      typedef std::shared_ptr<Port> PortRef;

      class Type {
        public:
          Id id;
          std::vector<PortRef> inputs;
          std::vector<PortRef> outputs;
      };

      typedef std::shared_ptr<Type> TypeRef;

      class Instance {
        public:
          Id id;
          Id typeId;
          std::string name;
      };

      typedef std::shared_ptr<Instance> InstanceRef;

      class ConnectionPoint {
        public:
          Id instanceId;
          Id portId;
          int order;
      };

      class Connection {
        public:
          uint id;
          ConnectionPoint output;
          ConnectionPoint input;
      };

      typedef std::shared_ptr<Connection> ConnectionRef;

      class Implementation {
        public:
          Id id;
          Id typeId;
        
          std::vector<InstanceRef> instances;
          std::vector<ConnectionRef> connections;

        private:
          int nextInstanceId = 0;
          uint nextConnectionId = 0;

        public:

          InstanceRef createInstance(const Id& type, const std::string& name) {
            auto ref = std::make_shared<info::Schema::Instance>();
            ref->id = std::to_string(nextInstanceId);
            nextInstanceId++;

            ref->typeId = type;
            ref->name = name;
            instances.push_back(ref);
            return ref;
          }

          ConnectionRef createConnection(InstanceRef outputInstance, const Id& outputPort, InstanceRef inputInstance, const Id& inputPort) {
            return createConnection(
              outputInstance->id,
              outputPort,
              nextOrderForOutputConnections(outputInstance->id, outputPort),
              inputInstance->id,
              inputPort,
              nextOrderForInputConnections(inputInstance->id, inputPort)
            );
          }

          ConnectionRef createConnection(const Id& outputPort, InstanceRef inputInstance, const Id& inputPort) {
            return createConnection(
              this->id,
              outputPort,
              nextOrderForOutputConnections(this->id, outputPort),
              inputInstance->id,
              inputPort,
              nextOrderForInputConnections(inputInstance->id, outputPort)
            );
          }

        protected:

          ConnectionRef createConnection(const Id& outId, const Id& outPortId, int outOrder, const Id& inId, const Id& inPortId, int inOrder) {
            auto ref = std::make_shared<info::Schema::Connection>();

            { // output
              ref->output.instanceId = outId;
              ref->output.portId = outPortId;
              ref->output.order = outOrder;
            }

            { // input
              ref->input.instanceId = inId;
              ref->input.portId = inPortId;
              ref->output.order = inOrder;
            }

            connections.push_back(ref);
            return ref;
          } 

          int nextOrderForOutputConnections(const Id& instanceId, const Id& port) {
            std::vector<ConnectionRef> conns;
            loadOutputConnections(instanceId, port, conns);

            int max = -1;
            for(auto conn : conns)
              max = std::max(conn->output.order, max);

            return max+1;
          }

          int nextOrderForInputConnections(const Id& instanceId, const Id& port) {
            std::vector<ConnectionRef> conns;
            loadInputConnections(instanceId, port, conns);

            int max = -1;
            for(auto conn : conns)
              max = std::max(conn->input.order, max);

            return max+1;
          }

          void loadOutputConnections(const Id& instanceId, const Id& port, std::vector<ConnectionRef>& target) {
            for(auto connRef : connections)
              if (connRef->output.instanceId == instanceId && connRef->output.portId == port)
                target.push_back(connRef);
          }

          void loadInputConnections(const Id& instanceId, const Id& port, std::vector<ConnectionRef>& target) {
            for(auto connRef : connections)
              if (connRef->input.instanceId == instanceId && connRef->input.portId == port)
                target.push_back(connRef);
          }
      };

      typedef std::shared_ptr<Implementation> ImplementationRef;

    public:

      ImplementationRef createImplementation(const Id& id) {
        // an implementation with this ID already exists
        if (getImplementation(id)) return nullptr;

        // create type
        auto typeRef = std::make_shared<Type>();
        typeRef->id = id;
        this->typeRefs.push_back(typeRef);

        // create implementation
        auto ref = std::make_shared<info::Schema::Implementation>();
        implementationRefs.push_back(ref);
        ref->id = id;
        ref->typeId = typeRef->id;

        return ref;
      }

      ImplementationRef getImplementation(const std::string& id) {
        for(auto ref : implementationRefs) 
          if(ref->id == id)
            return ref;
        return nullptr;
      }

    public:

      std::vector<TypeRef> typeRefs;
      std::vector<ImplementationRef> implementationRefs;
  };
}