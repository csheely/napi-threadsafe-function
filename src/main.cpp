#include <napi.h>
#include "foo.h"
#include "bar_interface.h"

// Main library function to specify all functions and classes in the library
// available to Javascript.
Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
    CBarInterface::Init(env, exports);
    return CFoo::Init(env, exports);
}

NODE_API_MODULE(tsf_test, InitAll)
