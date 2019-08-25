#pragma once

#include "Runtime.h"
#include "Schema.h"
#include "Instance.h"
#include "Type.h"
#include "Implementation.h"


namespace info {
  /// creates and returns a new Runtime instance with all of the given runtime's types
  /// as well as new types added based on the implementations in the given schema
  RuntimeRef mergeSchemaTypes(Runtime& runtime, Schema& schema);
  
  void addDefaultTypesToRuntime(Runtime& runtime);
  RuntimeRef getDefaultRuntime();
}