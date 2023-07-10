#ifndef GLOBAL_H
#define GLOBAL_H

#include <JavaScriptCore/JavaScript.h>
#include <string>

namespace global {
    void define(JSContextRef context, JSObjectRef *globalObject, std::string dirname, std::string fullname, JSValueRef *exception, JSObjectRef *exportsObject, std::string mode);
}

#endif
