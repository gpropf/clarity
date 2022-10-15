#include "testbed.hpp"

#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "globals.hpp"
#include "showcase.hpp"

template <>
const array<string, 8> CanvasGrid<unsigned char>::colors = {"#F5F5DC", "#00FF00", "#00AA00", "#FF00FF",
                                                            "#AA00AA", "#00AAFF", "#9090AA", "#888888"};


void framework(ClarityNode * content) {
    val CLE = val::global("CLElement");
    val nodeAudit = CLE["nodeAudit_double"];

    CLNodeFactory<HybridNode, double, double> rootBuilder("div", "root");
    HybridNode<double> *root = rootBuilder.build();

    HybridNode<double> *sidebar =
        rootBuilder.withParent(root).withName("sidebar").withAttributes({{"class", val("sidebar")}}).build();
    HybridNode<double> *testarea =
        rootBuilder.withParent(root).withName("testarea").withAttributes({{"class", val("mainarea")}}).build();

    testarea->appendChild(content);

    CLNodeFactory<HybridNode, double, double> sidebarBuilder =
        rootBuilder.withName("sidebar_subnode").createChildrenOf(sidebar);
    CLNodeFactory<HybridNode, double, double> testareaBuilder =
        rootBuilder.withName("testarea_subnode").createChildrenOf(testarea);

    HybridNode<int> *nodeAuditButton =
        (CLNodeFactory<HybridNode, int, int>(sidebarBuilder)).button("nodeAuditButton", "Node Audit", nodeAudit);

}




int main() {
    
    Showcase showcase;
    framework(showcase.content());    
    return 0;

}
