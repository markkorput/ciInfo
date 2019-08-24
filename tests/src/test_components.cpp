#include "catch.hpp"

#include <iostream>
#include <memory>
#include "info/Runtime.h"
#include "info/components.h"

using namespace info;

TEST_CASE("info::components", ""){
  SECTION("float and string"){
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

  SECTION("int, int2string, string"){
    RuntimeRef runtimeRef = info::getDefaultRuntime();

    auto intInstanceRef = runtimeRef->createInstance("int");
    auto int2stringInstanceRef = runtimeRef->createInstance("int_to_string");

    std::string result = "";

    intInstanceRef->getOutput("init")->onData<int>([int2stringInstanceRef](const int& v){
      int2stringInstanceRef->getInput("convert")->sendData<int>(v);
    });

    int2stringInstanceRef->getOutput("conversion")->onData<std::string>([&result](const std::string& v){
      result = v;
    });

    REQUIRE(result == "");
    intInstanceRef->getInput("value")->sendData<int>(25);
    REQUIRE(result == "25");
  }
}