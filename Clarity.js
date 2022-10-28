/**
 * 
 * CLElement class and helper functions. This is the main JS file in the project.
 */

function loadAuxScript(scriptName) {
  var el = document.createElement('script');
  el.async = false;
  el.src = ClarityInstallDir + '/js.d/' + scriptName;
  el.type = 'text/javascript';  
  (document.getElementsByTagName('HEAD')[0] || document.body).appendChild(el);
}

var auxScripts = ["auxExample1.js","auxExample2.js"];
auxScripts.map(loadAuxScript);

class CLElement {

  constructor() { }

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

  set tag(tag) {
    this.tag_ = tag
  }

  get tag() {
    return this.tag_
  }
 
  set type(type) {
    this.type_ = type
  }

  get type() {
    return this.type_
  }
  
  set domElement(domElement) {
    this.domElement_ = domElement
  }

  get domElement() {
    return this.domElement_
  } 

  set name(name) {
    this.name_ = name;
    if (this.domElement_) {
      this.domElement_.setAttribute("name", name);
    }
  }

  get name() {
    //return this.domElement_.name;
    return this.name_;
  }

  set id(id) {
    this.id_ = id
  }
  
  get id() {
    return this.id_
  }

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
  static eventListenerGenerators = {
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

  eventHandlerGeneratorsByClass = {
    a:2
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
    //console.log("JS:make_trs_ints()");
    Module.ClarityNode.runCallbackById("make_trs_ints");
  }

  printVal(v) {
    console.log("printval(v):" + "id: " + this.id + ". JSVAL = " + v);
  }

  static zerofloor(n) {
    if (n < 0) return 0;
    return n;
  }

  
  /**
   * 
   * @param {*} temp: In degrees Kelvin 
   * @param {*} originalColor: the object's 'actual' color at 0 K.
   * @returns a very crude (for now) attempt at showing color color change due to heating.
   * Color is returned as a string representing an RGB triplet with values from 0-255.
   */
  static blackbody_st(temp, originalColor = { "r": 15, "g": 15, "b": 15 }) {
    temp += 273;

    var r = originalColor.r + CLElement.zerofloor(temp - 500);
    var g = originalColor.g + CLElement.zerofloor(temp - 1000) / 10;
    var b = originalColor.b + CLElement.zerofloor(temp - 1500) / 10;

    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    var rgbstr = `rgb(${r},${g},${b})`;
    return rgbstr;
  }  

  static generateTransformFn(constantOrFunction) {
    if (constantOrFunction instanceof Function) {
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

  placeElement(existingElement, el, attachmentMode) {
    if (existingElement == null) attachmentMode = Module.AttachmentMode.NEW;
    switch (attachmentMode) {
      case Module.AttachmentMode.ATTACH_ID:
      case Module.AttachmentMode.ATTACH_NAME:
        existingElement.appendChild(el);
        break;
      case Module.AttachmentMode.REPLACE_ID:
      case Module.AttachmentMode.REPLACE_NAME:
        existingElement.parentNode.replaceChild(el, existingElement);
        break;
      case Module.AttachmentMode.NEW:
        document.body.appendChild(el);
        break;
      default:
        console.log("Invalid attachment mode!");
    }
  }

  //createDOMElement(id, tag, name, attachmentMode, attachmentId = null) {
  createDOMElement(id, attachmentMode, attachmentId = null) {

    this.clarityNode_ = Module.ClarityNode.getCLElementById(id);
    this.id_ = this.clarityNode_.getId();
    this.tag_ = this.clarityNode_.getTag();
    this.name = this.clarityNode_.getName();
    this.boundField_ = this.clarityNode_.getBoundField();
    //console.log(attachmentMode);

    var el = this.createDOMElementByTagType();
    var existingElement = null;

    if (attachmentMode == Module.AttachmentMode.REPLACE_ID
      || attachmentMode == Module.AttachmentMode.ATTACH_ID) {
      existingElement = document.getElementById(attachmentId);
    } else if (attachmentMode == Module.AttachmentMode.REPLACE_NAME
      || attachmentMode == Module.AttachmentMode.ATTACH_NAME) {
      var nameElements = document.getElementsByName(attachmentId);
      if (nameElements.length > 0) {
        existingElement = nameElements[0];
      }
    }

    this.placeElement(existingElement, el, attachmentMode);

    el.id = this.id_

    if (this.name_ != "") {
      el.name = this.name_
      el.setAttribute("name", this.name_);
    }

    this.domElement_ = el    
    this.generateEventHandlers(this);
  }  

  addOptionElementFromString(v, lbl) {
    var opt = document.createElement("option");
    opt.value = v;
    opt.label = lbl;
    this.domElement_.appendChild(opt);
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
    if (outerThis.tag_ in CLElement.eventListenerGenerators) {
      let listenerList = CLElement.eventListenerGenerators[outerThis.tag_];
      listenerList.map(function (kv) {
        outerThis.domElement_.addEventListener(kv.eventName, kv.eventHandlerGenerator(outerThis));
      })
    }
  }  

  addEventListenerById(eventName, id) {
    this.domElement_.addEventListener(eventName, (e) => {
      Module.ClarityNode.runCallbackById(id)
    })
  }
  
  printState() {
    console.log(`CNN: FOR ID: ${this.id}, THIS VAL = ${this.domElement_.value}`);
  }  

  static setGridLocToCurrentVal(ev) {
    let domElement = ev.target;
    let domrect = domElement.getBoundingClientRect();

    let gw = domElement.gw;
    let cw = domElement.cw;
    let gh = domElement.gh;
    let ch = domElement.ch;
    
    var gridDimensions = { w: gw, h: gh };
    let loc = { x: (ev.x - domrect.x), y: (ev.y - domrect.y) };
    var locIn = { x: Math.floor(loc.x / cw), y: Math.floor(loc.y / ch) };    
    var locOut = Util.capCoords(locIn, gridDimensions);
    domElement.gridRef.setValXY(locOut.x, locOut.y);
    console.log(locOut);
  }
}

//let myVar = setInterval(myTimer, 300);
function myTimer() {
  const d = new Date();
  // console.log(d.toLocaleTimeString())
  Module.ClarityNode.runCallbackById("tick")
}

window.CLElement = CLElement

// DEPRECATED BELOW THIS LINE ------------------------------

// printToConsole(v) {
  //   console.log(v)
  // }

  // printType(v) {
  //   console.log(typeof (v));
  // }

// makeOptionsFromList(lst) {
  //   var opts = lst.map(function (x) { return `<option value='${x}'>${x}</option>`; })
  //   console.log(opts);
  // }

  // createCLElement() {
  //   return new CLElement()
  // }

    // multiplyValues(a, b) {
  //   return a * b
  // }

  // invertValue(v) {
  //   return 1 / v
  // }

  // static square(x) {
  //   return x ** 2;
  // }

  // static draw_test_pattern() {
  //   console.log("draw_test_pattern");
  //   let canvasEl = document.getElementById(2);
  //   var canvasCode = CLElement.extraInitCode["canvas"];
  //   canvasCode(canvasEl);
  // }


  // drawBlueTestPattern() {
  //   var domElement = this.domElement_;
  //   // let domElement = this.domElement;
  //   if (domElement.getContext) {
  //     domElement.style.display = 'none';
  //     domElement.style.display = 'block';
  //     let ctx = domElement.getContext('2d');
  //     ctx.scale(2, 2);
  //     ctx.fillStyle = 'blue';
  //     ctx.fillRect(10, 10, 60, 60);
  //     ctx.fillRect(100, 10, 90, 60);

  //     ctx.beginPath();
  //     ctx.arc(250, 40, 32, 0, 2 * Math.PI);
  //     ctx.fill();

  //     ctx.beginPath();
  //     ctx.moveTo(10, 160);
  //     ctx.lineTo(90, 160);
  //     ctx.lineTo(50, 110);
  //     ctx.closePath();
  //     ctx.fill();

  //     ctx.save();
  //     ctx.scale(2, 1);
  //     ctx.beginPath();
  //     ctx.arc(72, 130, 25, 0, 2 * Math.PI);
  //     ctx.fill();
  //     ctx.restore();

  //     ctx.beginPath();
  //     ctx.arc(250, 120, 40, 0, Math.PI);
  //     ctx.fill();
  //     //alert(ctx);
  //     console.log("drawBlueTestPattern()");

  //   }
  // }

  // canvasTestPattern() {
  //   var domElement = this.domElement_;
  //   if (domElement.getContext) {
  //     domElement.style.display = 'none';
  //     domElement.style.display = 'block';
  //     let ctx = domElement.getContext('2d');
  //     let [w, h, hw, hh] = [domElement.width, domElement.height, domElement.width / 2, domElement.height / 2];
  //     let [qw, qh] = [w / 4, h / 4];
  //     ctx.scale(1, 1);
  //     ctx.fillStyle = 'green';
  //     ctx.fillRect(0, 0, w, h);
  //     ctx.fillStyle = 'blue';
  //     ctx.fillRect(qw, qh, hw, hh);
  //     console.log("drawGreenTestPattern()");
  //   }
  // }

  // blackbody(temp) {
  //   var originalColor = { "r": 0, "g": 200, "b": 75 };
  //   return CLElement.blackbody_st(temp, originalColor);
  // }

  
  // passthru(x) {
  //   return x;
  // }