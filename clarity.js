
// Run with 'python3 -m http.server 8000'

class CLElement {
  constructor() {}

  jsToCPPVal(jsval) {
    switch (this.cpptype_) {
      case Module.WebElementCppType.Int:
        return parseInt(jsval)
      case Module.WebElementCppType.Float:
        return parseFloat(jsval)
      case Module.WebElementCppType.Double:
        return parseFloat(jsval)
      case Module.WebElementCppType.NoData:
        return null
      default:
        return jsval
    }
  }

  //object.freeze(CPP_Type);
 
  appendChild(child) {
    this.domElement.appendChild(child.domElement)
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

  get domElement() {
    return this.domElement_
  }

  set name(name) {
    this.name_ = name;
    this.domElement_.setAttribute("name", name);
  }

  get name() {
    return this.domElement_.name;
  }

  set id(id) {   
    this.id_ = id    
    console.log(`ID ${id} being set by C++ constructor.`)
    var el = document.getElementById(this.id_)
    if (el == null) {      
      console.log(`ELEMENT ${id}: tag is ${this.tag_}`)
      el = document.createElement(this.tag_)
      document.body.appendChild(el)
      el.id = id
      el.type = this.type_
    }
    this.domElement_ = el    

    if (this.tag_ == 'input') {
      var outerThis = this
      this.domElement_.addEventListener('change', function (e) {        
        console.log('Javascript onchange callback called')
        //outerThis.printState()
        Module.WebElement.updateModelFromViewById(outerThis.id_) 
        Module.ControlNetworkNode.pushValToPeersById(outerThis.id_)
        Module.ControlNetworkNode.markNodeDirtyById(outerThis.id_)
      })
    }
  }

  addEventListenerById(eventName, id) {
    this.domElement_.addEventListener(eventName, (e) => {
      Module.WebElement.runCallbackById(id)
    })
  }

  get id() {
    return this.id_
  }

  printState() {
    console.log(`CNN: FOR ID: ${this.id}, THIS VAL = ${this.domElement_.value}`);
  }

  printToConsole(v) {
    console.log(v)
  }

  printType(v) {
    console.log(typeof(v));
  }

}

window.CLElement = CLElement