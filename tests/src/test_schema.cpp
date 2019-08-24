#include "catch.hpp"

#include <iostream>
#include <memory>

#include "info/Schema.h"

void _init(info::Schema& schema) {
  // this implicitly should create the type "HelloWorldApp"
  auto app = schema.createImplementation("HelloWorldApp");
  auto messageInstance = app->createInstance("string", "Message");
  auto printInstance = app->createInstance("Printer", "Printer");

  // create connection of the "fired" message to the printer
  app->createConnection(messageInstance, "fire", printInstance, "print");

  // create connection between the app's "start" input port
  // and the message's "fire" input port.
  app->createConnection("start", messageInstance, "fire");
}

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
}