#include "catch.hpp"

#include <iostream>
#include <memory>
#include "info/Runtime.h"
// #include "cinder/app/App.h" // for CINDER_MSW macro
#include "cinder/Signals.h"


TEST_CASE("info::Runtime", ""){

  SECTION("addType"){
    info::Runtime runtime;

    REQUIRE(runtime.getTypes().size() == 0);
    
    auto type = runtime.addType<bool>("bool", [](info::TypeBuilder<bool>& builder){
    });

    REQUIRE(runtime.getTypes().size() == 1);
    REQUIRE(runtime.getTypes()[0]->getId() == "bool");
  }

  SECTION("createInstance"){
    info::Runtime runtime;

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

    std::shared_ptr<info::Instance> instanceRef = runtime.createInstance("bool");

    bool feedback = false;
    instanceRef->port<bool>("value")->onDataOut([&feedback](const bool& val){
      feedback = val;
    });

    REQUIRE(feedback == false);
    instanceRef->port<bool>("value")->dataIn(true);
    REQUIRE(feedback == true);
    instanceRef->port<bool>("value")->dataIn(false);
    REQUIRE(feedback == false);
  }
}