#pragma once


#include "Runtime.h"
#include "Schema.h"
#include "Instance.h"
#include "Type.h"
#include "Implementation.h"

namespace info {
  /// creates and returns a new Runtime instance with all of the given runtime's types
  /// as well as new types added based on the implementations in the given schema
  RuntimeRef mergeSchemaTypes(Runtime& runtime, Schema& schema) {
    // create new runtime
    auto newRuntime = std::make_shared<Runtime>();
    // copy all existing types in the given runtime
    Runtime::copyTypes(runtime, *newRuntime);

    for(auto implementationRef : schema.implementationRefs) {
      // add type to new runtime
      newRuntime->addType<Implementation>(implementationRef->id,
        // instantiator
        [newRuntime, &schema, implementationRef](TypeRef typeRef) {
          auto instanceRef = Implementation::instantiate(*newRuntime, schema, implementationRef->id, typeRef);
          return instanceRef;
        },

        // builder
        [&schema, implementationRef](TypeBuilder<Implementation>& builder) {
          Implementation::build(builder, schema, implementationRef->id);
        });
    }

    return newRuntime;
  }
}