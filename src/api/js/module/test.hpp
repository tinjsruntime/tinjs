#ifndef TEST_MODULE_HPP
#define TEST_MODULE_HPP

#include <JavaScriptCore/JavaScript.h>

namespace testMod {
    JSObjectRef init_test_module(JSContextRef context, JSObjectRef *exports);
}

#endif