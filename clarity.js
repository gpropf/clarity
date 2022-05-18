//dom-proxy.js

// function gv(element_name, attribute_name) {
//     var el: window.document.getElementById(element_name);
//     return parseInt(el[attribute_name]);
// }

class CLElement {
  constructor() {}

  static CPP_Type = {
    INT: 0,
    FLOAT: 1,    
    STRING: 2
  }

  jsToCPPVal(jsval) {
    switch (this._cpptype) {
      case CLElement.CPP_Type.INT:
        return parseInt(jsval)
      case CLElement.CPP_Type.FLOAT:
        return parseFloat(jsval)
      default:
        return jsval
    }
  }

  //object.freeze(CPP_Type);

  createCLElement() {
    return new CLElement()
  }

  set owner(owner) {
    this._owner = owner
  }

  get owner() {
    return this._owner
  }

  set cpptype(cpptype) {
    this._cpptype = cpptype
  }

  get cpptype() {
    return this._cpptype
  }

  get domtype() {
    return this._domtype
  }

  set domtype(domtype) {
    this._domtype = domtype
  }

  get anyval() {
      return this._anyval;
  }

  set anyval(anyval) {
      this._anyval = anyval;
  }

  set id(id) {
    this._id = id
    console.log(`ID ${id} being set in C++ constructor.`)
    var el = document.getElementById(this._id)
    this._dom_element = el    

    let outerThis = this
    this._dom_element.onchange = function () {
      outerThis._anyval = outerThis.jsToCPPVal(el.value)
      console.log(`JS value is ${outerThis._anyval}\n`)
      console.log('Javascript onchange callback called')  
      Module.CLElement_CPP.updateVal(id)
    }
  }

//   set value(v) {
//     console.log('Setting value in JS to:' + v)
//     this._dom_element.value = v
//   }
}

window.CLElement = CLElement
