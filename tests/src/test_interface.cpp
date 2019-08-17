#include "catch.hpp"

#include "cinder/app/App.h" // for CINDER_MSW macro
#include "info/Interface.h"

class Keyboard {
  public:
    static info::Interface* createInfoInterface() {
      return info::Interface::create<Keyboard>([](info::Builder<Keyboard>& builder){
        builder.output<char>("KeyCode")
          .apply([](Keyboard& instance, std::function<void(const char&)> out) {
            instance.onKeyDown([&out](char keycode){
              out(keycode);
            });
          });

        builder.output<bool>("HasKeyDown");
        
        builder.attr<bool>("enabled")
          .apply([](Keyboard& instance, info::TypedPort<bool>& port) {
            

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
    bool enabled = true;
};

TEST_CASE("info::Interface", ""){
  SECTION("create"){
    auto info = Keyboard::createInfoInterface();

    // verify we can extract outputs information from info interface
    std::vector<std::string> ids = {"KeyCode", "HasKeyDown", "enabled"};
    for(int i=0; i<ids.size(); i++) {
      REQUIRE(ids[i] == info->getOutputs()[i]->getId());
    }

    // verify we can extract outputs type information from info interface
    std::vector<std::string> types = {"c" /* char */, "b" /* bool */};
    for(int i=0; i<types.size(); i++) {
      REQUIRE(types[i] == info->getOutputs()[i]->getType());
    }
  }

  SECTION("createInstance") {
    auto info = Keyboard::createInfoInterface();

    Keyboard keyboard;
    auto instanceRef = info->createInstance(keyboard);

    instanceRef->port<bool>("enabled")->emit(false);
    REQUIRE(keyboard.enabled == false);
    instanceRef->port<bool>("enabled")->emit(true);
    REQUIRE(keyboard.enabled == true);
  }
}
