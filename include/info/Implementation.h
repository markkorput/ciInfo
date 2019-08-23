#pragma once

#include "Runtime.h"
#include "Schema.h"
#include "Type.h"
#include "Instance.h"
#include "TypeBuilder.hpp"

namespace info {
  class Implementation {
    public:
      static InstanceRef instantiate(Runtime& runtime, Schema& schema, const std::string& implementationId) {
        return nullptr;
      }

      static void build(TypeBuilder<Implementation>& builder) {

      }
  };
}