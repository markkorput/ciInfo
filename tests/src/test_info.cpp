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

TEST_CASE("info::functions", ""){

  SECTION("Usage in app") {
    // main
    info::Runtime runtime;
    initFunctionsRuntime(runtime);

    info::Schema schema;
    auto instanceRef = info::createSchemaInstance(runtime, schema, "Platform/HelloWorldApp");
    REQUIRE(instanceRef == nullptr);
  }
}