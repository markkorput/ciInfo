#include "catch.hpp"

#include <iostream>
#include <memory>

#include "info/Schema.h"

// void _init(info::Schema& schema) {
//   // this implicitly should create the type "HelloWorldApp"
//   auto app = schema.createImplementation("HelloWorldApp");
//   // also creates the type "string" if it didn't exist yet
//   auto messageInstance = schema->createInstance(app, "string", "Message");
//   auto printInstance = schema->createInstance(app, "Printer", "Printer");

//   // create connection of the "fired" message to the printer
//   schema.createConnection(messageInstance, "fire", printInstance, "print");

//   // create connection between the app's "start" input port
//   // and the message's "fire" input port.
//   schema.createConnection("start", messageInstance, "fire");
// }

using namespace info;

TEST_CASE("info::Schema", ""){
  SECTION("createImplementation") {
    Schema schema;
    auto app = schema.createImplementation("MyFirstImplementation");
    REQUIRE(app->id == "MyFirstImplementation");
    REQUIRE(schema.implementationRefs.size() == 1);
    REQUIRE(schema.implementationRefs[0] == app);

    REQUIRE(schema.typeRefs.size() == 1);
    REQUIRE(schema.typeRefs[0]->id == app->id);
  }

  SECTION("createImplementation fails on duplicate ID") {
    Schema schema;
    auto imp = schema.createImplementation("UniqueId");
    auto imp2 = schema.createImplementation("UniqueId");
    REQUIRE(imp != nullptr);
    REQUIRE(imp2 == nullptr);
  }


  SECTION("createInstance") {
    Schema schema;
    // create SomeApp implementation
    auto app = schema.createImplementation("SomeApp");
    // add SomeObject instance to SomeApp
    auto inst = schema.createInstance(app, "SomeObject", "Object#1");
    
    // verify the implementation contains the instance data
    REQUIRE(schema.implementationRefs[0]->instances.size() == 1);
    REQUIRE(schema.implementationRefs[0]->instances[0]->id == "0");
    REQUIRE(schema.implementationRefs[0]->instances[0]->typeId == "SomeObject");
    REQUIRE(schema.implementationRefs[0]->instances[0]->name == "Object#1");

    // verify types for both the implementation and the object are created
    REQUIRE(schema.typeRefs.size() == 2);
    REQUIRE(schema.typeRefs[0]->id == "SomeApp");
    REQUIRE(schema.typeRefs[1]->id == "SomeObject");
  }
  
  SECTION("createConnection") {
    Schema schema;
    // create SomeApp implementation
    auto app = schema.createImplementation("StartApp");
    // add SomeObject instance to SomeApp
    auto obj = schema.createInstance(app, "StartObject");
    schema.createConnection(app, "startup", obj, "begin");

    REQUIRE(schema.typeRefs.size() == 2);
    REQUIRE(schema.typeRefs[0]->id == "StartApp");
    REQUIRE(schema.typeRefs[0]->inputs.size() == 1);
    REQUIRE(schema.typeRefs[0]->inputs[0]->id == "startup");

    REQUIRE(schema.typeRefs[1]->id == "StartObject");
    REQUIRE(schema.typeRefs[1]->inputs.size() == 1);
    REQUIRE(schema.typeRefs[1]->inputs[0]->id == "begin");
  }
}