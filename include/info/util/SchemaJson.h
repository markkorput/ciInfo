#pragma once

#include "cinder/app/App.h"
#include "info/Schema.h"

namespace info { namespace util {
  class SchemaJson {
    public:
      static std::string serialize(const Schema& schema);
      // static SchemaRef deserialize(const std::string& json);
      static void write_serialized(const Schema& schema, const ci::fs::path& path);
  };
}}