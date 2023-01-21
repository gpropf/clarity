#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <stdio.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <map>
#include <sstream>


namespace cl2 {

    

class WebElementBuilder {;
    std::string tag_, name_;
    int id_;
    public:
    WebElementBuilder(std::string tag, std::string name, int id): tag_(tag), name_(name), id_(id) {
        std::cout << "WebElementBuilder CALLED!" << std::endl;
    }
};

}  // namespace cl2
