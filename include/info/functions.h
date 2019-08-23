#pragma once


#include "Runtime.h"
#include "Schema.h"
#include "Instance.h"

namespace info {
  InstanceRef createSchemaInstance(Runtime& runtime, Schema& schema, const std::string& componentId) {
    // check if there's an implementation for the specified type

    // instantiate schema implementation


    return nullptr;
  }

  RuntimeRef mergeSchemaTypes(Runtime& runtime, Schema& schema) {
    auto ref = std::make_shared<Runtime>();
    Runtime::copyTypes(runtime, *ref);

    return ref;
  }
}