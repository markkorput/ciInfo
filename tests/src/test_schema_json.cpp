#include "catch.hpp"

#include <iostream>
#include <memory>

#include "info/Schema.h"
#include "info/util/SchemaJson.h"

using namespace info;
using namespace info::util;

TEST_CASE("info::util::SchemaJson", ""){
  SECTION("serialize") {
    Schema schema;
    auto app = schema.createImplementation("MyFirstImplementation");
    auto stringInst = schema.createInstance(app, "string");
    auto stringInst2 = schema.createInstance(app, "string2");
    schema.createConnection(app, stringInst, "fire", stringInst2, "value");

    auto json = SchemaJson::serialize(schema);
    REQUIRE(json ==
R"V0G0N({
  "implementations" : [
    {
      "connections" : [
        {
            "id" : 0,
            "receiver_instance" : "1",
            "receiver_port" : "value",
            "sender_instance" : "0",
            "sender_order" : 0,
            "sender_port" : "fire"
        }
      ],
      "initial_values" : null,
      "instances" : [
        {
            "id" : "0",
            "name" : "string",
            "type" : "string"
        },
        {
            "id" : "1",
            "name" : "string2",
            "type" : "string2"
        }
      ]
    }
  ]
})V0G0N");
  }
}