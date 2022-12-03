/**
 * Selectables.js: event listener generators
 */

class Selectables {

  constructor() { }

  static elgInputCheckboxChange(jsProxyNode) {
    return function (e) {
      Module.ClarityNode.updateNodeFromDomById(jsProxyNode.id)
      if (jsProxyNode.domElement.checked) {
        jsProxyNode.nodelog("2 Checkbox is checked..");
      } else {
        jsProxyNode.nodelog("2 Checkbox is not checked..");
      }
    }
  }

  static elgSelectAlltypesChange(jsProxyNode) {
    return function (e) {
      Module.ClarityNode.updateNodeFromDomById(jsProxyNode.id)
      jsProxyNode.nodelog("<SELECT> value changed to: " + e.target.value);
      //console.log("<SELECT> element id: " + jsProxyNode.id + " Value changed to: " + e.target.value);
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

  static listenerGenerators = {
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

  static elgSimpleSelectAlltypesChange(jsProxyNode) {
    return function (e) {
      jsProxyNode.currentSelection = parseInt(e.target.value);
      jsProxyNode.nodelog("elgSimpleSelectAlltypesChange element id: " + jsProxyNode.id + " Value changed to: " + e.target.value);
      Module.ClarityNode.updateNodeFromDomById(jsProxyNode.id)
      let selectedCar = jsProxyNode.clarityNode.getSelectedLabel();
      jsProxyNode.nodelog("Selected car is " + selectedCar);
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