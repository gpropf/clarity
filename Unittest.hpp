#ifndef showcase_hpp
#define showcase_hpp

#include "CLNodeFactory.hpp"
#include "CanvasElement.hpp"
#include "ClarityNode.hpp"
#include "PageContent.hpp"
#include "Unittester.hpp"
#include "clarity.hpp"

using namespace clarity;

/**
 * @brief Unit tests. Eventually this will contain instances of each type of control and will run a few tests on it.
 *
 */
struct Unittest : public PageContent {
    ClarityNode *content(ClarityNode *innerContent = nullptr) {
        CLNodeFactory<HybridNode, double, double> builder("div", "test_root");
        auto *maindiv = builder.build();

        CLNodeFactory<HybridNode, double, double> testBuilder = builder.withChildrenOf(maindiv);

        bool succeeded;

        TextinputUT<HybridNode, double, double> tidbl(&testBuilder);
        tidbl.setup();
        tidbl.runEvents();
        succeeded = tidbl.evaluate();
        tidbl.notify(succeeded);
        // if (succeeded) tidbl.cleanup();

        TextinputUT<HybridNode, int, int> tiint(&testBuilder);
        tiint.setup();
        tiint.runEvents();
        tiint.notify(tiint.evaluate());

        TextinputUT<HybridNode, unsigned char, int> tiuch(&testBuilder);
        tiuch.setup();
        tiuch.runEvents();
        tiuch.notify(tiuch.evaluate());

        TextinputUT<HybridNode, float, int> tiflt(&testBuilder);
        tiflt.setup();
        tiflt.runEvents();
        tiflt.notify(tiflt.evaluate());

        SimpleSelectUT<SimpleSelect, int, int> ssint(&testBuilder);
        ssint.setup();
        ssint.runEvents();
        ssint.notify(ssint.evaluate());


        // if (succeeded) tiint.cleanup();

        // auto *doubleTextinputTest =
        //     childOfMaindivBuilder.withName("doubleTextinputTest").withCppVal(dptr).textInput();

        // tidbl.cleanup({ptr,node});

        cout << "Setup complete!" << endl;
        return maindiv;
    }
};

#endif
