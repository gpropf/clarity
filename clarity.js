
// Run with 'python3 -m http.server 8000'

class CLElement {

  constructor() { }

  static tagToUrl = { "svg": "http://www.w3.org/2000/svg", "circle": "http://www.w3.org/2000/svg" };
  static extraInitCode = {
    "canvas": function (domElement) {
      if (domElement.getContext) {
        domElement.style.display = 'none';
        domElement.style.display = 'block';
        let ctx = domElement.getContext('2d');
        ctx.fillStyle = 'red';
        ctx.beginPath();
        ctx.arc(250, 40, 32, 0, 2 * Math.PI);
        ctx.fill();
        // console.log("extraInitCode['canvas']()")
      }
    }
  };

  /**
   * See note below on these generator functions.
   */
  static eventHandlerGenerators = {
    "input": [{
      "eventName": "change", "eventHandlerGenerator":
        function (element) {
          return function (e) {
            Module.ClarityNode.updateNodeFromDomById(element.id)
            if (element.boundField != undefined)
              console.log("For ID: " + element.id + " Value changed to: " + element.domElement[element.boundField]);
            else
              console.log("For ID: " + element.id + " Value changed");
          }
        }
    }],
    "textarea": [{
      "eventName": "input", "eventHandlerGenerator":
        function (element) {
          return function (e) {
            console.log("KEYPRESSED!");
          }
        }
    },
    {
      "eventName": "change", "eventHandlerGenerator":
        function (element) {
          return function (e) {
            console.log("textarea lost focus!");
            Module.ClarityNode.updateNodeFromDomById(element.id)
            if (element.boundField != undefined)
              console.log("For ID: " + element.id + " Value changed to: " + element.domElement[element.boundField]);
            else
              console.log("For ID: " + element.id + " Value changed");
          }
        }
    }],
    "select": [],
    "option": []
  }



  multiplyValues(a, b) {
    return a * b
  }

  invertValue(v) {
    return 1 / v
  }

  static square(x) {
    return x ** 2;
  }

  static doNothing() {
    console.log("doNothing(): a function that proudly does nothing...");
  }

  static nodeAudit_int() {
    Module.HybridNode.nodeAudit_int();
  }

  static nodeAudit_double() {
    Module.HybridNode.nodeAudit_double();
  }

  static destroy_everything() {
    console.log("destroy_everything():BOOM!");
    Module.ClarityNode.runCallbackById("destroy_everything")
  }

  static make_trs_ints() {
    console.log("JS:make_trs_ints()");
    Module.ClarityNode.runCallbackById("make_trs_ints");
  }

  printVal(v) {
    console.log("printval(v):" + "id: " + this.id + ". JSVAL = " + v);
  }

  static zerofloor(n) {
    if (n < 0) return 0;
    return n;
  }

  static draw_test_pattern() {
    console.log("draw_test_pattern");
    let canvasEl = document.getElementById(2);
    var canvasCode = CLElement.extraInitCode["canvas"];
    canvasCode(canvasEl);
  }


  drawBlueTestPattern() {
    var domElement = this.domElement_;
    // let domElement = this.domElement;
    if (domElement.getContext) {
      domElement.style.display = 'none';
      domElement.style.display = 'block';
      let ctx = domElement.getContext('2d');
      ctx.scale(2, 2);
      ctx.fillStyle = 'blue';
      ctx.fillRect(10, 10, 60, 60);
      ctx.fillRect(100, 10, 90, 60);

      ctx.beginPath();
      ctx.arc(250, 40, 32, 0, 2 * Math.PI);
      ctx.fill();

      ctx.beginPath();
      ctx.moveTo(10, 160);
      ctx.lineTo(90, 160);
      ctx.lineTo(50, 110);
      ctx.closePath();
      ctx.fill();

      ctx.save();
      ctx.scale(2, 1);
      ctx.beginPath();
      ctx.arc(72, 130, 25, 0, 2 * Math.PI);
      ctx.fill();
      ctx.restore();

      ctx.beginPath();
      ctx.arc(250, 120, 40, 0, Math.PI);
      ctx.fill();
      //alert(ctx);
      console.log("drawBlueTestPattern()");

    }
  }



  canvasTestPattern() {
    var domElement = this.domElement_;
    if (domElement.getContext) {
      domElement.style.display = 'none';
      domElement.style.display = 'block';
      let ctx = domElement.getContext('2d');
      let [w, h, hw, hh] = [domElement.width, domElement.height, domElement.width / 2, domElement.height / 2];
      let [qw, qh] = [w / 4, h / 4];
      ctx.scale(1, 1);
      ctx.fillStyle = 'green';
      ctx.fillRect(0, 0, w, h);
      ctx.fillStyle = 'blue';
      ctx.fillRect(qw, qh, hw, hh);
      console.log("drawGreenTestPattern()");
    }
  }


