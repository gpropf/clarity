/**
 * @file Clarity.js
 * @author Greg Propf (gpropf@gmail.com)
 * @brief This is the library of basic JavaScript functions needed by the Clarity framework.
 * @version 0.2
 * @date 2023-10-15
 *
 * @copyright Copyright (c) 2023
 *
 */

class WebElement {
    constructor() { }

    /**
     * This is actually what creates the DOM element. It will try not to create empty name and id fields.
     * 
     * @param {str} tag 
     * @param {str} name 
     * @param {int} id 
     * @returns 
     */
    static initElement(tag, name = null, id = null) {

        var domElement = WebElement.createDOMElementByTagType(tag);

        if (id && id != "") domElement.id = id;
        if (name && name != "") {
            domElement.name = name;
            domElement.setAttribute("name", name);
        }
        document.body.appendChild(domElement); // Fixme
        return domElement;
    }

    static tagToUrl = {
        "svg": "http://www.w3.org/2000/svg",
        "circle": "http://www.w3.org/2000/svg",
        "ellipse": "http://www.w3.org/2000/svg",
        "rect": "http://www.w3.org/2000/svg"
    };


    static createDOMElementByTagType(tag) {
        var el;
        if (WebElement.tagToUrl[tag]) {
            el = document.createElementNS(WebElement.tagToUrl[tag], tag);
        }
        else {
            el = document.createElement(tag);
        }
        return el;
    }
}

function sayHello() {
    console.log("This just prints HELLO to the console.");
}

// /**
//  * I created this to print JS values from C++ but 
//  * 
//  * @param {*} v 
//  */
// function printVal(v) {
//     console.log(v);    
// }

function TickerTick(ticker) {
    return function () {
        //callMethodByName(ticker, "tick");
        //console.log("TICKING THE TICKER!", ticker.getDblval());

        ticker.tick();
        //ticker.printGroup("g1");
    }
}

function generateInjectFn(channel) {
    console.log("On JS side creating inject listener for channel: ", channel);
    return function (ev) {
        //let s = testObjCSO.getSignal();
        console.log("inject generated fn called for channel at: ", channel);
        channel.inject(ev.target.value, 0);
    };
}

function generateEventListenerForChannel_TextField(channel) {
    console.log("generateEventListenerForChannel_TextField: ", channel);
    return function (ev) {
        //let s = testObjCSO.getSignal();
        console.log("generateEventListenerForChannel_TextField --> inject generated fn called for channel at: ", channel);
        channel.inject(ev.target.value, 0);
    };
}

function generateEventListenerForChannel_RangeInput(channel) {
    console.log("generateEventListenerForChannel_RangeInput: ", channel);
    return function (ev) {
        //let s = testObjCSO.getSignal();
        console.log("generateEventListenerForChannel_RangeInput --> inject generated fn called for channel at: ", channel);
        channel.inject(ev.target.value, 0);
    };
}


function generateEventListenerForChannel_Button(channel, clickCommand = "CLICK") {
    console.log("generateEventListenerForChannel_Button: ", channel);
    return function (ev) {
        //let s = testObjCSO.getSignal();
        console.log("generateEventListenerForChannel_Button --> inject generated fn called for channel at: ", channel);
        channel.inject(clickCommand, 0);
        
    };
}



function lockChannel(channel) {
    return function (ev) {
        //let s = testObjCSO.getSignal();
        // console.log("inject generated fn called.");
        // channel.inject(ev.target.value, 0);
    };
}

/**
 * 
 * @param {bound C++ object} obj 
 * @param {string} objMethodName 
 * @returns whatever the named method returns
 */
function callMethodByName(obj, objMethodName) {
    var fn = obj[objMethodName];
    if (typeof fn === "function") {
        return fn.apply(obj);
    }
}

/**
 * 
 * @param {bound C++ object} obj 
 * @param {string} objMethodName 
 * @returns a function that calls the method on the provided object.
 * This is useful for buttons that simply need to fire off a zero-argument
 *  method on some arbitrary C++ object.
 */
function elgCallMethodOnObjByName(obj, objMethodName) {
    return function () {        
        console.log("elgCallMethodOnObjByName generated fn called.");
        callMethodByName(obj, objMethodName);        
    };
}

function installTickCallback(tickBtnId) {
    console.log("installTickCallback():", tickBtnId);
    var tickbtn = document.getElementById(tickBtnId);
    tickbtn.addEventListener("click", appBuilderSingletonTick);
}

function getTickFn(obj) {    
    return function () {        
        console.log("Generated tick fn has object at address: ", obj)
        obj.tick();        
    }
}

// DEPRECATED BELOW -----------------------------------------

// function appBuilderGetSingleton() {
//     return Module.AppBuilder.getSingleton();
// }

// function installEventListenerById(elementId) {
//     console.log("installEventListenerById: ", elementId);
//     var abSingleton = appBuilderGetSingleton();
//     var webEl = abSingleton.getWebElement(elementId);
//     var domEl = webEl.getDomElement();

//     var evListener = function (ev) {
//         console.log("EVLISTENER in installEventListenerById method called!!!!!")
//         staticTick();
//         //abSingleton.threadTestFn();
//     }

//     domEl.addEventListener("click", evListener);
// }

// function appBuilderSingletonStart(tickInterval = 1000) {
//     setInterval(staticTick, tickInterval);
//     //appBuilderGetSingleton().start(tickInterval);
// }

