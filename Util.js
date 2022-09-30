
/**
 * Mostly a place for little utility functions to live. I made it a class so I could use it to test
 * instantiating things with Emscripten. Most methods should be static.
 */
class Util {

    constructor() { }

    utiltest(x) {
        console.log("x is:" + x);
    }

    static make_trs() {
        console.log("Making the fields!");
        Module.Util.make_trs();
    }

    /**
     * Takes a vector location {x,y} and a grid {w,h} and 'clamps' the location to within that grid so
     * no negative values are allowed and x and y must be strictly less than w and h respectively.
     * 
     * @param {2d vector} loc 
     * @param {2d vector} grid 
     * @returns Clamped location vector
     */
    static capCoords(loc, grid) {
        var outx = loc.x;
        var outy = loc.y;
        if (loc.x < 0) outx = 0;
        if (loc.y < 0) outy = 0;
        if (loc.x >= grid.w) { outx = grid.w - 1 };
        if (loc.y >= grid.h) { outy = grid.h - 1 };
        return { x: outx, y: outy };
    }

    /**
     * Takes a vector location {x,y} and a grid {w,h} and 'wraps' the location to within that grid so
     * using modular arithmetic. This is the "Asteroids" algorithm.
     * 
     * @param {2d vector} loc 
     * @param {2d vector} grid 
     * @returns Clamped location vector
     */
    static wrapCoords(loc, grid) {
        loc.x = loc.x % grid.w;
        loc.y = loc.y % grid.h;
        return loc;
    }

    /**
     * Gets the absolute screen location of the event and uses the bounding box of the
     * DOM element to work out the location relative to the upper left corner of the box.
     * 
     * 
     * @param {Dom Event} ev 
     * @returns Location of the event relative to the upper-left corner of the element it occurred im.
     */
    static locateEvent(ev) {
        let domElement = ev.target;
        let domrect = domElement.getBoundingClientRect();
        let loc = { x: (ev.x - domrect.x), y: (ev.y - domrect.y) };
        return loc;
    }
}


window.Util = Util