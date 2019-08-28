#include "catch.hpp"

#include <iostream>
#include <memory>
#include "info/Runtime.h"
#include "info/Port.hpp"
#include "cinder/Signals.h"


void initRuntime(info::Runtime& runtime) {
  auto signalRef = std::make_shared<info::Port::Signal>();

  auto type = runtime.addType<bool>("bool", [signalRef](info::TypeBuilder<bool>& builder){
    builder.input<bool>("value")
      ->apply([signalRef](bool& instance, info::Port& port) {
        // when data comes in through the value-IN-port; apply that value
        port.onData<bool>([signalRef, &instance](const bool& val){
          if (val == instance) return;
          instance = val;
          signalRef->emit(&instance);
        });
      });

    builder.output<bool>("value")->apply(*signalRef);
      // ->apply([signalRef](bool& instance, info::Port& port) {
      //   port.inputFrom(*signalRef);
      // });
  });
}

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
    initRuntime(runtime);

    std::shared_ptr<info::Instance> instanceRef = runtime.createInstance("bool");

    bool feedback = false;
    instanceRef->getOutput("value")->onData<bool>([&feedback](const bool& val){
      feedback = val;
    });

    REQUIRE(feedback == false);
    instanceRef->getInput("value")->sendData(true);
    REQUIRE(feedback == true);
    instanceRef->getInput("value")->sendData(false);
    REQUIRE(feedback == false);
  }
}