

function countElementsELG(avstring) {
    return function (ev) {
        console.log("Count Elements  Clicked!");
        console.log(avstring);
        avstring.countElements();
    }
}

function deleteFirst(avstring, iterator) {
    return function (ev) {
        console.log(avstring);
        console.log(iterator);
        console.log("deleteFirst Clicked!");
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