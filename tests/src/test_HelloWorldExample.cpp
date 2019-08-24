#include "catch.hpp"

#include <iostream>
#include <memory>

#include "info/Runtime.h"
#include "info/Schema.h"
#include "info/functions.h"

std::vector<std::string> printedValues;

void initHellowWorldNativeRuntime(info::Runtime& runtime) {
  runtime.addType<bool>("bool", [](info::TypeBuilder<bool>& builder){
    builder.attr<bool>("value")
      ->apply([](bool& instance, info::TypedPort<bool>& port) {
        // when data comes in through the value-IN-port; apply that value
        port.onDataIn([&instance, &port](const bool& val){
          if (val == instance) return;
          instance = val;
            // immediately send the new value to the out port
          port.dataOut(instance);
        });
      });
  });

  runtime.addType<std::string>("string", [](info::TypeBuilder<std::string>& builder){
    builder.attr<std::string>("fire")
      ->apply([](std::string& instance, info::TypedPort<std::string>& port) {
        port.onInput([&instance, &port](){
            port.dataOut(instance);
        });
      });
  });

  runtime.addType<bool>("Printer", [](info::TypeBuilder<bool>& builder){
    builder.input<std::string>("print")
      ->onDataIn([](const std::string& value){
          std::cout << " ### PRINTER printing: " << value << std::endl;
          printedValues.push_back(value);
      });
  });
}

void initHelloWorldSchema(info::Schema& schema) {
  auto app = schema.createImplementation("HelloWorldApp");
  auto messageInstance = schema.createInstance(app, "string", "Message");
  auto printInstance = schema.createInstance(app, "Printer", "Printer");

  // create connection of the "fired" message to the printer
  schema.createConnection(app, messageInstance, "fire", printInstance, "print");

  // create connection that "fires" the message when the app's input "start" port is triggered
  schema.createConnection(app, "start", messageInstance, "fire");
}

TEST_CASE("Examples", ""){
  SECTION("HelloWorld") {
    // main
    info::Runtime nativeRuntime;
    initHellowWorldNativeRuntime(nativeRuntime);

    info::Schema schema;
    initHelloWorldSchema(schema);

    info::RuntimeRef schemaRuntimeRef = mergeSchemaTypes(nativeRuntime, schema);

    // std::cout << "before: " << schemaRuntimeRef->getInstances().size() << std::endl;
    auto instanceRef = schemaRuntimeRef->createInstance("HelloWorldApp");
    // std::cout << "after: " << schemaRuntimeRef->getInstances().size() << std::endl;
    REQUIRE(schemaRuntimeRef->getInstances().size() == 3); // The HellowWorldApp, the Printer and the string
    REQUIRE(instanceRef != nullptr);
    
    auto pStartPort = instanceRef->signalPort("start");
    REQUIRE(pStartPort != NULL);
    pStartPort->signalIn();
    REQUIRE(printedValues.size() == 1);
    REQUIRE(printedValues[0] == "Hello World!");
    pStartPort->signalIn();
    pStartPort->signalIn();
    REQUIRE(printedValues.size() == 3);
  }
}