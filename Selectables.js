

class Selectables {

  constructor() { }

  static foo() {
    console.log("Selectables.foo");
  }

  static eventListenerGenerators = {
    "select": {
      "change": function (element) {
        return function (e) {
          Module.ClarityNode.updateNodeFromDomById(element.id)
          if (element.boundField != undefined)
            console.log("<SELECT> element id: " + element.id + " Value changed to: " + element.domElement[element.boundField]);
          else
            console.log("<SELECT> element id: " + element.id + " Value changed");
        }
      }
    },
    "input": {
      "checkbox": {
        "change": function (element) {
          return function (e) {
            //Module.ClarityNode.updateNodeFromDomById(element.id)
            if (element.domElement.checked) {
              console.log("Checkbox is checked..");
            } else {
              console.log("Checkbox is not checked..");
            }
          }
        }
      }
    }
  }

  //static getEventListenerGenerators() { return eventListenerGenerators; }

  static installEventListeners2(cle, elementTag, elementType) {
    var eventListenerGenerators = CLElement.getEventListenerGenerators(Selectables.eventListenerGenerators, elementTag, elementType);
    CLElement.installEventListeners2(cle, eventListenerGenerators);
  }

}

window.Selectables = Selectables