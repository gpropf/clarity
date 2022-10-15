#include "testbed.hpp"

#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "globals.hpp"

template <>
const array<string, 8> CanvasGrid<unsigned char>::colors = {"#F5F5DC", "#00FF00", "#00AA00", "#FF00FF",
                                                            "#AA00AA", "#00AAFF", "#9090AA", "#888888"};

int main() {
    val CLE = val::global("CLElement");
    val doNothing = CLE["doNothing"];
    val square = CLE["square"];
    val blackbody_st = CLE["blackbody_st"];
    val nodeAudit = CLE["nodeAudit_double"];

    CLNodeFactory<HybridNode, double, double> rootBuilder("div", "root");
    HybridNode<double> *root = rootBuilder.build();

    HybridNode<double> *sidebar =
        rootBuilder.withParent(root).withName("sidebar").withAttributes({{"class", val("sidebar")}}).build();
    HybridNode<double> *testarea =
        rootBuilder.withParent(root).withName("testarea").withAttributes({{"class", val("mainarea")}}).build();

    CLNodeFactory<HybridNode, double, double> sidebarBuilder =
        rootBuilder.withName("sidebar_subnode").createChildrenOf(sidebar);
    CLNodeFactory<HybridNode, double, double> testareaBuilder =
        rootBuilder.withName("testarea_subnode").createChildrenOf(testarea);

    HybridNode<int> *nodeAuditButton =
        (CLNodeFactory<HybridNode, int, int>(sidebarBuilder)).button("nodeAuditButton", "Node Audit", nodeAudit);

    double *a = new double(3.2);
    HybridNode<double> *tinp =
        testareaBuilder.withLinkMultiplierConstant(1).withName("a_tinp").withCppVal(a).textInput();

    return 0;
}
