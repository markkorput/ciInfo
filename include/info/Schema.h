#pragma once

#include <stdlib.h>
#include <memory>
#include <vector>
#include <functional>

// #include "Port.hpp"

namespace info {
  class Schema;
  typedef std::shared_ptr<Schema> SchemaRef;

  class Schema {

    public: // sub-types

      typedef std::string Id;

      class Port;
      class Type;
      class Instance;
      class Connection;
      class Implementation;
      class InitialValue;

      typedef std::shared_ptr<Port> PortRef;
      typedef std::shared_ptr<Type> TypeRef;
      typedef std::shared_ptr<Instance> InstanceRef;
      typedef std::shared_ptr<Connection> ConnectionRef;
      typedef std::shared_ptr<Implementation> ImplementationRef;
      typedef std::shared_ptr<InitialValue> InitialValueRef;

      class Port {
        public:
          Id id;
          Id typeName;
      };

      class Type {
        public:
          Id id;
          std::vector<PortRef> inputs;
          std::vector<PortRef> outputs;

        public:
          PortRef getInput(const Id& id) {
            for(auto p : inputs)
              if (p->id == id)
                return p;
            return nullptr;
          }

          PortRef getOutput(const Id& id) {
            for(auto p : outputs)
              if (p->id == id)
                return p;
            return nullptr;
          }
      };

      class Instance {
        public:
          Id id;
          Id typeId;
          std::string name;
      };

      class Connection {
        public:
          class ConnectionPoint {
            public:
              Id instanceId;
              Id portId;
              int order;
          };

        public:
          unsigned int id;
          ConnectionPoint output;
          ConnectionPoint input;
      };

      class InitialValue {
          public:
            Id id;
            Id instanceId;
            Id portId;
            std::string typeName;
            void* value = NULL;

          public: // methods

            ~InitialValue() {
              if (this->cleanupFunc) this->cleanupFunc();
            }

            template<typename V>
            void set(const V& val) {
              if (value != NULL && this->cleanupFunc) {
                this->cleanupFunc();
              }

              V* pointer = new V();
              *pointer = val;
              this->value = (void*)pointer;

              this->cleanupFunc = [this]() {
                V* deleter = (V*)this->value;
                delete deleter;
                this->value = NULL;
                this->cleanupFunc = nullptr;
              };
            }
          
          private:
            std::function<void()> cleanupFunc = nullptr;
      };

      class Implementation {
        public:
          Id id;
          Id typeId;
        
          std::vector<InstanceRef> instances;
          std::vector<ConnectionRef> connections;
          std::vector<InitialValueRef> initialValues;
      };

    public: // get methods

      ImplementationRef getImplementation(const std::string& id);
      const std::vector<ImplementationRef> getImplementations() const { return implementationRefs; }
      TypeRef getType(const std::string& id);

    public: // create methods

      ImplementationRef createImplementation(const Id& id);
      InstanceRef createInstance(ImplementationRef imp, const Id& type, const std::string& name = "");

      /// Create connection from the output port of an implementation's instance,
      /// to the input port of another instance in the same implementation
      ConnectionRef createConnection(ImplementationRef imp, InstanceRef outputInstance, const Id& outputPort, InstanceRef inputInstance, const Id& inputPort);

      /// Create connection from an input port of the implementation to an input of one of its instances
      ConnectionRef createConnection(ImplementationRef imp, const Id& outputPort, InstanceRef inputInstance, const Id& inputPort);

      template <typename V>
      InitialValueRef createInitialValue(ImplementationRef imp, InstanceRef instance, const Id& portId, const V& val);

    protected: // methods

      PortRef createPort(TypeRef type, const Id& portId, bool input);
      PortRef createOutput(TypeRef type, const Id& portId);
      PortRef createInput(TypeRef type, const Id& portId);
      ConnectionRef createConnection(ImplementationRef imp, const Id& outId, const Id& outPortId, int outOrder, const Id& inId, const Id& inPortId, int inOrder);
      Id getTypeIdForInstance(ImplementationRef imp, const Id& instanceId);
      TypeRef getTypeForInstance(ImplementationRef imp, const Id& instanceId);
      int nextOrderForOutputConnections(ImplementationRef imp, const Id& instanceId, const Id& port);
      int nextOrderForInputConnections(ImplementationRef imp, const Id& instanceId, const Id& port);
      void loadOutputConnections(ImplementationRef imp, const Id& instanceId, const Id& port, std::vector<ConnectionRef>& target);
      void loadInputConnections(ImplementationRef imp, const Id& instanceId, const Id& port, std::vector<ConnectionRef>& target);

    public: // attributes //TODO make private

      std::vector<TypeRef> typeRefs;
      std::vector<ImplementationRef> implementationRefs;

    private: // attributes

      int nextInstanceId = 0;
      unsigned int nextConnectionId = 0;
  };

  template <typename V>
  Schema::InitialValueRef Schema::createInitialValue(Schema::ImplementationRef imp, Schema::InstanceRef instance, const Schema::Id& portId, const V& val) {
    auto ref = std::make_shared<Schema::InitialValue>();

    ref->instanceId = instance->id;
    ref->portId = portId;
    ref->typeName = typeid(V).name();
    ref->set<V>(val);
    imp->initialValues.push_back(ref);

    return ref;
  }

}