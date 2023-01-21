class WebElement {
    constructor(tag, id, name) {
        this.tag_ = tag;
        this.id_ = id;
        this.name_ = name;
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