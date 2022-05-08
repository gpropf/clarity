//dom-proxy.js

function gv(element_name, attribute_name) {
    var el = window.document.getElementById(element_name);
    return parseInt(el[attribute_name]);
}

class CLElement {
    constructor() {
        
        // this._type = "";
        // this._id = "";
        // this._dom_element = null;
        // this._cvar = null;
        //this._owner = null;
    }

    init(type, id) {
        this._type = type;
        this._id = id;
    }

    createCLElement() {
        return new CLElement();
    }

    
    set owner(owner) {
        console.log("Trying to set owner from JS side.");
        this._owner = owner;
        console.log("this._owner = " + owner);
        
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
            //alert(el.value);
            console.log("Javascript onchange callback called")
            this._cvar = parseInt(el.value);
            
            //Module.lerp(9);
            this._owner["valueUpdated"]();
        };
    }


    set value(v) {
        this._dom_element.value = v;
    }
}


window.CLElement = CLElement;
window.gv = gv;


