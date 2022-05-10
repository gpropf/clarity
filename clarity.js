//dom-proxy.js

// function gv(element_name, attribute_name) {
//     var el = window.document.getElementById(element_name);
//     return parseInt(el[attribute_name]);
// }

class CLElement {
    constructor() {}

    init(type, id) {
        this._type = type;
        this._id = id;
    }

    createCLElement() {
        return new CLElement();
    }
    
    set owner(owner) {}

    get owner() {
        return this._owner;
    }

    get type() {
        return this._type;
    }

    set type(type) {        
        this._type = type;
    }

    set id(id) {
        this._id = id;
        console.log("ID {id} being set in C++ constructor.");
        var el = document.getElementById(this._id);
        this._dom_element = el;        
        this._dom_element.onchange = function() {            
            console.log("Javascript onchange callback called")
            this._cvar = parseInt(el.value);
            Module.CLElement_CPP.updateCurrent(this._cvar, id);            
        };
    }

    set value(v) {
        console.log("Setting value in JS to:" + v)
        this._dom_element.value = v;
    }
}

window.CLElement = CLElement;
