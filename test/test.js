/* eslint-disable no-console */
/* eslint-disable no-unused-vars */
'use strict';

/* global */
const { Foo } = require('../build/Release/tsf_test');
// const { Foo } = require('../build/Debug/tsf_test');

console.log('Create Foo Instance');
let myFoo = new Foo('Foo01');
console.log('Foo Instance Created');
let BarArray = [];

class BarOfInterest {
    constructor(unique_id) {
        this.name = `bar-${unique_id}`;
        this.barInterace = null;
    }

    callbackFunction(eventName, eventData) {
        console.log(`${this.name}: callbackFunction(), eventName = ${eventName}, eventData = ${eventData}`);
    }

    register() {
        this.barInterface = myFoo.registerBar(this.name, this.callbackFunction.bind(this));
        console.log(`Registered ${this.name}`);
    }

    deregister() {
        if (this.barInterface) {
            myFoo.deregisterBar(this.barInterface);
            console.log(`Deregistered ${this.name}`);
            this.barInterface = null;
        }
    }
}

function registerBars() {
    for (let i = 0; i < 10; i++) {
        let bar = new BarOfInterest(i);

        bar.register();
        BarArray.push(bar);
    }
    
    setTimeout(() => {
        deregisterBars();
    }, 1000);
}

function deregisterBars() {
    BarArray.forEach((bar) => {
        bar.deregister();
    });

    BarArray.length = 0;

    setTimeout(() => {
        registerBars();
    }, 1000);
}

registerBars();

setInterval(() => { }, 1 << 30);
