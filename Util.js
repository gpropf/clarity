
/**
 * Mostly a place for little utility functions to live. I made it a class so I could use it to test
 * instantiating things with Emscripten. Most or all methods with be static.
 */
class Util {

    constructor() { }

    utiltest(x) {
        console.log("utiltest value:" + x);
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

    /**
     * This is meant to be used as a quick way to create JS functions that can be called from setTimeout()
     * or setInterval() or used as event listeners for buttons or other fire-and-forget situations. For
     * genuine JavaScript objects you can, of course, just use the 'apply' or 'call' functions but the 
     * idea here is that the "obj" parameter actaully represents a C++ object that has an embinding that
     * allows it to be passed to JS functions. The event object is just thrown away if you set
     * "isEventListener" to true but the point of having that flag is simply to produce something that
     * has the correct form for the application.
     * 
     * 
     * @param {bound C++ object} obj The actual bound C++ object that we want to call the method on.
     * @param {string} objMethod A string that names a method defined on the object.
     * @returns a function that calls the named method on the object
     */
    static callMethodByName(obj, objMethod, isEventListener = false) {
        let fnStr = `obj.${objMethod}()`;
        //console.log(fnStr);
        if (isEventListener) {
            return function (ev) {
                eval(fnStr);
            }
        }
        return function () {
            eval(fnStr);
        }
    }

    /**
     * This is one of my very imperfect solutions to the problem of how to handle timers. 
     * It would be nice to just pass a C++ lambda function that encapsulated whatever you wanted to iterate. This 
     * causes problems with Emscripten so we offer this. The idea is that the `obj` is an Emscripten bound
     * C++ object with a method called "tick".
     * 
     * 
     * @param {C++ object} obj 
     * @param {int} intervalMillis 
     * @returns int
     */
    static setIntervalForObjectWithTickMethod(obj, intervalMillis) {
        //return setTimeout(obj.tick, intervalMillis);
        return setInterval(() => { obj.tick() }, intervalMillis);
    }

    /**
     * Same as above method but allows you to specify the method name. Avoids the use of `eval`
     * so the method should be reasonably secure and fast.
     * 
     * @param {C++ object} obj 
     * @param {string} objMethodName 
     * @param {int} intervalMillis 
     * @returns int     
     */
    static setIntervalForObjectWithNamedMethod(obj, objMethodName, intervalMillis) {        
        var fn = obj[objMethodName];
        if (typeof fn === "function") {
            return setInterval(() => { fn.apply(obj) }, intervalMillis);
        }
    }
}

window.Util = Util