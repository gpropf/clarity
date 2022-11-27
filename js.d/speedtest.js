

function elgDestroyFields(speedtester) {
    return function (e) {
        console.log("Simplified EL style...");
        console.log(speedtester);
        speedtester.destroyEverything();
    }
}

function elgMakeFields(speedtester) {
    return function (e) {
        console.log("Simplified EL style...");
        console.log(speedtester);
        speedtester.makeTrsNoGraph();
    }
}

function doNothingEL(ev) {
    console.log("CLICK: " + ev);
}

class Speedtester {

    staticSpeedtester = null;

    makeFields() {
        console.log(this);
    }

    static makeFields(tester) {

    }











}