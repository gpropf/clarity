

class Util {

    constructor() { }


    utiltest(x) {
        console.log("x is:" + x);
    }


    static make_trs() {
        console.log("Making the fields!");
        Module.Util.make_trs();
    }

    static capCoords(loc, grid) {
        var outx = loc.x;
        var outy = loc.y;
        if (loc.x < 0) outx = 0;
        if (loc.y < 0) outy = 0;
         if (loc.x >= grid.w) { outx = grid.w - 1 };
         if (loc.y >= grid.h) { outy = grid.h - 1 };
        return { x: outx, y: outy };
    }

    static wrapCoords(loc, grid) {
        loc.x = loc.x % grid.w;
        loc.y = loc.y % grid.h;
        return loc;
    }

    static locateEvent(ev) {
        let domElement = ev.target;
        let domrect = domElement.getBoundingClientRect();
        let loc = { x: (ev.x - domrect.x), y: (ev.y - domrect.y) };
        return loc;
    }

}


window.Util = Util