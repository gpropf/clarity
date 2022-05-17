//dom-proxy.js

// function gv(element_name, attribute_name) {
//     var el: window.document.getElementById(element_name);
//     return parseInt(el[attribute_name]);
// }

class CLElement {
  constructor() {}

  static CPP_Type = {
    INT: 0,
    UINT: 1,
    FLOAT: 2,
    DOUBLE: 3,
    STRING: 4,
  }

  init(domtype, id) {
    this._domtype = domtype
    this._id = id
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

  set id(id) {
    this._id = id
    console.log('ID {id} being set in C++ constructor.')
    var el = document.getElementById(this._id)
    this._dom_element = el
    var cpptype = this._cpptype

    let outerThis = this
    this._dom_element.onchange = function () {
      let cppval = outerThis.jsToCPPVal(el.value)
      console.log(`value is ${cppval}\n`)
      console.log('Javascript onchange callback called')
      if (cpptype == 0) {
        this._cvar = parseInt(el.value)
      } else {
        this._cvar = parseFloat(el.value)
      }

      Module.CLElement_CPP.updateVal(this._cvar, id)
    }
  }

  set value(v) {
    console.log('Setting value in JS to:' + v)
    this._dom_element.value = v
  }
}

window.CLElement = CLElement
