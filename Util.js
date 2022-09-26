

class Util {

    constructor() { }

    static lastClick = { x: 0, y: 0 };
    static cellDimensions = { w: 1, h: 1 };

    static locateEvent(ev) {
        let domrect = ev.target.getBoundingClientRect();
        let x = ev.x - domrect.x;
        let y = ev.y - domrect.y;
        Util.lastClick.x = Math.floor(x / Util.cellDimensions.w);
        Util.lastClick.y = Math.floor(y / Util.cellDimensions.h);
        console.log(Util.lastClick)
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