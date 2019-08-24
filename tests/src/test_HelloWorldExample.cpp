#include "catch.hpp"

#include <iostream>
#include <memory>

#include "info/Runtime.h"
#include "info/Schema.h"
#include "info/functions.h"
#include "info/components/Value.hpp"

std::vector<std::string> printedValues;

void initHelloWorldNativeRuntime(info::Runtime& runtime) {

  info::components::Value<std::string>::addToRuntime(runtime, "string");

  runtime.addType<bool>("Printer", [](info::TypeBuilder<bool>& builder){
    builder.input<std::string>("print")
      ->onData([](const std::string& value){
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

  schema.createInitialValue<std::string>(app, messageInstance, "value", "Hello World!");
}

TEST_CASE("Examples", ""){
  SECTION("HelloWorld") {
    // main
    info::Runtime nativeRuntime;
    initHelloWorldNativeRuntime(nativeRuntime);

    info::Schema schema;
    initHelloWorldSchema(schema);

    info::RuntimeRef schemaRuntimeRef = mergeSchemaTypes(nativeRuntime, schema);

    // // std::cout << "before: " << schemaRuntimeRef->getInstances().size() << std::endl;
    auto instanceRef = schemaRuntimeRef->createInstance("HelloWorldApp");
    REQUIRE(instanceRef != nullptr);
    // // std::cout << "after: " << schemaRuntimeRef->getInstances().size() << std::endl;
    REQUIRE(schemaRuntimeRef->getInstances().size() == 3); // The HellowWorldApp, the Printer and the string
    
    
    auto pStartPort = instanceRef->getInput("start");
    REQUIRE(pStartPort);
    pStartPort->sendSignal();
    REQUIRE(printedValues.size() == 1);
    REQUIRE(printedValues[0] == "Hello World!");
    pStartPort->sendSignal();
    pStartPort->sendSignal();
    REQUIRE(printedValues.size() == 3);
  }
}