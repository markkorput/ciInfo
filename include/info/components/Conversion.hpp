#pragma once

#include "info/Port.hpp";
#include "info/Type.h";

namespace info { namespace components {

  template<typename S, typename D>
  class Conversion {
    public: // types

      typedef std::function<D(S)> ConversionReturnFunc;
      typedef std::function<bool(const S&, D&)> ConversionSuccessFunc;

    public: // static methods

      static void addToRuntime(Runtime& runtime, const std::string& typeName, ConversionSuccessFunc func) {
        runtime.addType<Conversion<S,D>>(typeName,
          // instantiator func
          [func](){ return new Conversion<S,D>(func); },
          // builder func
          getBuilderFunc());
      }

      static void addToRuntime(Runtime& runtime, const std::string& typeName, ConversionReturnFunc func) {
        addToRuntime(runtime, typeName, [func](const S& val, D& out){
          try {
            out = func(val);
          } catch(std::invalid_argument exc){
            return false;
          } catch (std::exception exc) {
            return false;
          }

          return true;
        });
      }

      static std::function<void(TypeBuilder<Conversion<S,D>>&)> getBuilderFunc() {
        return [](info::TypeBuilder<Conversion<S,D>>& builder){

          // // connect input port to member method WITH argument
          builder.template input<S>("convert")->apply(&Conversion<S,D>::convert);

          // // connect outputs ports to member signals
          builder.template output<D>("conversion")->apply(&Conversion<S,D>::conversionSignal);
          builder.addOutPort("failure")->apply(&Conversion<S,D>::failureSignal);
        };
      }

    public:
      Conversion(ConversionSuccessFunc conversionFunc) : conversionFunc(conversionFunc){}

      void convert(const S& val) {
        if (conversionFunc != nullptr) {
          D conv;
          bool success = conversionFunc(val,conv);
          if (success)
            conversionSignal.emit(&conv);
          else 
            failureSignal.emit(&conv);
        }
      }

    public:
      Port::Signal conversionSignal;
      Port::Signal failureSignal;

    private:
      ConversionSuccessFunc conversionFunc;
  };

}}
