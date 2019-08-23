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
          Id type;
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
          Id type;
          std::string name;
      };

      typedef std::shared_ptr<Instance> InstanceRef;

      class ConnectionPoint {
        public:
          Id instance;
          Id port;
          int order;
      };

      class Connection {
        public:
          uint id;
          ConnectionPoint output;
          ConnectionPoint input;
      };

      typedef std::shared_ptr<Connection> ConnectionRef;

      class Implementation : public Instance {
        public:
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

            ref->type = type;
            ref->name = name;
            instances.push_back(ref);
            return ref;
          }

          /// creates connection from an outputPort of this implementation to an input of an instance inside the implementation
          ConnectionRef createConnection(const Id& outputPort, InstanceRef inputInstance, const Id& inputPort) {
            auto ref = std::make_shared<info::Schema::Connection>();

            ref->id = nextConnectionId;
            nextConnectionId++;

            { // output
              ref->output.instance = this->id;
              ref->output.port = outputPort;
              ref->output.order = nextOrderForOutputConnections(*(Instance*)this, outputPort);
            }

            { // input
              ref->input.instance = inputInstance->id;
              ref->input.port = inputPort;
              ref->output.order = nextOrderForInputConnections(*(Instance*)this, outputPort);
            }

            connections.push_back(ref);
            return ref;
          }

          ConnectionRef createConnection(InstanceRef outputInstance, const Id& outputPort, InstanceRef inputInstance, const Id& inputPort) {
            auto ref = std::make_shared<info::Schema::Connection>();

            ref->id = nextConnectionId;
            nextConnectionId++;

            { // output
              ref->output.instance = outputInstance->id;
              ref->output.port = outputPort;
              ref->output.order = nextOrderForOutputConnections(*outputInstance, outputPort);
            }

            { // input
              ref->input.instance = inputInstance->id;
              ref->input.port = inputPort;
              ref->output.order = nextOrderForInputConnections(*outputInstance, outputPort);
            }

            connections.push_back(ref);
            return ref;
          }

        protected:

          int nextOrderForOutputConnections(Instance& instance, const Id& port) {
            std::vector<ConnectionRef> conns;
            loadOutputConnections(instance, port, conns);

            int max = -1;
            for(auto conn : conns)
              max = std::max(conn->output.order, max);

            return max+1;
          }

          int nextOrderForInputConnections(Instance& instance, const Id& port) {
            std::vector<ConnectionRef> conns;
            loadInputConnections(instance, port, conns);

            int max = -1;
            for(auto conn : conns)
              max = std::max(conn->input.order, max);

            return max+1;
          }


          void loadOutputConnections(Instance& instance, const Id& port, std::vector<ConnectionRef>& target) {
            for(auto connRef : connections)
              if (connRef->output.instance == instance.id && connRef->output.port == port)
                target.push_back(connRef);
          }

          void loadInputConnections(Instance& instance, const Id& port, std::vector<ConnectionRef>& target) {
            for(auto connRef : connections)
              if (connRef->input.instance == instance.id && connRef->input.port == port)
                target.push_back(connRef);
          }
      };

      typedef std::shared_ptr<Implementation> ImplementationRef;

    public:

      ImplementationRef createImplementation(const Id& id) {
        auto ref = std::make_shared<info::Schema::Implementation>();
        implementationRefs.push_back(ref);

        ref->id = id;
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