#ifndef PageContent_hpp
#define PageContent_hpp

#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"
#include "clarity.hpp"
//#include "embindings.hpp"
//#include "globals.hpp"

using namespace clarity;

/**
 * @brief Abstract convenience class. Overload this class and the content() method
 * and you can easily put your content in an existing template like the testbed.cpp
 * example program.
 *
 */
struct PageContent {
   // virtual ClarityNode *content() = 0;
    virtual ClarityNode *content(ClarityNode *innerContent = nullptr) = 0;
    ClarityNode * rootNode;
};

/**
 * @brief Provides a node audit button and eventually perhaps other controls meant to provide a test
 * framework for new and existing controls as well as aid users in developing and testing their
 * projects.
 *
 */
struct TestFramework : public PageContent {
    ClarityNode *content(ClarityNode *innerContent = nullptr) {
        val CLE = val::global("CLElement");
        val nodeAudit = CLE["nodeAudit_double"];

        CLNodeFactory<HybridNode, double, double> rootBuilder("div", "root");
        HybridNode<double> *root = rootBuilder.build();

        HybridNode<double> *sidebar = rootBuilder.withParent(root)
                                          .withName("sidebar")
                                          .withInnerHTML("<h1>Clarity Web Framework</h1>")
                                          .withAttributes({{"class", val("sidebar")}})
                                          .build();
        HybridNode<double> *testarea = rootBuilder.withParent(root)
                                           .withInnerHTML("<h2>Test Area</h2>")
                                           .withName("testarea")
                                           .withAttributes({{"class", val("mainarea")}})
                                           .build();

        testarea->appendChild(innerContent);

        CLNodeFactory<HybridNode, double, double> sidebarBuilder =
            rootBuilder.withName("sidebar_subnode").createChildrenOf(sidebar);
        CLNodeFactory<HybridNode, double, double> testareaBuilder =
            rootBuilder.withName("testarea_subnode").createChildrenOf(testarea);

        HybridNode<int> *nodeAuditButton = (CLNodeFactory<HybridNode, int, int>(sidebarBuilder))
                                               .button("nodeAuditButton", "Node Audit", nodeAudit);
        return root;
    }

//    ClarityNode *content() { return content(nullptr); }
};

#endif
