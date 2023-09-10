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

#ifndef SignalBuilder3_hpp
#define SignalBuilder3_hpp

#include "SignalPrimitives.hpp"
#include "Util3.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"



namespace cl3 {


class WebSignal {

};



/**
 * @brief Factory class that creates WebElements and signals. Also connects signals to one another.
 *
 */
class SignalBuilder3 {
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
    SignalBuilder3(int startId = 0, bool labelAllInputs = true,
                  bool labelsSwallowTheirReferents = true, val parentDOMElement = val::null())
        : labelAllInputs_(labelAllInputs),
          labelsSwallowTheirReferents_(labelsSwallowTheirReferents),
          parentDOMElement_(parentDOMElement) {
        tm_ = TicketMachine(startId);
    }
};
}  // namespace cl3
#endif