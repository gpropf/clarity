/**
 * Selectables.js: event listener generators and 
 */

class Selectables {

  constructor() { }

  static foo() {
    console.log("Selectables.foo");
  }

  static elgInputCheckboxChange(element) {
    return function (e) {
      Module.ClarityNode.updateNodeFromDomById(element.id)
      if (element.domElement.checked) {
        console.log("2 Checkbox is checked..");
      } else {
        console.log("2 Checkbox is not checked..");
      }
    }
  }

  static elgSelectAlltypesChange (element) {
    return function (e) {
      Module.ClarityNode.updateNodeFromDomById(element.id)      
      console.log("<SELECT> element id: " + element.id + " Value changed to: " + e.target.value);      
    }
  }

  static expListenerGens = {
    "select": {
      "NOTYPE": {
        "change": this.elgSelectAlltypesChange
      }
    },
    "input": {
      "checkbox": {
        "change": this.elgInputCheckboxChange
      }
    }
  }

}

window.Selectables = Selectables