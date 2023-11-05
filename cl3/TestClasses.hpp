/**
 * @file TestClasses.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief Some simple classes to test the cl3 object integrations.
 * @version 0.3
 * @date 2023-10-15
 *
 * @copyright Copyright (c) 2023
 *
 */


#include <memory>

#include "AppBuilder.hpp"
#include "Util.hpp"

using std::cout;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;

using namespace cl3;


namespace cl3 {

class OtherClass {
   public:
    int foo_ = 7;
    int getFoo() { return foo_; }
    void setFoo(int f) { foo_ = f; }
};

/**
 * @brief This is a simple do-nothing class meant to test the `CppObjectSignalObject` class' ability
 * to use getter and setter method pointers to act as initiation points or endpoints for signals.
 *
 */
class TestObj : public std::enable_shared_from_this<TestObj>, public Ticker {
    int i_ = 3;
    double d_ = 3.5;
    double dblval_ = 3.14159;
    std::string s_ = "foo";
    int width_ = 13;

   public:
    static TestObj *transfer(TestObj *t) { return t; }
    std::shared_ptr<TestObj> getptr() { return this->shared_from_this(); }

    virtual void tick() {        
        auto d = getDblval();
        d += 0.1;
        setDblval(d);
        auto w = getWidth();
        setWidth(++w);
        cout << "TestObj ticks! " << dblval_ << endl;
    }    

    /**
     * @brief This installs this class' `tick()` method to be called from the `setInterval()` JS
     * function.
     *
     */
    virtual void generateTickFunction() { tickJS_ = val::global("TickerTick")(this->getptr()); }

    void setS(const std::string &s) {
        s_ = s;
        cout << "TestObj.s_ has been set to: " << s_ << endl;
    }

    void signalAcceptorTestMethod(const std::string &s) {
        s_ = s;
        cout << "TestObj::signalAcceptorTestMethod(): " << s_ << endl;
    }

    std::string signalEmitterTestMethod() {
        cout << "TestObj::signalEmitterTestMethod(): " << s_ << endl;
        return s_;
    }

    void setWidth(const int &w) {
        cout << "TestObj::setWidth(): " << w << endl;
        width_ = w;
    }

    int getWidth() const {
        cout << "TestObj::getWidth(): " << width_ << endl;
        return width_;
    }

    void setDblval(const double &w) {
        cout << "TestObj::setDblval(): " << w << endl;
        dblval_ = w;
    }

    static void setDblvalOnObj(TestObj *tptr, double v) { tptr->setDblval(v); }

    double getDblval() const {        
        cout << "TestObj::getDblval(): " << dblval_ << endl;
        return dblval_;
    }

    /**
     * @brief We're also using the TestObj class to test the MouseSignal class.
     *
     * @param mouseLocation
     */
    void mouseClickAcceptorMethod(const std::pair<double, double> &mouseLocation) {
        cout << "TestObj::mouseClickAcceptorMethod(): x = " << mouseLocation.first
             << ", y = " << mouseLocation.second << endl;
    }

    std::string getS() { return s_; }
};

}  // namespace cl3