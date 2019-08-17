#include "catch.hpp"

#include "cinder/app/App.h" // for CINDER_MSW macro
#include "info/Interface.h"

class InfoKeyboard {
  public:
    static info::Interface* createInfoInterface() {
      return info::Interface::create<InfoKeyboard>([](info::Builder<InfoKeyboard>& builder){
        builder.output<char>("KeyCode")
          .apply([](InfoKeyboard& instance, std::function<void(const char&)> out) {
            instance.onKeyDown([&out](char keycode){
              out(keycode);
            });
          });

        builder.output<bool>("HasKeyDown");
        
        builder.attr<bool>("enabled")
          .apply([](InfoKeyboard& instance, info::TypedPort<bool>& port) {
            

          });
      });
    }

  protected:

    void onKeyDown(std::function<void(char)> func) {
      // if(key.size() != 1) return;
      // char chr = key[0];

      // this->connections.push_back(ci::app::getWindow()->getSignalKeyDown().connect([func](ci::app::KeyEvent& event){
      //   func(event.getChar());
      // }));

      // this->connections.push_back(this->keySignal.connect(func));
    }

  private:
    std::vector<ci::signals::Connection> connections;
  public:
    // ctree::Signal<void(char)> keySignal;
};

TEST_CASE("info::Interface", ""){
  SECTION("create"){
    auto info = InfoKeyboard::createInfoInterface();

    // verify we can extract outputs information from info interface
    std::vector<std::string> ids = {"KeyCode", "HasKeyDown", "enabled"};
    for(int i=0; i<ids.size(); i++) {
      REQUIRE(ids[i] == info->getOutputs()[i]->getId());
    }

    std::vector<std::string> types = {"c" /* char */, "b" /* bool */};
    for(int i=0; i<types.size(); i++) {
      REQUIRE(types[i] == info->getOutputs()[i]->getType());
    }
  }
}
