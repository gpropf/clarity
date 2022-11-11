/**
 * Selectables.js: event listener generators and 
 */

class Selectables {

  constructor() { }

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

  static elgSelectAlltypesChange(element) {
    return function (e) {
      Module.ClarityNode.updateNodeFromDomById(element.id)
      console.log("<SELECT> element id: " + element.id + " Value changed to: " + e.target.value);
    }
  }

  // static listenerGenerators = {
  //   "select": {
  //     "NOTYPE": {
  //       "change": this.elgSelectAlltypesChange
  //     }
  //   },
  //   "input": {
  //     "checkbox": {
  //       "change": this.elgInputCheckboxChange
  //     }
  //   }
  // }

  static listenerGenerators2 = {
    "Select": {
      "select": {
        "NOTYPE": {
          "change": this.elgSelectAlltypesChange
        }
      }
    },
    "Checkbox": {
      "input": {
        "checkbox": {
          "change": this.elgInputCheckboxChange
        }
      }
    },
    "SimpleSelect":
    {
      "select": {
        "NOTYPE": {
          "change": this.elgSimpleSelectAlltypesChange
        }
      }
    }
  }

  static elgSimpleSelectAlltypesChange(element) {
    return function (e) {
      element.currentSelection = parseInt(e.target.value);
      console.log("elgSimpleSelectAlltypesChange element id: " + element.id + " Value changed to: " + e.target.value);
      Module.ClarityNode.updateNodeFromDomById(element.id)
      let selectedCar = element.clarityNode.getSelectedLabel();
      console.log("Selected car is " + selectedCar);
    }
  }

  // static simpleSelectGens = {
  //   "select": {
  //     "NOTYPE": {
  //       "change": this.elgSimpleSelectAlltypesChange
  //     }
  //   }
  // }


}

window.Selectables = Selectables