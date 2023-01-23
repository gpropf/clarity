class WebElement {
    constructor() {}

    initElement(tag, name, id) {
        this.tag_ = tag;
        this.id_ = id;
        this.name_ = name;
        this.domElement_ = this.createDOMElementByTagType();
        document.body.appendChild(this.domElement_); // Fixme
        return this.domElement_;
    }

    static tagToUrl = {
        "svg": "http://www.w3.org/2000/svg",
        "circle": "http://www.w3.org/2000/svg",
        "ellipse": "http://www.w3.org/2000/svg"
    };


    createDOMElementByTagType() {
        var el
        if (WebElement.tagToUrl[this.tag_]) {
            el = document.createElementNS(WebElement.tagToUrl[this.tag_], this.tag_)
        }
        else {
            el = document.createElement(this.tag_)
        }
        // if (JSProxyNode.extraInitCode[this.tag_]) {
        //   JSProxyNode.extraInitCode[this.tag_](el);
        // }
        return el
    }

}

function logStuff(stuff) {
    console.log("This is a simple signal endpoint: " + stuff);
}

function testListenerFn(ev) {
    console.log("testListenerFn() FIRED!");
}

function elgEmitFn(webElementSignalObject) {
    return function(ev) {
        webElementSignalObject.emit("text field change event called!");
    };
}

window.WebElement = WebElement