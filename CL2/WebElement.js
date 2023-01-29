class WebElement {
    constructor() { }

    /**
     * This is actually what creates the DOM element. The rule for ids is that
     * the id is not set if the id argument is negative.
     * 
     * @param {str} tag 
     * @param {str} name 
     * @param {int} id 
     * @returns 
     */
    static initElement(tag, name, id = null) {
        // this.tag_ = tag;
        // this.id_ = id;
        // this.name_ = name;
        var domElement = WebElement.createDOMElementByTagType(tag);
        if (typeof (id) === 'number') {
            if (id >= 0)
                domElement.id = id;
        }
        else {
            domElement.id = id;
        }
        domElement.name = name;
        domElement.setAttribute("name", name);
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
}

function testListenerFn(ev) {
    console.log("testListenerFn() FIRED!");
}

function elgEmitFn(webElementSignalObject) {
    return function (ev) {
        let text = ev.target.value;
        webElementSignalObject.emit(text);
    };
}

window.WebElement = WebElement