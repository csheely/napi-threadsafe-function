# napi-threadsafe-function
Example code that demonstrates problem I'm having with Napi::ThreadSafeFunction

## The Purpose of the Code
This module is meant to allow Node.js applications to do the following:

- create `Foo` objects (exposed by this module)
- use a `Foo` object to register interest in multiple `Bar` objects and provide a different callback function for each `Bar` object that's registered.
- successful registration returns a `BarInterface` object to the caller they can use to make requests of the object.
- when done with a `Bar` object, the `Foo` object can be used to deregister it and clean up.

There is external activity that will generate events on the `Bar` objects, so the callback function is executed when those events occur.  The events and callbacks will occur in a different thread context.

## Code Classes

`CFoo` (src/foo.cpp) is an object that can be constructed from Javascript.

`CFoo::RegisterBar()` creates a `CBarInterface` (src/bar_interface.cpp) object that's returned to the caller. As part of registration a `CBarActivityReceiver` class (src/bar_activity_receiver.cpp) is also instantiated.

`CBarActivityReceiver` has a `Napi::ThreadSafeFunction` object to store the callback function.  This class receives external activity and executes the callback function whenever that activity warrants it.

## The Problem
De-registering `BarInterface` objects appears to work fine on Windows platforms (or at least works for a very, very long time without problems).  On Linux (testing on Ubuntu 18.04 and derivatives), the code is much less stable, and appears to be related to destroying the `Napi::ThreadSafeFunction` instance.

Deregistering one or two `BarInterface` objects will work for a short time on Linux and then cause a core dump.  Deregistering a larger amount (10 obects one after the other) appears to always crash.
