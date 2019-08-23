#include "catch.hpp"

#include <iostream>
#include <memory>

#include "info/Runtime.h"
#include "info/Schema.h"
#include "info/functions.h"

void initFunctionsRuntime(info::Runtime& runtime) {
  auto type = runtime.addType<bool>("bool", [](info::TypeBuilder<bool>& builder){
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
}

void initSchema(info::Schema& schema) {
  //schema.root().add("HellowWorldApp");
  {
    auto app = schema.createImplementation("HelloWorldApp");
    auto messageInstance = app->createInstance("string", "Message");
    auto printInstance = app->createInstance("print", "Printer");

    // create connection of the "fired" message to the printer
    app->createConnection("fire", printInstance, "print");

    // create connection that "fires" the message
    app->createConnection(app, "start", messageInstance, "fire");
  }
}

TEST_CASE("info::functions", ""){

  SECTION("createSchemaInstance") {
    // main
    info::Runtime nativeRuntime;
    initFunctionsRuntime(nativeRuntime);

    info::Schema schema;
    initSchema(schema);

    info::RuntimeRef schemaRuntimeRef = mergeSchemaTypes(nativeRuntime, schema);

    auto instanceRef = schemaRuntimeRef->createInstance("HelloWorldApp");
    REQUIRE(instanceRef != nullptr);
    REQUIRE(schemaRuntimeRef->getInstances().size() == 1);
    auto pStartPort = instanceRef->signalPort("start");
    REQUIRE(pStartPort != NULL);
    pStartPort->signalOut();
  }
}