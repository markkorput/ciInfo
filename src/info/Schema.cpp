#include <iostream>
#include "info/Schema.h"

using namespace info;

// typedef std::string Id;
using Id = Schema::Id;
using Port = Schema::Port;
using Type = Schema::Type;
using Instance = Schema::Instance;
using Connection = Schema::Connection;
using Implementation = Schema::Implementation;
using InitialValue = Schema::InitialValue;

using PortRef = Schema::PortRef;
using TypeRef = Schema::TypeRef;
using InstanceRef = Schema::InstanceRef;
using ConnectionRef = Schema::ConnectionRef;
using ImplementationRef = Schema::ImplementationRef;
using InitialValueRef = Schema::InitialValueRef;

ImplementationRef Schema::getImplementation(const std::string& id) {
  for(auto ref : implementationRefs) 
    if(ref->id == id)
      return ref;
  return nullptr;
}

TypeRef Schema::getType(const std::string& id) {
  for(auto ref : typeRefs) 
    if(ref->id == id)
      return ref;
  return nullptr;
}

ImplementationRef Schema::createImplementation(const Id& id) {
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

InstanceRef Schema::createInstance(ImplementationRef imp, const Id& type, const std::string& name) {
  // find existing type
  auto typeRef = getType(type);
  // create new (empty) type if necessary
  if (!typeRef) {
    typeRef = std::make_shared<Type>();
    typeRef->id = type;
    typeRefs.push_back(typeRef);
  }
  // create implementation
  auto ref = std::make_shared<info::Schema::Instance>();
  ref->id = std::to_string(nextInstanceId++);
  ref->typeId = typeRef->id;
  ref->name = name != "" ? name : type;
  // save
  imp->instances.push_back(ref);
  // return
  return ref;
}

/// Create connection from the output port of an implementation's instance,
/// to the input port of another instance in the same implementation
ConnectionRef Schema::createConnection(ImplementationRef imp, InstanceRef outputInstance, const Id& outputPort, InstanceRef inputInstance, const Id& inputPort) {
  return createConnection(
    imp,
    outputInstance->id,
    outputPort,
    nextOrderForOutputConnections(imp, outputInstance->id, outputPort),
    inputInstance->id,
    inputPort,
    nextOrderForInputConnections(imp, inputInstance->id, inputPort)
  );
}

/// Create connection from an input port of the implementation to an input of one of its instances
ConnectionRef Schema::createConnection(ImplementationRef imp, const Id& outputPort, InstanceRef inputInstance, const Id& inputPort) {
  return createConnection(
    imp,
    imp->id,
    outputPort,
    nextOrderForOutputConnections(imp, imp->id, outputPort),
    inputInstance->id,
    inputPort,
    nextOrderForInputConnections(imp, inputInstance->id, outputPort)
  );
}

PortRef Schema::createPort(TypeRef type, const Id& portId, bool input) {
  return input ? createInput(type, portId) : createOutput(type, portId);
}

PortRef Schema::createOutput(TypeRef type, const Id& portId) {
  auto portRef = std::make_shared<Port>();
  portRef->id = portId;
  type->outputs.push_back(portRef);
  return portRef;
}

PortRef Schema::createInput(TypeRef type, const Id& portId) {
  auto portRef = std::make_shared<Port>();
  portRef->id = portId;
  type->inputs.push_back(portRef);
  return portRef;
}

ConnectionRef Schema::createConnection(ImplementationRef imp, const Id& outId, const Id& outPortId, int outOrder, const Id& inId, const Id& inPortId, int inOrder) {
  auto ref = std::make_shared<info::Schema::Connection>();

  { // output
    ref->output.instanceId = outId;
    ref->output.portId = outPortId;
    ref->output.order = outOrder;
    
    { // make sure the specified port exists in the relevant type
      bool isInput = false;
      std::shared_ptr<Type> typeRef = nullptr;

      if (outId == imp->id) {
        isInput = true;
        typeRef = getType(imp->typeId);
      } else {
        typeRef = getTypeForInstance(imp, outId);
      }

      if (!typeRef) {
        std::cerr << "Schema.createConnection could not find type for: " << outId << std::endl;
      } else {
        auto portRef = isInput ? typeRef->getInput(outPortId) : typeRef->getOutput(outPortId);
        if (!portRef) createPort(typeRef, outPortId, isInput);
      }
    }
  }

  { // input
    ref->input.instanceId = inId;
    ref->input.portId = inPortId;
    ref->input.order = inOrder;

    { // make sure the specified port exists in the relevant type
      bool isInput = true;
      std::shared_ptr<Type> typeRef = nullptr;

      if (inId == imp->id) {
        isInput = false;
        typeRef = getType(imp->typeId);
      } else {
        typeRef = getTypeForInstance(imp, inId);
      }

      if (!typeRef) {
        std::cerr << "Schema.createConnection could not find type for: " << inId << std::endl;
      } else {
        auto portRef = isInput ? typeRef->getInput(inPortId) : typeRef->getOutput(inPortId);
        if (!portRef) createPort(typeRef, inPortId, isInput);
      }
    }
  }

  imp->connections.push_back(ref);
  return ref;
} 

Id Schema::getTypeIdForInstance(ImplementationRef imp, const Id& instanceId) {
  for(auto inst : imp->instances)
    if (inst->id == instanceId)
      return inst->typeId;

  // // id references the implementation itself
  // if (instanceId == imp->id)
  //   return imp->typeId;

  return "";
}

TypeRef Schema::getTypeForInstance(ImplementationRef imp, const Id& instanceId) {
  Id id = getTypeIdForInstance(imp, instanceId);
  return id == "" ? nullptr : getType(id);
}

int Schema::nextOrderForOutputConnections(ImplementationRef imp, const Id& instanceId, const Id& port) {
  std::vector<ConnectionRef> conns;
  loadOutputConnections(imp, instanceId, port, conns);

  int max = -1;
  for(auto conn : conns)
    max = std::max(conn->output.order, max);

  return max+1;
}

int Schema::nextOrderForInputConnections(ImplementationRef imp, const Id& instanceId, const Id& port) {
  std::vector<ConnectionRef> conns;
  loadInputConnections(imp, instanceId, port, conns);

  int max = -1;
  for(auto conn : conns)
    max = std::max(conn->input.order, max);

  return max+1;
}

void Schema::loadOutputConnections(ImplementationRef imp, const Id& instanceId, const Id& port, std::vector<ConnectionRef>& target) {
  for(auto connRef : imp->connections)
    if (connRef->output.instanceId == instanceId && connRef->output.portId == port)
      target.push_back(connRef);
}

void Schema::loadInputConnections(ImplementationRef imp, const Id& instanceId, const Id& port, std::vector<ConnectionRef>& target) {
  for(auto connRef : imp->connections)
    if (connRef->input.instanceId == instanceId && connRef->input.portId == port)
      target.push_back(connRef);
}

