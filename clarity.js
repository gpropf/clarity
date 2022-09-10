
// Run with 'python3 -m http.server 8000'

class CLElement {

  constructor() { }

  multiplyValues(a, b) {
    return a * b
  }

  invertValue(v) {
    return 1 / v
  }

  static square(x) {
    return x ** 2;
  }

  static testAlert() {
    alert("FOO!")
  }

  static doNothing() {
    console.log("doNothing(): a function that proudly does nothing...");
  }

  printVal(v) {
    console.log("JSVAL = " + v);
  }

  /**
   * 
   * @param {*} temp: In degrees Kelvin 
   * @returns a crude (for now) attempt at showing hot objects glowing the right way. This
   * is returned as a string representing an RGB triplet with values from 0-255.
   */
  static blackbody_st(temp) {
    temp+=600;
    // return "#aa0ff9";
    var r = temp - 500;
    var g = r / 2;
    var b = r / 3;
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    var rgbstr = `rgb(${r},${g},${b})`;
    return rgbstr;

  }

  blackbody(temp) {
    return blackbody_st(temp);
  }

  static generateTransformFn(constantOrFunction) {
    if (constantOrFunction instanceof Function) {
      console.log("Multiplier is function")
      return constantOrFunction;
    }
    var generatedMultiplierFunction = (x) => x * constantOrFunction;
    // If you just do 'return (x) => x * multiplier;' the function has no name attribute.
    return generatedMultiplierFunction;
  }

  static applyTransformFn(f, v) {
    if (!f) {  console.log("F IS UNDEFINED") }
    var r = f.call(f, v);
    console.log("Value " + v + ", Transformed into " + r + " by function " + f.name);
    return r;
  }


  jsToCPPVal(jsval) {

    switch (this.cpptype_) {

      case Module.CppType.Int:
        console.log("jsToCPPVal: Int")
        return parseInt(jsval)
      case Module.CppType.Float:
        console.log("jsToCPPVal: Float")
        return parseFloat(jsval)
      case Module.CppType.Double:
        console.log("jsToCPPVal: Double")
        return parseFloat(jsval)
      case Module.CppType.NoData:
        console.log("jsToCPPVal: NoData")
        return null
      default:
        return jsval
    }
  }
  //object.freeze(CPP_Type);

  appendChild(child) {
    if (child.domElement && child.domElement instanceof Element)
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

  set domElement(domElement) {
    this.domElement_ = domElement
  }

  set name(name) {
    this.name_ = name;
    if (this.domElement_) {
      this.domElement_.setAttribute("name", name);
    }
    //
  }

  get name() {
    return this.domElement_.name;
  }

  static tagToUrl = { "svg": "http://www.w3.org/2000/svg", "circle": "http://www.w3.org/2000/svg" };

  createDOMElementByTagType() {
    var el
    if (CLElement.tagToUrl[this.tag_]) {
      el = document.createElementNS(CLElement.tagToUrl[this.tag_], this.tag_)
    }
    else {
      el = document.createElement(this.tag_)
    }
    return el
  }

  createDOMElement(id, tag, cpptype, name) {
    this.id_ = id
    this.tag_ = tag
    this.cpptype_ = cpptype
    this.name_ = name

    console.log(`ID ${id} from ticketMachine.`)
    var el = document.getElementById(this.id_)
    if (el == null) {
      el = document.getElementById(this.name_)
      if (el == null) {
        console.log(`ELEMENT ${id}: tag is ${this.tag_}`)
        el = this.createDOMElementByTagType()
        document.body.appendChild(el)
        // Without this the elements get garbage collected and vanish. The idea is that you append them later to
        // their actual parents using a call in C++ to the appendChild method.

        el.id = this.id_
        el.type = this.type_
        el.name = this.name_
      }
    }
    this.domElement_ = el

    if (this.tag_ == 'input') {
      var outerThis = this
      this.domElement_.addEventListener('change', function (e) {
        console.log(`Javascript onchange callback called for outerThis.id_ = ${outerThis.id_}`)
        Module.ClarityNode.pushValToPeersById2(outerThis.id_)
        Module.ClarityNode.markNodeDirtyById(outerThis.id_)
      })
    }
  }

  set id(id) {
    this.id_ = id
  }

  addEventListenerById(eventName, id) {
    this.domElement_.addEventListener(eventName, (e) => {
      Module.ClarityNode.runCallbackById(id)
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
    console.log(typeof (v));
  }

}

//let myVar = setInterval(myTimer, 300);
function myTimer() {
  const d = new Date();
  console.log(d.toLocaleTimeString())
  Module.ClarityNode.runCallbackById("tick")
}

window.CLElement = CLElement

