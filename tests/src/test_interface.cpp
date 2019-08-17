#include "catch.hpp"

#include <iostream>
#include "cinder/app/App.h" // for CINDER_MSW macro
#include "info/Interface.h"

class Keyboard {
  public:
    static info::Interface* createInfoInterface() {
      return info::Interface::create<Keyboard>([](info::Builder<Keyboard>& builder){
        builder.output<char>("KeyCode")
          ->apply([](Keyboard& instance, std::function<void(const char&)> out) {
            // instance.onKeyDown([&out](char keycode){
            //   out(keycode);
            // });
          });

        builder.output<bool>("HasKeyDown");
        
        builder.attr<bool>("enabled")
          ->apply([](Keyboard& instance, info::TypedPort<bool>& port) {
            std::cout << "Keyboard::enabled::apply" << std::endl;

            port.input([&instance](const bool& val){
              std::cout << "Keyboard::enabled::apply::connect: " << val << std::endl;
              instance.enabled = val;
            });

          });
      });
    }

    void onKeyDown(std::function<void(char)> func) {
      // if(key.size() != 1) return;
      // char chr = key[0];

      // this->connections.push_back(ci::app::getWindow()->getSignalKeyDown().connect([func](ci::app::KeyEvent& event){
      //   func(event.getChar());
      // }));

      // this->connections.push_back(this->keySignal.connect(func));
    }

    // std::vector<ci::signals::Connection> connections;
  
    // ctree::Signal<void(char)> keySignal;
    bool enabled = false;
};

TEST_CASE("info::Interface", ""){
  SECTION("create"){
    auto info = Keyboard::createInfoInterface();

    // verify we can extract outputs information from info interface
    std::vector<std::string> ids = {"KeyCode", "HasKeyDown", "enabled"};
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

  SECTION("createInstance") {
    auto info = Keyboard::createInfoInterface();

    Keyboard keyboard;
    auto instanceRef = info->createInstance(keyboard);
    REQUIRE(instanceRef->port<bool>("enabled") != NULL);

    REQUIRE(keyboard.enabled == false);
    instanceRef->port<bool>("enabled")->emitIn(true);
    REQUIRE(keyboard.enabled == true);
    instanceRef->port<bool>("enabled")->emitIn(false);
    REQUIRE(keyboard.enabled == false);
  }
}
