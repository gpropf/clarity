//dom-proxy.js

function gv(element_name, attribute_name) {
    var el = window.document.getElementById(element_name);
    return parseInt(el[attribute_name]);
}

class CLElement {
    constructor() {
        this._type = "";
        this._id = "";
    }

    init(type, id) {
        this._type = type;
        this._id = id;
    }

    createCLElement() {
        return new CLElement();
    }

    get_type() {
        return this._type;
    }
}


window.CLElement = CLElement;
window.gv = gv;


