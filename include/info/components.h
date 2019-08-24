#pragma once

#include <iostream>
#include <memory>
#include "Runtime.h"
#include "components/Value.hpp"
#include "components/Conversion.hpp"
#include "boost/lexical_cast.hpp"

namespace info {

  void addDefaultTypesToRuntime(Runtime& runtime) {
    components::Value<bool>::addToRuntime(runtime, "bool");
    components::Value<int>::addToRuntime(runtime, "int");
    components::Value<long>::addToRuntime(runtime, "long");
    components::Value<char>::addToRuntime(runtime, "char");
    components::Value<std::string>::addToRuntime(runtime, "string");
    components::Value<float>::addToRuntime(runtime, "float");
    components::Value<double>::addToRuntime(runtime, "double");

    // ...-to-string
    components::Conversion<int, std::string>::addToRuntime(runtime, "int_to_string", 
      [](int val){ 
        auto v = std::to_string(val); 
        // std::cout << "int_to_string CONVERTING:" << val << "->" << v<< std::endl;
        return v;});
    components::Conversion<float, std::string>::addToRuntime(runtime, "float_to_string", 
      [](float val){ return std::to_string(val); });
    components::Conversion<char, std::string>::addToRuntime(runtime, "char_to_string", 
      [](char val){ return std::to_string(val); });
    components::Conversion<bool, std::string>::addToRuntime(runtime, "bool_to_string", 
      [](bool val){ return std::to_string(val); });
    components::Conversion<double, std::string>::addToRuntime(runtime, "double_to_string", 
      [](double val){ return std::to_string(val); });
    components::Conversion<long, std::string>::addToRuntime(runtime, "long_to_string", 
      [](long val){ return std::to_string(val); });
    components::Conversion<float, int>::addToRuntime(runtime, "float_to_int", 
      [](float val){ return (int)val; });
    
    // string-to-...
    components::Conversion<std::string, int>::addToRuntime(runtime, "string_to_int", 
      [](std::string val){
        try {
          return std::stoi(val);
        } catch(std::invalid_argument exc){
          // std::cerr << exc.what();
        }
      });

    components::Conversion<std::string, float>::addToRuntime(runtime, "string_to_float", 
      [](std::string val){
        try {
          return std::stof(val);
        } catch(std::invalid_argument exc){
          // std::cerr << exc.what();
        }
      });

    components::Conversion<std::string, bool>::addToRuntime(runtime, "string_to_bool", 
      [](std::string val){
        try {
          return boost::lexical_cast<bool>(val);
        } catch(boost::bad_lexical_cast exc){
          // std::cerr << exc.what();
        }
      });
  }

  RuntimeRef getDefaultRuntime() {
    auto ref = std::make_shared<Runtime>();
    addDefaultTypesToRuntime(*ref);
    return ref;
  }
}