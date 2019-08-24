#pragma once

// #include "ctree/signal.hpp"
// #include "ciCMS/cfg/Cfg.h"
#include <stdlib.h>
#include <string>
#include <memory>
#include "cinder/Signals.h"

namespace info {

  class Port;
  typedef std::shared_ptr<Port> PortRef;

  class Port {
    
    public:
      typedef cinder::signals::Signal<void(const void*)> Signal;

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

      // cinder::signals::Connection inputTo(Port& outputPort, bool performTypeCheck = false) {
      //   // return dummy connection if types don't match. TODO; log warning?
      //   if (performTypeCheck && outputPort.type != this->type) return cinder::signals::Connection();
      //   return Port::connect(this->outSignal, outputPort.inSignal);
      // }

      cinder::signals::Connection outputTo(Port& inputPort, bool performTypeCheck = false) {
        // return dummy connection if types don't match. TODO; log warning?
        if (performTypeCheck && inputPort.type != this->type) return cinder::signals::Connection();
        return Port::connect(this->outSignal, inputPort.inSignal);
      }

      cinder::signals::Connection outputTo(Signal& signal) {
        return Port::connect(outSignal, signal);
      }

      cinder::signals::Connection inputFrom(Signal& signal) {
        return Port::connect(signal, this->inSignal);
      }

      // cinder::signals::Connection inputTo(Signal& signal) {
      //   return Port::connect(inSignal, signal);
      // }

      cinder::signals::Connection outputFrom(Signal& signal) {
        return Port::connect(signal, this->outSignal);
      }

      inline static cinder::signals::Connection connect(Port& output, Port& input, bool performTypeCheck = false) {
        // return dummy connection if types don't match. TODO; log warning?
        if (performTypeCheck && input.type != output.type) return cinder::signals::Connection();
        return connect(output.outSignal, input.inSignal);
      }

      inline static cinder::signals::Connection connect(Signal& output, Signal& input) {
        return output.connect([&input](const void* arg){
          input.emit(arg);
        });
      }

    protected:

      inline InFuncVoid toVoid(InFuncNoArg func) { 
        return [func](const void* arg) {
          func();
        };
      }

    public:
      Signal inSignal;
      Signal outSignal;

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
