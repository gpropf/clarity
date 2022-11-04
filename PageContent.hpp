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
    virtual ClarityNode *content(ClarityNode *innerContent = nullptr) = 0;
};

/**
 * @brief Provides a node audit button and eventually perhaps other controls meant to provide a test
 * framework for new and existing controls as well as aid users in developing and testing their
 * projects.
 *
 */
struct TestFramework : public PageContent {
    ClarityNode *content(ClarityNode *innerContent = nullptr) {

        val nodeAudit = ClarityNode::CLElement_["nodeAudit_double"];

        CLNodeFactory<HybridNode, double, double> rootBuilder("div", "tf_root");

        // Uncomment the lines below to see how to attach a node to existing content.
        // auto *root = rootBuilder.withAttachmentMode(ClarityNode::AttachmentMode::REPLACE_ID)
        //                  .withAttachmentId("hookid")
        //                  .build();

        // Default attachment mode is to just attach the node directly to the body.
        auto *root = rootBuilder.build();

        auto *sidebar = rootBuilder.withParent(root)
                            .withName("sidebar")
                            .withInnerHTML("<h1>Clarity Web Framework</h1>")
                            .withAttributes({{"class", val("sidebar")}})
                            .build();
        auto *testarea = rootBuilder.withParent(root)
                             .withInnerHTML("<h2>Test Area</h2>")
                             .withName("testarea")
                             .withAttributes({{"class", val("mainarea")}})
                             .build();

        testarea->appendChild(innerContent);

        CLNodeFactory<HybridNode, double, double> sidebarBuilder =
            rootBuilder.withName("sidebar_subnode").withChildrenOf(sidebar);
        CLNodeFactory<HybridNode, double, double> testareaBuilder =
            rootBuilder.withName("testarea_subnode").withChildrenOf(testarea);

        auto *nodeAuditButton = (CLNodeFactory<HybridNode, int, int>(sidebarBuilder))
                                    .button("nodeAuditButton", "Node Audit", nodeAudit);
        return root;
    }
};

#endif
