//dom-proxy.js

// function gv(element_name, attribute_name) {
//     var el: window.document.getElementById(element_name);
//     return parseInt(el[attribute_name]);
// }

class CLElement {
  constructor() {}

  
  translateTag() {
    switch (this._tag) {
      case Module.CLElement_CPPTag.Div:
        return "div";
      case Module.CLElement_CPPTag.Button:
        return "button";
      case Module.CLElement_CPPTag.Input:
        return "input";
      default:
        return "br";
    }
  }


  jsToCPPVal(jsval) {
    switch (this._cpptype) {
      case Module.CLElement_CPPCppType.Int:
        return parseInt(jsval)
      case Module.CLElement_CPPCppType.Float:
        return parseFloat(jsval)
      case Module.CLElement_CPPCppType.NoData:
        return null;
      default:
        return jsval
    }
  }

  //object.freeze(CPP_Type);

  installEventHandlers() {
    let outerThis = this
    var handlerMap = new Map([
      [Module.CLElement_CPPTag.Div , null],
      [Module.CLElement_CPPTag.Button , null],
      [Module.CLElement_CPPTag.Input , function (e) {
        outerThis._anyval = outerThis.jsToCPPVal(outerThis._domElement.value)
        console.log(`${outerThis.id}: JS value is ${outerThis._anyval}\n`)
        console.log('Javascript onchange callback called')
        Module.CLElement_CPP.updateVal(outerThis._id)
        //this._owner.valueUpdated();
      }]]);
      
    
    this._domElement.addEventListener('change', handlerMap[this._tag]);
    console.log(this._domElement.onchange);
  }

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
    //this._owner = Module.CLElement_CPP.getCLElementById[id]
    this._id = id
    //console.log(`jsToCPPVal: ${this.jsToCPPVal('1.3')}`)
    //this._owner = Module.CLElement_CPP.getCLElementById(id);
    console.log(`ID ${id} being set by C++ constructor.`)
    var el = document.getElementById(this._id)
    if (el == null) {
      //el = document.createElement(this.getTag())
      console.log(`ELEMENT ${id}: tag is ${this._tag.value}`)
      el = document.createElement(this.translateTag())
      document.body.appendChild(el)
      el.id = id
      el.type = this._type
      
    }
    this._domElement = el
    

    
    // let outerThis = this
    // this._domElement.onchange = function () {
    //   outerThis._anyval = outerThis.jsToCPPVal(el.value)
    //   console.log(`${outerThis.id}: JS value is ${outerThis._anyval}\n`)
    //   console.log('Javascript onchange callback called')
    //   Module.CLElement_CPP.updateVal(id)
    //   //this._owner.valueUpdated();
    // }
    this.installEventHandlers()
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
