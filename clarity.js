//dom-proxy.js

// function gv(element_name, attribute_name) {
//     var el: window.document.getElementById(element_name);
//     return parseInt(el[attribute_name]);
// }

// Run with 'python3 -m http.server 8000'

class CLElement {
  constructor() {}

  jsToCPPVal(jsval) {
    switch (this.cpptype_) {
      case Module.WebElementCppType.Int:
        return parseInt(jsval)
      case Module.WebElementCppType.Float:
        return parseFloat(jsval)
      case Module.WebElementCppType.NoData:
        return null;
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
    this.owner_ = owner
  }

  get owner() {
    return this.owner_
  }

  set cpptype(cpptype) {
    this.cpptype_ = cpptype
  }

  get cpptype() {
    return this.cpptype_
  }

  get tag() {
    return this.tag_
  }

  set tag(tag) {
    this.tag_ = tag
  }

  get type() {
    return this.type_
  }

  set type(type) {
    this.type_ = type
  }

  get anyval() {
    return this.anyval_
  }

  set anyval(anyval) {
    this.anyval_ = anyval
  }

  get domElement() {
    return this.domElement_
  }

  set id(id) {
    //this.owner_ = Module.WebElement.getCLElementById[id]
    this.id_ = id
    //console.log(`jsToCPPVal: ${this.jsToCPPVal('1.3')}`)
    //this.owner_ = Module.WebElement.getCLElementById(id);
    console.log(`ID ${id} being set by C++ constructor.`)
    var el = document.getElementById(this.id_)
    if (el == null) {
      //el = document.createElement(this.getTag())
      console.log(`ELEMENT ${id}: tag is ${this.tag_}`)
      el = document.createElement(this.tag_)
      document.body.appendChild(el)
      el.id = id
      el.type = this.type_
      
    }
    this.domElement_ = el
    
    if (this.tag_ == "input") {
      var outerThis = this;
      this.domElement_.addEventListener('change', function (e) {
        outerThis.anyval_ = outerThis.jsToCPPVal(outerThis.domElement_.value)
        console.log(`${outerThis.id}: JS value is ${outerThis.anyval_}\n`)
        console.log('Javascript onchange callback called')
        Module.WebElement.updateVal(outerThis.id_)
      })
    }
  }

  get id() {
    return this.id_;
  }  
}

window.CLElement = CLElement
