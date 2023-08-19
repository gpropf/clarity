/**
 * @file Clarity.js
 * @author Greg Propf (gpropf@gmail.com)
 * @brief This is the library of basic JavaScript functions needed by the Clarity framework.
 * @version 0.1
 * @date 2023-03-01
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

function logStuff(stuff) {
    console.log("This is a simple signal endpoint: " + stuff);
    return stuff;
}

function testListenerFn(ev) {
    console.log("testListenerFn() FIRED!");
}

function elgEmitFn(webElementSignalObject) {
    return function (ev) {
        let text = ev.target.value;
        console.log("elgEmitFn generated fn called. Emitting signal: " + text);
        webElementSignalObject.emit(text);
    };
}

function elgButtonFn(webElementSignalObject) {
    return function (ev) {
        let text = ev.target.id;
        // It is implied in the use of the name field that we are emitting
        // a string value. If this is not the case, problems will arise.
        console.log("elgButtonFn generated fn called. Emitting signal: " + text);
        webElementSignalObject.emit(text);
    };
}

function elgSelectEmitFn(SelectEmitter) {
    return function (ev) {
        let text = ev.target.value;
        console.log("elgSelectEmitFn generated fn called. Emitting signal: " + text);
        SelectEmitter.emit(text);
    };
}

function elgMouseSignal(mouseSignal, domElement) {
    return function (ev) {
        //let text = ev.target.value;
        var boundingBox = domElement.getBoundingClientRect();
        console.log("boundingBox: ", boundingBox);
        //var viewBox = domElement.getAttribute("viewBox");
        let relX = ev.clientX - boundingBox.x;
        let relY = ev.clientY - boundingBox.y;


        var viewBoxAttr = domElement.getAttribute("viewBox");
        console.log("viewBox: ", viewBoxAttr);
        var loc;
        if (viewBoxAttr == null) {
            loc = Module.MouseSignal.packageCoordPair(relX, relY);
        }
        else {
            var viewBox = domElement.viewBox;
            console.log("viewBox.baseVal.width: ", viewBox.baseVal.width);
            let ratioX = relX / boundingBox.width;
            let ratioY = relY / boundingBox.height;
            let vbX = viewBox.baseVal.width * ratioX + viewBox.baseVal.x;
            let vbY = viewBox.baseVal.height * ratioY + viewBox.baseVal.y;
            loc = Module.MouseSignal.packageCoordPair(vbX, vbY);
            console.log("viewBox click location (x,y): ", vbX, vbY);
        }
        //console.log("elgMouseSignal generated fn called. Emitting signal: " + loc);
        mouseSignal.emit(loc);
    };
}

function elgTestObjEmitter(testObjCSO) {
    return function () {
        let s = testObjCSO.getSignal();
        console.log("elgTestObjEmitter generated fn called. Emitting signal: " + s);
        testObjCSO.emit(s);
    };
}

function elgObjEmitter(objEmitter) {
    return function () {
        console.log("elgObjEmitter generated fn called. Originating signal.");
        objEmitter.emit();
    };
}

function elgMergeRecompute(mergeSO) {
    return function () {
        //let s = testObjCSO.getSignal();
        console.log("elgMergeRecompute generated fn called. Recomputing...:");
        mergeSO.recompute();
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
        //let s = testObjCSO.getSignal();
        console.log("elgCallMethodOnObjByName generated fn called.");
        callMethodByName(obj, objMethodName);
        //obj.printTest();
    };
}

window.WebElement = WebElement