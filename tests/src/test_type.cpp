#include "catch.hpp"

#include <iostream>
#include <memory>
#include "info/Type.h"
// #include "cinder/app/App.h" // for CINDER_MSW macro
#include "cinder/Signals.h"

class Keyboard {
  public:
    static std::shared_ptr<info::Type> createInfoType() {
      return info::Type::create<Keyboard>("Keyboard", [](info::TypeBuilder<Keyboard>& builder){
        builder.output<char>("KeyCode")
          ->apply([](Keyboard& instance, std::function<void(const char&)> out) {
            instance.keySignal.connect([out](char keycode){
              out(keycode);
            });
          });

        builder.output<bool>("HasKeyDown");
        
        builder.input<bool>("enabled")
          ->apply([](Keyboard& instance, info::TypedPort<bool>& port) {

            port.onData<bool>([&instance](const bool& val){
              instance.enabled = val;
            });

          });

        builder.addOutput("AnyKey")->apply([](Keyboard& instance, info::Port& port) {
          instance.keySignal.connect([&port](char keycode){
              port.sendData(NULL);
            });
          });
      });
    }
 
    cinder::signals::Signal<void(char)> keySignal;
    bool enabled = false;
};

TEST_CASE("info::Type", ""){
  SECTION("create"){
    auto info = Keyboard::createInfoType();

    // verify we can extract outputs information from info interface
    std::vector<std::string> ids = {"KeyCode", "HasKeyDown", "enabled", "AnyKey"};
    REQUIRE(ids.size() == info->getPorts().size());
    for(int i=0; i<ids.size(); i++) {
      REQUIRE(ids[i] == info->getPorts()[i]->getId());
    }

    // verify we can extract outputs type information from info interface
    std::vector<std::string> types = {"c" /* char */, "b" /* bool */};
    for(int i=0; i<types.size(); i++) {
      REQUIRE(types[i] == info->getPorts()[i]->getType());
    }
  }

  SECTION("createInstance + input") {
    auto info = Keyboard::createInfoType();

    Keyboard keyboard;
    auto instanceRef = info->createInstance(keyboard);
    REQUIRE(instanceRef->port<bool>("enabled") != NULL);

    REQUIRE(keyboard.enabled == false);
    instanceRef->getInput("enabled")->sendData<bool>(true);
    REQUIRE(keyboard.enabled == true);
    instanceRef->getInput("enabled")->sendData<bool>(false);
    REQUIRE(keyboard.enabled == false);
  }

  SECTION("createInstance + output") {
    auto info = Keyboard::createInfoType();

    Keyboard keyboard;
    auto instanceRef = info->createInstance(keyboard);
    char capture = '0';
    
    auto outport = instanceRef->getOutput("KeyCode");
    REQUIRE(outport);
    outport->onData<char>([&capture](const char& newval){
      capture = newval;
    });

    REQUIRE(capture == '0');
    keyboard.keySignal.emit('3');
    REQUIRE(capture == '3');
  }

  SECTION("signalOut & onOutput") {
    auto info = Keyboard::createInfoType();

    Keyboard keyboard;
    auto instanceRef = info->createInstance(keyboard);
    char counter = 0;
    
    instanceRef->getOutput("AnyKey")->onSignal([&counter](){
      counter += 1;
    });

    REQUIRE(counter == 0);
    keyboard.keySignal.emit('7');
    REQUIRE(counter == 1);
    keyboard.keySignal.emit(' ');
    keyboard.keySignal.emit(' ');
    REQUIRE(counter == 3);
  }
}
