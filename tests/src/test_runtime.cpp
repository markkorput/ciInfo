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
}