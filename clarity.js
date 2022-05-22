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
    STRING: 2,
  };

  static Tagmap = {
    0: "div",
    1: "button",
    2: "input"
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

  appendChild(child) {
    this.domElement.appendChild(child.domElement);
  }

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

  get tag() {
    return this._tag
  }

  set tag(tag) {
    this._tag = tag
  }

  get type() {
    return this._type
  }

  set type(type) {
    this._type = type
  }

  get anyval() {
    return this._anyval
  }

  set anyval(anyval) {
    this._anyval = anyval
  }

  get domElement() {
    return this._domElement
  }

  set id(id) {
    this._id = id
    //this._owner = Module.CLElement_CPP.getCLElementById(id);
    console.log(`ID ${id} being set by C++ constructor.`)
    var el = document.getElementById(this._id)
    if (el == null) {
      el = document.createElement(CLElement.Tagmap[this._tag.value])
      document.body.appendChild(el)
      el.id = id
      el.type = this._type
      
    }
    this._domElement = el
    

    let outerThis = this
    this._domElement.onchange = function () {
      outerThis._anyval = outerThis.jsToCPPVal(el.value)
      console.log(`${outerThis.id}: JS value is ${outerThis._anyval}\n`)
      console.log('Javascript onchange callback called')
      Module.CLElement_CPP.updateVal(id)
      //this._owner.valueUpdated();
    }
  }

  get id() {
    return this._id;
  }

  //   set value(v) {
  //     console.log('Setting value in JS to:' + v)
  //     this._domElement.value = v
  //   }
}

window.CLElement = CLElement