  /**
   * 
   * @param {*} temp: In degrees Kelvin 
   * @param {*} originalColor: the object's 'actual' color at 0 K.
   * @returns a very crude (for now) attempt at showing hot objects glowing the right way. This
   * is returned as a string representing an RGB triplet with values from 0-255.
   */
  static blackbody_st(temp, originalColor = { "r": 22, "g": 45, "b": 70 }) {
    // console.log(originalColor);
    temp += 273;
    // return "#aa0ff9";

    var r = originalColor.r + CLElement.zerofloor(temp - 500) / 2;
    var g = originalColor.g + CLElement.zerofloor(temp - 1000) / 5;
    var b = originalColor.b + CLElement.zerofloor(temp - 1500) / 5;

    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    var rgbstr = `rgb(${r},${g},${b})`;
    return rgbstr;

  }

  blackbody(temp) {
    var originalColor = { "r": 0, "g": 200, "b": 75 };
    return CLElement.blackbody_st(temp, originalColor);
  }

  /**
   * 
   * @param {*} x 
   * @returns Whatever you pass in. This is mainly for anything that is not some kind of numeric value.
   */
  passthru(x) {
    return x;
  }

  static generateTransformFn(constantOrFunction) {
    if (constantOrFunction instanceof Function) {
      // console.log("Multiplier is function")
      return constantOrFunction;
    }
    var generatedMultiplierFunction = (x) => x * constantOrFunction;
    // If you just do 'return (x) => x * multiplier;' the function has no name attribute.
    return generatedMultiplierFunction;
  }

  static applyTransformFn(f, v) {
    if (!f) { console.log("F IS UNDEFINED") }
    var r = f.call(f, v);
    //console.log("Value " + v + ", Transformed into " + r + " by function " + f.name);
    return r;
  }

  appendChild(child) {
    if (child.domElement && child.domElement instanceof Element)
      this.domElement.appendChild(child.domElement)
  }

  createCLElement() {
    return new CLElement()
  }

  set boundField(boundField) {
    this.boundField_ = boundField;
  }

  get boundField() {
    return this.boundField_;
  }

  set owner(owner) {
    this.owner_ = owner
  }

  get owner() {
    return this.owner_
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
  }

  get name() {
    return this.domElement_.name;
  }


  createDOMElementByTagType() {
    var el
    if (CLElement.tagToUrl[this.tag_]) {
      el = document.createElementNS(CLElement.tagToUrl[this.tag_], this.tag_)
    }
    else {
      el = document.createElement(this.tag_)
    }
    if (CLElement.extraInitCode[this.tag_]) {
      CLElement.extraInitCode[this.tag_](el);
    }
    return el
  }

  createDOMElement(id, tag, name) {
    this.id_ = id
    this.tag_ = tag
    this.name_ = name
    this.boundField_ = "value"; // FIXME

    // console.log(`ID ${id} from ticketMachine.`)
    var el = document.getElementById(this.id_)
    if (el == null) {
      el = document.getElementById(this.name_)
      if (el == null) {
        el = this.createDOMElementByTagType()

        document.body.appendChild(el)
        // Without this the newly created elements get garbage collected and vanish.
        // The idea is that you append them later to their actual parents using a call
        // in C++ to the appendChild method.

        el.id = this.id_
        el.name = this.name_
        el.setAttribute("name", name);
      }
    }
    this.domElement_ = el
    this.generateEventHandlers(this);
  }

  /**
   * We need to generate event listeners for each node we create of a given type. 
   * There is something about the way the closure for the functions is defined that
   * make 'this' not work as expected so I explicitly pass in a parameter to each
   * generator that references the element's 'this' correctly. Storing the actual handlers
   * simply does not work as the value of the 'element' variable is different for each
   * field.
   * 
   * @param {CLElement} outerThis 
   */
  generateEventHandlers(outerThis) {
    if (outerThis.tag_ in CLElement.eventHandlerGenerators) {
      let listenerList = CLElement.eventHandlerGenerators[outerThis.tag_];
      listenerList.map(function (kv) {
        outerThis.domElement_.addEventListener(kv.eventName, kv.eventHandlerGenerator(outerThis));
      })
    }
  }

  /**
   * 
   */
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

  static setGridLocToCurrentVal(ev) { // FIXME: Need to rework to remove translator stuff.
    let domElement = ev.target;
    let domrect = domElement.getBoundingClientRect();

    let gw = domElement.gw;
    let cw = domElement.cw;
    let gh = domElement.gh;
    let ch = domElement.ch;
    //let cellDimensions = domElement.cellDimensions;
    //let gridDimensions = domElement.gridDimensions;
    var gridDimensions = { w: gw, h: gh };
    let loc = { x: (ev.x - domrect.x), y: (ev.y - domrect.y) };
    var locIn = { x: Math.floor(loc.x / cw), y: Math.floor(loc.y / ch) };
    //let locOut = Util.wrapCoords(locIn, gridDimensions);
    var locOut = Util.capCoords(locIn, gridDimensions);
    domElement.gridRef.setValXY(locOut.x, locOut.y);
    //console.log(locOut);
  }


}

//let myVar = setInterval(myTimer, 300);
function myTimer() {
  const d = new Date();
  // console.log(d.toLocaleTimeString())
  Module.ClarityNode.runCallbackById("tick")
}

window.CLElement = CLElement

