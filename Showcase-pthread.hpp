#ifndef showcase_hpp
#define showcase_hpp

#include <chrono>
#include <thread>
#include <emscripten/wasm_worker.h>

#include "CLNodeFactory.hpp"
#include "CanvasElement.hpp"
#include "ClarityNode.hpp"
#include "PageContent.hpp"
#include "clarity.hpp"

using namespace clarity;

HybridNode<double> *cir1Radius_tinp_;
pthread_t id;
int ret;

double *globalDbl = new double(6.28);
emscripten_wasm_worker_t worker = emscripten_malloc_wasm_worker(/*stack size: */ 1024);

void run_in_worker() {
    printf("Hello from wasm worker!\n");
    cir1Radius_tinp_->setCppValFromJSVal(val(*globalDbl));
    printf("Set the value!.\n");
    cir1Radius_tinp_->refresh();
    printf("Refresh Successful!!!.\n");
}

void *threadFunction(void *args) {
    int i = 0;
    while (1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        i++;
        *globalDbl += 3.1;

        if ((i % 7) == 0) {
            printf("I am threadFunction %d, %lf.\n", i, *globalDbl);
            cir1Radius_tinp_->setCppValFromJSVal(val(*globalDbl));
            printf("Set the value!.\n");
            cir1Radius_tinp_->refresh();
            printf("Refresh Successful!!!.\n");
        }
    }
}

int initMyThread() {
    /*creating thread*/
    ret = pthread_create(&id, NULL, &threadFunction, NULL);
    if (ret == 0) {
        printf("Thread created successfully.\n");
    } else {
        printf("Thread not created.\n");
        return -1; /*return from main*/
    }
    return 0;
}

void runww() {
    // emscripten_wasm_worker_post_function_v(worker, run_in_worker);
}

// EMSCRIPTEN_BINDINGS(wwtest) {
// emscripten::function("run_in_worker", &run_in_worker, allow_raw_pointers())
//  .emscripten::function("runww", &runww, allow_raw_pointers());
//}

/**
 * @brief Used to test all the major types of web controls.
 *
 */
struct Showcase : public PageContent {
    ClarityNode *content(ClarityNode *innerContent = nullptr) {
        ClarityNode::addJSAuxScript("showcaseAux.js");
        ClarityNode::runJSAuxScripts();

        val elgInputAlltypesMouseover = ClarityNode::JSProxyNode_["elgInputAlltypesMouseover"];

        ClarityNode::installListenerGenerators(
            "JSProxyNode", "HybridNode", elgInputAlltypesMouseover, "input", "range", "mouseover");

        val blackbody_st = ClarityNode::JSProxyNode_["blackbody_st"];

        // double *temp = new double(330);
        double *cir1Radius_value = new double(9.14);

        CLNodeFactory<HybridNode, double, double> builder("div", "showcase_root");

#ifdef USETF
        auto *maindiv = builder.build();
#else
        auto *maindiv = builder.withAttachmentId("hookid")
                            .withAttachmentMode(clarity::ClarityNode::AttachmentMode::REPLACE_ID)
                            .build();
#endif

        CLNodeFactory<HybridNode, double, double> childOfMaindivBuilder =
            builder.withChildrenOf(maindiv);

        cir1Radius_tinp_ = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                               .withName("cir1Radius_tinp_")
                               .withCppVal(cir1Radius_value)
                               .textInput();

        //initMyThread();
        //emscripten_wasm_worker_post_function_v(worker, run_in_worker);
        run_in_worker();

        
        printf("Setup complete!\n");
        return maindiv;
    }
};

#endif
