#pragma once

#include "info/Port.hpp";
#include "info/Type.h";

namespace info { namespace components {

  template<typename S, typename D>
  class Conversion {
    public: // types

      typedef std::function<D(S)> ConversionReturnFunc;

    public: // static methods

      static void addToRuntime(Runtime& runtime, const std::string& typeName, ConversionReturnFunc func) {
        runtime.addType<Conversion<S,D>>(typeName,
          // instantiator func
          [func](){ return new Conversion<S,D>(func); },
          // builder func
          getBuilderFunc());
      }

      static std::function<void(TypeBuilder<Conversion<S,D>>&)> getBuilderFunc() {
        return [](info::TypeBuilder<Conversion<S,D>>& builder){

          // // connect input port to member method WITH argument
          builder.template input<S>("convert")->apply(&Conversion<S,D>::convert);

          // // connect outputs ports to member signals
          builder.template output<D>("conversion")->apply(&Conversion<S,D>::conversionSignal);
        };
      }

    public:
      Conversion(std::function<D(S)> conversionFunc) : conversionFunc(conversionFunc){}

      void convert(const S& val) {
        if (conversionFunc != nullptr) {
          D conv = conversionFunc(val);
          conversionSignal.emit(&conv);
        }
      }

    public:
      Port::Signal conversionSignal;

    private:
      ConversionReturnFunc conversionFunc;
  };

}}
