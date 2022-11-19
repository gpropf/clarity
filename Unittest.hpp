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
 * @brief Unit
 *
 */
struct Unittest : public PageContent {
    ClarityNode *content(ClarityNode *innerContent = nullptr) {
        CLNodeFactory<HybridNode, double, double> builder("div", "test_root");
        auto *maindiv = builder.build();

        CLNodeFactory<HybridNode, double, double> testBuilder =
            builder.withChildrenOf(maindiv);

        

        TextinputDoubleUT<HybridNode, double, double> tidbl(&testBuilder);

        

        auto [ptr, node] = tidbl.setup();
        tidbl.runEvent();
        bool succeeded = tidbl.evaluate();
        // auto *doubleTextinputTest =
        //     childOfMaindivBuilder.withName("doubleTextinputTest").withCppVal(dptr).textInput();
        
        //tidbl.cleanup({ptr,node});

        cout << "Setup complete!" << endl;
        return maindiv;
    }
};

#endif
