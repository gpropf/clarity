

function countElementsELG(avstring, lastSize) {
    return function (ev) {
        console.log("Count Elements  Clicked!");
        console.log(avstring);
        avstring.countElements(lastSize);
    }
}

function deleteLast(avstring, iterator) {
    return function (ev) {
        console.log(avstring);
        console.log(iterator);
        console.log("deleteLast Clicked!");
        avstring.erase(iterator);
    }
}

function eraseNth(avstring, n) {
    return function (ev) {
        console.log("eraseNth Clicked!");
        console.log(avstring);
        console.log(n);
        
        avstring.eraseNth(n);
    }
}