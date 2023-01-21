#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <stdio.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <map>
#include <sstream>

using emscripten::val;
using std::cout;
using std::endl;

namespace cl2 {

class WebElementBuilder {
    std::string tag_, name_;
    int id_;
    val webElement_;

   public:
    WebElementBuilder(std::string tag, std::string name, int id) : tag_(tag), name_(name), id_(id) {
        std::cout << "WebElementBuilder CALLED!" << std::endl;
        val webElement_ = val::global("WebElement").new_();
        webElement_.call<void>("initElement", tag_, name_, id_);
    }

    void printStats() const {
        cout << "Tag is: " << tag_ << endl;
        cout << "element name is: " << name_ << endl;
        cout << "id is: " << id_ << endl;
    }
};

}  // namespace cl2
