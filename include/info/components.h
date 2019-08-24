#pragma once

#include <memory>
#include "Runtime.h"
#include "components/Value.hpp"

namespace info {
  RuntimeRef getDefaultRuntime() {
    auto ref = std::make_shared<Runtime>();
    components::Value<bool>::addToRuntime(*ref, "bool");
    components::Value<int>::addToRuntime(*ref, "int");
    components::Value<long>::addToRuntime(*ref, "long");
    components::Value<char>::addToRuntime(*ref, "char");
    components::Value<std::string>::addToRuntime(*ref, "string");
    components::Value<float>::addToRuntime(*ref, "float");
    components::Value<double>::addToRuntime(*ref, "double");
    return ref;
  }
}