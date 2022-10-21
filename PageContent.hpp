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
    //virtual ClarityNode *content(ClarityNode *innerContent = nullptr) = 0;
    virtual ClarityNode *content(ClarityNode *innerContent = nullptr, string attachmentNodeName = "") = 0;
    CLNodeFactoryBase * rootBuilder_;
    string attachmentNodeName_ = "";
    virtual void buildPage() {
        if (attachmentNodeName_ == "") {
            attachmentNodeName_ = "clroot";
        }
        //CLNodeFactory<HybridNode, double, double> rootBuilder("div", attachmentNodeName_);
        HybridNode<void> *root = dynamic_cast<HybridNode<void>*>(rootBuilder_->build());
    }
};

/**
 * @brief Provides a node audit button and eventually perhaps other controls meant to provide a test
 * framework for new and existing controls as well as aid users in developing and testing their
 * projects.
 *
 */
struct TestFramework : public PageContent {
    
    ClarityNode *content(ClarityNode *innerContent, string attachmentNodeName = "") {
        val CLE = val::global("CLElement");
        val nodeAudit = CLE["nodeAudit_double"];

        if (attachmentNodeName == "") {
            attachmentNodeName = "clroot";
        }
        CLNodeFactory<HybridNode, int, double> rootBuilder("div", attachmentNodeName);
        auto *root = rootBuilder.build();

        auto *sidebar = rootBuilder.withParent(root)
                                          .withName("sidebar")
                                          .withInnerHTML("<h1>Clarity Testbed</h1>")
                                          .withAttributes({{"class", val("sidebar")}})
                                          .build();
        auto *testarea = rootBuilder.withParent(root)
                                           .withInnerHTML("<h2>Test Area</h2>")
                                           .withName("testarea")
                                           .withAttributes({{"class", val("mainarea")}})
                                           .build();

        testarea->appendChild(innerContent);

        CLNodeFactory<HybridNode, double, double> sidebarBuilder =
            rootBuilder.withName("sidebar_subnode").createChildrenOf(sidebar);
        CLNodeFactory<HybridNode, double, double> testareaBuilder =
            rootBuilder.withName("testarea_subnode").createChildrenOf(testarea);

        auto *nodeAuditButton = (CLNodeFactory<HybridNode, int, int>(sidebarBuilder))
                                               .button("nodeAuditButton", "Node Audit", nodeAudit);
        return root;
    }

    //    ClarityNode *content() { return content(nullptr); }
};

#endif
