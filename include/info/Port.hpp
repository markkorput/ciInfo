#pragma once

// #include "ctree/signal.hpp"
// #include "ciCMS/cfg/Cfg.h"
#include <stdlib.h>
#include <string>
#include "cinder/Signals.h"

namespace info {

  class Port {
    public:
      const static int FLAG_IN = 1;
      const static int FLAG_OUT = (1 >> 1);
      const static int FLAG_INOUT = (FLAG_IN & FLAG_OUT);
      
      typedef std::function<void(const void*)> InFuncVoid;
      typedef std::function<void()> InFuncNoArg;

    public:

      Port(const std::string& id, const std::string& type, int flags = FLAG_INOUT) : id(id), type(type), flags(flags) {
      }
  
    public:
      const std::string& getId() const { return id; }
      const std::string& getType() const { return type; }

      template<typename V>
      void dataOut(const V& val) {
        outSignal.emit((const void*)&val);
      }

      template<typename V>
      void dataIn(const V& val) {
        inSignal.emit((const void*)&val);
      }

      void signalOut() {
        outSignal.emit(NULL);
      }

      void signalIn() {
        inSignal.emit(NULL);
      }

      cinder::signals::Connection onInput(InFuncNoArg func) {
        return inSignal.connect(toVoid(func));
      }

      cinder::signals::Connection onOutput(InFuncNoArg func) {
        return outSignal.connect(toVoid(func));
      }

    protected:

      inline InFuncVoid toVoid(InFuncNoArg func) { 
        return [func](const void* arg) {
          func();
        };
      }

    public:
      cinder::signals::Signal<void(const void*)> inSignal;
      cinder::signals::Signal<void(const void*)> outSignal;

    private:
      std::string id;
      std::string type;
      int flags;
  };

  template<typename V>
  class TypedPort : public Port {
    
    public:
      typedef std::function<void(const V&)> InFuncTypeRef;

    public:
      TypedPort(const std::string& id, int flags = Port::FLAG_INOUT) : Port(id, typeid(V).name(), flags) {
      }

      void dataIn(const V& val) {
        Port::dataIn<V>(val);
      }

      void dataOut(const V& val) {
        Port::dataOut<V>(val);
      }

      cinder::signals::Connection onDataIn(InFuncTypeRef func) {
        return inSignal.connect(toVoid(func));
      }

      cinder::signals::Connection onDataOut(InFuncTypeRef func) {
        return outSignal.connect(toVoid(func));
      }

      private:

        inline InFuncVoid toVoid(InFuncTypeRef func) { 
          return [func](const void* arg){
            func(*(const V*)arg);
          };
        }
  };

}
