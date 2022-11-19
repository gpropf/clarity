#ifndef showcase_hpp
#define showcase_hpp

#include "CLNodeFactory.hpp"
#include "CanvasElement.hpp"
#include "ClarityNode.hpp"
#include "PageContent.hpp"
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

        CLNodeFactory<HybridNode, double, double> childOfMaindivBuilder =
            builder.withChildrenOf(maindiv);

        double *dptr = new double(1.2);
        auto *doubleTextinputTest =
            childOfMaindivBuilder.withName("doubleTextinputTest").withCppVal(dptr).textInput();

        cout << "Setup complete!" << endl;
        return maindiv;
    }
};

#endif
