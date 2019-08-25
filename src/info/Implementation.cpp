#include "info/Implementation.h"

using namespace info;

InstanceRef Implementation::instantiate(Runtime& runtime, Schema& schema, const Schema::Id& implementationId, TypeRef typeRef) {
  Schema::ImplementationRef implRef = schema.getImplementation(implementationId);
  if (!implRef) {
    std::cerr << "Implementation::instantiate could not find implementation in schema: " << implementationId << std::endl;
    return nullptr;
  }

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
      // std::cout << "Implementation cleanup func" << std::endl;
      runtime.removeInstance(instanceRef);
    });
  }

  //
  // [TODO; apply all initial values]
  //

  for(auto initialValRef : implRef->initialValues) {
    auto instance = pImp->findInstanceForSchemaId(initialValRef->instanceId);
    auto portRef = instance ? instance->getInput(initialValRef->portId) : nullptr;

    if (!portRef) {
      std::cerr << "Could not apply initial value because of missing port and/or instance: " << initialValRef->instanceId << "/" << initialValRef->portId << std::endl;
      continue;
    }

    portRef->signal.emit(initialValRef->value);
  }

  applyInternalSchemaConnections(*pImp, implRef);
  

  auto instanceRef = typeRef->template createInstance<Implementation>(*pImp, true /* delete pImp when instance expires */);
  return instanceRef;
}

void Implementation::build(TypeBuilder<Implementation>& builder, Schema& schema, const Schema::Id& implementationId) {
  Schema::ImplementationRef implRef = schema.getImplementation(implementationId);
  if (!implRef) return;

  auto schemaTypeRef = schema.getType(implRef->typeId);

  // create an input on the Runtime Type for every input in the schema's Type
  for(auto input : schemaTypeRef->inputs) {
    builder.addInPort(input->id)->apply([implRef, input](Implementation& imp, Port& inputPort){
      // establish all connection made to this port according to the schema's implementation
      withEachPortConnectedToInput(input->id, imp, implRef, [&inputPort](Port& p){
        // std::cout << "withEachPortConnectedToInput" << std::endl;
        // inputPort.signal.connect([&p](const void* dummy){
        //     std::cout << "INPUT INPUT: " << p.getId() << std::endl;
        // });
        Port::connect(inputPort, p);
      });
    });
  }

  // create an output on the Runtime Type for every output in the schema's Type
  for(auto output : schemaTypeRef->outputs) {
    // std::cout << "Implementation output: " << output->id << std::endl;
    builder.addOutPort(output->id)->apply([output,implRef](Implementation& imp, Port& outputPort){
      // establish all connection made to this port according to the schema's implementation
      withEachPortConnectedToOutput(output->id, imp, implRef, [&outputPort](Port& p){
        // std::cout << "withEachPortConnectedToOutput" << std::endl;
        Port::connect(p, outputPort);
      });
    });
  }
}

Implementation::~Implementation() {
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

void Implementation::withEachPortConnectedToInput(const Schema::Id& inputId, Implementation& imp, Schema::ImplementationRef implRef, std::function<void(Port& p)> func) {

  for(auto connRef : implRef->connections) {
    
    if (connRef->output.instanceId == implRef->id && connRef->output.portId == inputId) {
      
      auto pReceiver = imp.findInstanceForSchemaId(connRef->input.instanceId);

      if (!pReceiver) {
        std::cerr << "Implementation::withEachPortConnectedToInput could not find receiving instance for id: " << connRef->input.instanceId << std::endl;
        continue;
      }

      auto portRef = pReceiver->getInput(connRef->input.portId);

      if (!portRef) {
        std::cerr << "Implementation::withEachPortConnectedToInput could not find port: " << connRef->input.portId << "(for instance: " << connRef->input.instanceId << ")" << std::endl;
        continue;
      }

      func(*portRef);
    }
  }
}

void Implementation::withEachPortConnectedToOutput(const Schema::Id& outputId, Implementation& imp, Schema::ImplementationRef implRef, std::function<void(Port& p)> func) {
  for(auto connRef : implRef->connections) {
    if (connRef->input.instanceId == implRef->id && connRef->input.portId == outputId) {

      // find the port that receives from this connection 
      auto pInstance = imp.findInstanceForSchemaId(connRef->output.instanceId);

      if (!pInstance) {
        std::cerr << "Implementation::withEachPortConnectedToOutput could not find sending instance for id: " << connRef->output.instanceId << std::endl;
        continue;
      }

      auto portRef = pInstance->getOutput(connRef->output.portId);

      if (!portRef) {
        std::cerr << "Implementation::withEachPortConnectedToOutput could not find port: " << connRef->output.portId << "(for instance: " << connRef->output.instanceId << ")" << std::endl;
        continue;
      }

      func(*portRef);
    }
  }
}

void Implementation::applyInternalSchemaConnections(Implementation& implementation, Schema::ImplementationRef implRef) {
  auto pImp = &implementation;

  //
  // Apply all connections in the schema implementation
  //

  for(auto connRef : implRef->connections) {
    // connection's that involve the implementation's own inputs/outputs will be established
    // in the build logic, when the Instance's ports are created
    if (connRef->input.instanceId == implRef->id || connRef->output.instanceId == implRef->id) continue;

    // std::cout << "conn: " << connRef->output.portId << " --> " << connRef->input.portId << std::endl;

    auto receiverRef = implementation.findInstanceForSchemaId(connRef->input.instanceId);
    auto receiverPort = receiverRef ? receiverRef->getInput(connRef->input.portId) : nullptr;

    auto senderRef = implementation.findInstanceForSchemaId(connRef->output.instanceId);
    auto senderPort = senderRef ? senderRef->getOutput(connRef->output.portId) : nullptr;

    if (!receiverPort) {
      std::cout << "Could not connect schema instance because of missing receiver port: " << connRef->input.portId << "(for: "<< connRef->input.instanceId << ")" <<std::endl;
      continue;
    }

    if (!senderPort) {
      std::cout << "Could not connect schema instance because of missing sender port: " << connRef->output.portId << "(for: "<< connRef->output.instanceId << ")" << std::endl;
      continue;
    }

    //
    // Connect Ports
    //

    auto signalConnection = Port::connect(*senderPort, *receiverPort, true /* check type */);
    pImp->signalConnections.push_back(signalConnection);
  }
}

Instance* Implementation::findInstanceForSchemaId(const Schema::Id& id) {
  auto it = schemaIdToInstanceMap.find(id);
  if (it != schemaIdToInstanceMap.end()) return it->second;
  return NULL;
}
