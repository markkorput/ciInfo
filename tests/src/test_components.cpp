#include "catch.hpp"

#include <iostream>
#include <memory>
#include "info/Runtime.h"
#include "info/components.h"

using namespace info;

TEST_CASE("info::components", ""){
  SECTION("Value<T>"){
    RuntimeRef runtimeRef = info::getDefaultRuntime();

    auto floatInstanceRef = runtimeRef->createInstance("float");
    auto stringInstanceRef = runtimeRef->createInstance("string");

    std::string result = "";

    stringInstanceRef->getOutput("init")->onData<std::string>([&result](const std::string& v){
      result = v;
    });

    // float "fire" output send float value to string instance
    floatInstanceRef->getOutput("fire")->onData<float>([stringInstanceRef](const float& v){
      stringInstanceRef->getInput("value")->sendData<std::string>(std::to_string(v));
    });

    REQUIRE(result == "");
    floatInstanceRef->getInput("value")->sendData<float>(33.33f);
    REQUIRE(result == "");

    floatInstanceRef->getInput("fire")->sendSignal();
    REQUIRE(result.rfind("33.33", 0) == 0);
  }
}