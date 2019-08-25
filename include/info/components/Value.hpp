#pragma once

#include "info/Port.hpp";
#include "info/Type.h";

namespace info { namespace components {
  template<typename V>
  class Value {

    public:
      const V& get() const { return value; }

      void set(const V& v) {
        bool change = this->value != v;
        bool init = false;

        if (!isSet) {
          isSet = true;
          init = true;
          change = true;
        } 

        value = v;
        setSignal.emit(&value);

        if (init) initSignal.emit(&value);
        if (change) changeSignal.emit(&value);
      }

      void reset() {
        if (!isSet) return;
        this->isSet = false;
        this->resetSignal.emit(NULL);
      }

      void fire() {
        // std::cout << "FIRE FIRE" << std::endl;
        fireSignal.emit(&value);
      }

    public:
      Port::Signal setSignal;
      Port::Signal changeSignal;
      Port::Signal initSignal;
      Port::Signal resetSignal;
      Port::Signal fireSignal;

    private:
      V value;
      bool isSet = false;

    public:

      static TypeRef createType(const std::string& typeName) {
        return info::Type::create<Value<V>>(typeName, getBuilderFunc());
      }

      static void addToRuntime(Runtime& runtime, const std::string& typeName) {
        runtime.addType<Value<V>>(typeName, getBuilderFunc());
      }

      static std::function<void(TypeBuilder<Value<V>>&)> getBuilderFunc() {
        return [](info::TypeBuilder<Value<V>>& builder){

          // connect inputs to argumentless member methods
          builder.addInPort("fire")->apply(&Value<V>::fire);
          builder.addInPort("reset")->apply(&Value<V>::reset);

          // connect input port to member method WITH argument
          builder.template input<V>("value")->apply(&Value<V>::set);

          // connect outputs ports to member signals
          builder.template output<V>("set")->apply(&Value<V>::initSignal);
          builder.template output<V>("change")->apply(&Value<V>::changeSignal);
          builder.template output<V>("init")->apply(&Value<V>::initSignal);
          builder.template output<V>("reset")->apply(&Value<V>::resetSignal);
          builder.template output<V>("fire")->apply(&Value<V>::fireSignal);
        };
      }
  };
}}