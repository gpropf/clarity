//dom-proxy.js

function gv(element_name, attribute_name) {
    var el = window.document.getElementById(element_name);
    return parseInt(el[attribute_name]);
}

class CLElement {
    constructor() {
        
        this._type = "";
        this._id = "";
        this._dom_element = null;
    }

    init(type, id) {
        this._type = type;
        this._id = id;
    }

    createCLElement() {
        return new CLElement();
    }

    setval(v) {
        
    }

    get type() {
        return this._type;
    }

    set type(type) {
        this._type = type;
    }

    set id(id) {
        this._id = id;
        var el = document.getElementById(this._id);
        this._dom_element = el;
        this._dom_element.onchange = function() {
            alert(el.value);
        };
    }


    set value(v) {
        this._dom_element.value = v;
    }
}


window.CLElement = CLElement;
window.gv = gv;