// function appBuilderSingletonTick() {
//     //return function(ev) {
//     console.log("TICK!TICK!TICK!TICK!TICK!TICK!TICK!TICK!TICK!TICK!TICK!TICK!TICK!");
//     //appBuilderGetSingleton().start(1000);
//     //var abs = Module.AppBuilder.getSingleton();
//     //abs.tick();
//     console.log("TOCK!TOCK!TOCK!TOCK!TOCK!TOCK!TOCK!TOCK!TOCK!TOCK!TOCK!TOCK!TOCK!");
//     //abs.listChannels();
//     //}    
// }

// function staticTick() {
//     Module.AppBuilder.tick__();
// }

//function testObjSetDblval(obj, v) {
//     obj.setDblval(v);
// }

// function testObjGetDblval(obj) {
//     return obj.getDblval();
// }


// function getTickerFn(obj) {
//     //var objWrapped = obj;
//     return function () {

//         var d = obj.getDblval();
//         d += 1.5;
//         obj.setDblval(d);
//         console.log("Double val from wrapped obj is: ", obj.getDblval())
//         obj.tick();
//     }
// }


// function pairChannelWithElement(channelId, elementId) {
//     console.log("pairChannelWithElement()", channelId, elementId);
//     var abSingleton = appBuilderGetSingleton();
//     var webEl = abSingleton.getWebElement(elementId);
//     var domEl = webEl.getDomElement();
//     var channel = abSingleton.getChannel(channelId);

//     var evListener = function (ev) {
//         console.log("EVLISTENER in pairing method called!!!!!")
//         channel.inject(ev.target.value, 0);
//         //abSingleton.tick();
//         staticTick();
//     }

//     domEl.addEventListener("change", evListener);

// }

//function getClosureOnObj(obj) {
    //     var objWrapped = Module.TestObj.transfer(obj);
    //     return function () {
    
    //         var d = objWrapped.getDblval();
    //         d += 1.5;
    //         objWrapped.setDblval(d);
    //         console.log("Double val from wrapped obj is: ", objWrapped.getDblval())
    //     }
    // }


// function setDblvalOnObj(obj, v) {
//     Module.TestObj.setDblvalOnObj(obj, v);
// }


// REALLY OLD STUFF BELOW -----------------------------------------

// function callIterateMethodOnObject(obj) {
//     return function() {
//         obj.iterateOnce();
//     }    
// }

// function logStuff(stuff) {
//     console.log("This is a simple signal endpoint: " + stuff);
//     return stuff;
// }

// function testListenerFn(ev) {
//     console.log("testListenerFn() FIRED!");
// }

// function elgEmitFn(webElementSignalObject) {
//     return function (ev) {
//         let text = ev.target.value;
//         console.log("elgEmitFn generated fn called. Emitting signal: " + text);
//         webElementSignalObject.emit(text);
//     };
// }

// function elgButtonFn(webElementSignalObject) {
//     return function (ev) {
//         let text = ev.target.id;        
//         console.log("elgButtonFn generated fn called. Emitting signal: " + text);
//         webElementSignalObject.emit(text);
//     };
// }

// function elgSelectEmitFn(SelectEmitter) {
//     return function (ev) {
//         let text = ev.target.value;
//         console.log("elgSelectEmitFn generated fn called. Emitting signal: " + text);
//         SelectEmitter.emit(text);
//     };
// }

// function elgMouseSignal(mouseSignal, domElement) {
//     return function (ev) {
//         //let text = ev.target.value;
//         var boundingBox = domElement.getBoundingClientRect();
//         console.log("boundingBox: ", boundingBox);
//         //var viewBox = domElement.getAttribute("viewBox");
//         let relX = ev.clientX - boundingBox.x;
//         let relY = ev.clientY - boundingBox.y;


//         var viewBoxAttr = domElement.getAttribute("viewBox");
//         console.log("viewBox: ", viewBoxAttr);
//         var loc;
//         if (viewBoxAttr == null) {
//             loc = Module.MouseSignal.packageCoordPair(relX, relY);
//         }
//         else {
//             var viewBox = domElement.viewBox;
//             console.log("viewBox.baseVal.width: ", viewBox.baseVal.width);
//             let ratioX = relX / boundingBox.width;
//             let ratioY = relY / boundingBox.height;
//             let vbX = viewBox.baseVal.width * ratioX + viewBox.baseVal.x;
//             let vbY = viewBox.baseVal.height * ratioY + viewBox.baseVal.y;
//             loc = Module.MouseSignal.packageCoordPair(vbX, vbY);
//             console.log("viewBox location (x,y): ", vbX, vbY);
//         }
//         //console.log("elgMouseSignal generated fn called. Emitting signal: " + loc);
//         mouseSignal.emit(loc);
//     };
// }

// function elgTestObjEmitter(testObjCSO) {
//     return function () {
//         let s = testObjCSO.getSignal();
//         console.log("elgTestObjEmitter generated fn called. Emitting signal: " + s);
//         testObjCSO.emit(s);
//     };
// }

// function elgObjEmitter(objEmitter) {
//     return function () {
//         console.log("elgObjEmitter generated fn called. Originating signal.");
//         objEmitter.emit();
//     };
// }

// function elgMergeRecompute(mergeSO) {
//     return function () {
//         //let s = testObjCSO.getSignal();
//         console.log("elgMergeRecompute generated fn called. Recomputing...:");
//         mergeSO.recompute();
//     };
// }

// function injectForWEC(channel) {
//     return function (ev) {
//         //let s = testObjCSO.getSignal();
//         console.log("injectForWEC generated fn called.");
//         channel.injectForWEC(ev.target.value);
//     };
// }

window.WebElement = WebElement