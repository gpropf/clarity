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
        "ellipse": "http://www.w3.org/2000/svg"
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

window.WebElement = WebElement