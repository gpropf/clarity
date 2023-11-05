/**
 * @file cl3.js
 * @author Greg Propf (gpropf@gmail.com)
 * @brief This is the library of basic JavaScript functions needed by the Clarity3 framework.
 * @version 0.3
 * @date 2023-11-04
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

function TickerTick(ticker) {
    return function () {
        ticker.tick();
    }
}

function generateInjectFn(channel) {
    console.log("On JS side creating inject listener for channel: ", channel);
    return function (ev) {
        console.log("inject generated fn called for channel at: ", channel);
        channel.inject(ev.target.value, 0);
    };
}

function generateEventListenerForChannel_AreaElement(channel, domElement) {
    console.log("generateEventListenerForChannel_AreaElement: ", channel);
    return function (ev) {
        //let text = ev.target.value;
        var boundingBox = domElement.getBoundingClientRect();
        // console.log("boundingBox: ", boundingBox);
        //var viewBox = domElement.getAttribute("viewBox");
        let relX = ev.clientX - boundingBox.x;
        let relY = ev.clientY - boundingBox.y;
        channel.inject([relX, relY], 0);
    }
}

function generateEventListenerForChannel_TextField(channel) {
    console.log("generateEventListenerForChannel_TextField: ", channel);
    return function (ev) {
        console.log("generateEventListenerForChannel_TextField --> inject generated fn called for channel at: ", channel);
        channel.inject(ev.target.value, 0);
    };
}

function generateEventListenerForChannel_RangeInput(channel) {
    console.log("generateEventListenerForChannel_RangeInput: ", channel);
    return function (ev) {
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

window.WebElement = WebElement