#pragma once

#include "info/Port.hpp";
#include "info/Type.h";

namespace info { namespace components {
  template<typename V>
  class Value {

    public:

      static TypeRef createType(const std::string& typeName) {
        return info::Type::create<Value<V>>(typeName, getBuilderFunc());
      }

      static void addToRuntime(Runtime& runtime, const std::string& typeName) {
        runtime.addType<Value<V>>(typeName, getBuilderFunc());
      }

      static std::function<void(TypeBuilder<Value<V>>&)> getBuilderFunc() {
        return [](info::TypeBuilder<Value<V>>& builder){

          // INPUTS

          builder.template input<V>("value")
            ->apply([](Value<V>& instance, Port& port) {
              port.onData<V>([&instance](const V& v){
                instance.set(v);
              });
            });

          builder.addInPort("fire")
            ->apply([](Value<V>& instance, Port& port) {
              port.onSignal([&instance]() {
                instance.fire();
              });
            });

          // OUTPUTS

          builder.template output<V>("fire")
            ->apply([](Value<V>& instance, Port& port) {
              port.inputFrom(instance.fireSignal);
            });
        };
      }

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

        if (init) initializeSignal.emit(&value);
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
      Port::Signal initializeSignal;
      Port::Signal resetSignal;
      Port::Signal fireSignal;

    private:
      V value;
      bool isSet = false;
  };
}}