#pragma once

// #include "ctree/signal.hpp"
// #include "ciCMS/cfg/Cfg.h"
#include <stdlib.h>
#include <iostream>
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
      const static int FLAG_OUT = (1 << 1);
      // const static int FLAG_INOUT = (FLAG_IN & FLAG_OUT);
      
      typedef std::function<void(const void*)> InFuncVoid;
      typedef std::function<void()> InFuncNoArg;

    public: // constructor

      Port(const std::string& id, const std::string& type, int flags = FLAG_IN) : id(id), type(type), flags(flags) {
      }

    public: // get methods

      const std::string& getId() const { return id; }
      const std::string& getType() const { return type; }
      const inline bool isInput() const { return flags & FLAG_IN; }
      const inline bool isOutput() const { return flags & FLAG_OUT; }

    public: // sending methods

      template<typename V>
      inline void sendData(const V& val) {
        signal.emit((const void*)&val);
      }

      inline void sendSignal() {
        signal.emit(NULL);
      }

    public: // receiving methods

      inline cinder::signals::Connection onSignal(InFuncNoArg func) {
        return signal.connect(toVoid(func));
      }

      template <typename V>
      cinder::signals::Connection onData(std::function<void(const V&)> func) {
        return signal.connect([func](const void* arg){
          func(*(V*)arg);
        });
      }

    public: // connect methods

      inline cinder::signals::Connection outputTo(Port& receiver, bool performTypeCheck = false) {
        return Port::connect(*this, receiver, performTypeCheck);
      }

      inline cinder::signals::Connection outputTo(Signal& signal) {
        return Port::connect(this->signal, signal);
      }

      inline cinder::signals::Connection inputFrom(Port& sender, bool performTypeCheck = false) {
        return Port::connect(sender, *this, performTypeCheck);
      }

      inline cinder::signals::Connection inputFrom(Signal& signal) {
        return Port::connect(signal, this->signal);
      }

      inline static cinder::signals::Connection connect(Port& sender, Port& receiver, bool performTypeCheck = false) {
        // return dummy connection if types don't match. TODO; log warning?
        if (performTypeCheck && sender.type != receiver.type) return cinder::signals::Connection();
        return Port::connect(sender.signal, receiver.signal);
      }

      inline static cinder::signals::Connection connect(Signal& output, Signal& input) {
        return output.connect([&input](const void* arg){
          input.emit(arg);
        });
      }

      inline static cinder::signals::Connection connect(Port& sender, Signal& receiver) {
        return sender.signal.connect([&receiver](const void* arg){
          receiver.emit(arg);
        });
      }

      inline static cinder::signals::Connection connect(Signal& sender, Port& receiver) {
        return sender.connect([&receiver](const void* arg){
          receiver.signal.emit(arg);
        });
      }

    protected: // methods

      inline InFuncVoid toVoid(InFuncNoArg func) { 
        return [func](const void* arg) {
          func();
        };
      }

    // TODO: make protected
    public: // attributes
      Signal signal;

    private: // attributes
      std::string id;
      std::string type;
      int flags;
  };

  // /// DEPRECATED
  // template<typename V>
  // class TypedPort : public Port {
    
  //   public:
  //     typedef std::function<void(const V&)> InFuncTypeRef;

  //   public:
  //     TypedPort(const std::string& id, int flags = Port::FLAG_IN) : Port(id, typeid(V).name(), flags) {
  //     }
  // };
}
