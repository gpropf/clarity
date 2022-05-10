//dom-proxy.js

function gv(element_name, attribute_name) {
    var el = window.document.getElementById(element_name);
    return parseInt(el[attribute_name]);
}

class CLElement {
    constructor() {
        
        // this._type = "";
         //this._id = "";
        // this._dom_element = null;
        // this._cvar = null;
        this._owner = null;
        //Object.keys(Module).forEach((prop)=> console.log(prop));
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
        console.log("Before setting: this._owner = " + this._owner);

        //this._owner = Module.CLElement_CPP.globalVec;

        //Object.keys(this._owner).forEach((prop)=> console.log(prop));
        //this._owner = owner;
        //console.log("After setting owner this._owner = " + owner);
        //Object.keys(owner).forEach((prop)=> console.log(prop));
        
    }

    get owner() {

        return this._owner;
    }

    get type() {
        return this._type;
    }

    set type(type) {
        //alert("TYPE:" + type);
        this._type = type;
    }

    set id(id) {
        this._id = id;
        console.log("ID {id} being set in C++ constructor.");
        var el = document.getElementById(this._id);
        this._dom_element = el;
        //this._owner = Module["getGlobalCLE"]();
        this._dom_element.onchange = function() {
            //alert(el.value);
            console.log("Javascript onchange callback called")
            this._cvar = parseInt(el.value);
            
            //Module.lerp(9);
            Module.CLElement_CPP.updateCurrent(this._cvar, id);
            //this._owner.valueUpdated();
        };
    }


    set value(v) {
        console.log("Setting value in JS to:" + v)
        this._dom_element.value = v;
    }
}


window.CLElement = CLElement;
window.gv = gv;


