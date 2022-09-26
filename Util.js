

class Util {

    constructor() { }

    static locateEvent(ev) {
        alert(ev.x);
    }

    utiltest(x) {
        console.log("x is:" + x);
    }


    static make_trs() {
        console.log("Making the fields!");
        Module.Util.make_trs();
    }

}


window.Util = Util