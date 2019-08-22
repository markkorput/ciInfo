#pragma once

#include <stdlib.h>
#include "TypeBuilder.hpp"
#include "Type.h"

namespace info {
  class Runtime {
    public:
      template<class T>
      std::shared_ptr<Type> addType(const std::string& typeId, std::function<void(TypeBuilder<T>&)> builderFunc) {
        auto typeRef = Type::create(typeId, builderFunc);
        typeRefs.push_back(typeRef);
        return typeRef;
      }

      const std::vector<std::shared_ptr<Type>>& getTypes() const {
        return typeRefs;
      }

    private:
      std::vector<std::shared_ptr<Type>> typeRefs;
  };
}