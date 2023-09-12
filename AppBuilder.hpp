/**
 * @file SignalBuilder.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief This is the factory class that creates and then connects the signals.
 * @version 0.1
 * @date 2023-03-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef AppBuilder3_hpp
#define AppBuilder3_hpp

#include "SignalPrimitives.hpp"
#include "Util3.hpp"
#include "WebElementSignals.hpp"
#include "WebElements3.hpp"

using std::cout;
using std::string;

namespace cl3 {

class IChannel {

};


template <typename S>
class Channel : public IChannel {
    S currentValue_;
};

/**
 * @brief Factory class that creates WebElements and signals. Also connects signals to one another.
 *
 */
//template <template <typename> class T>
class AppBuilder {
    std::vector<const int> currentGroupIds_;
    std::map<const std::string, shared_ptr<IChannel>> channels_;
    std::map<const std::string, val> domElements_;
    TicketMachine tm_;
    bool labelAllInputs_ = true;
    bool labelsSwallowTheirReferents_ = true;
    bool addBRAfterAllCalls_ =
        true;  //!< add a '<BR>' tag after each call so everything goes on its own line.
    val parentDOMElement_ = val::null();
    // std::map<const std::string, const WebElement*>
    //     elementMap_;  //!< a map of elements we've created so we can re-use them.
    // std::map<std::string, val> attrs_;
    bool attrsResetAfterSingleUse_ = true;

   public:
    AppBuilder(int startId = 0, bool labelAllInputs = true,
                   bool labelsSwallowTheirReferents = true, val parentDOMElement = val::null())
        : labelAllInputs_(labelAllInputs),
          labelsSwallowTheirReferents_(labelsSwallowTheirReferents),
          parentDOMElement_(parentDOMElement) {
        tm_ = TicketMachine(startId);
    }

    val textField(const std::string& name, val parentElement = val::null()) {        
        const string tfid = cl3::TicketMachine::getNextStrSid();
        auto tf = TextField(name, tfid , parentElement);
        val tfDomEl = tf.getDomElement();
        domElements_.insert({tfid, tfDomEl});
        return tfDomEl;
    }



};
}  // namespace cl3
#endif