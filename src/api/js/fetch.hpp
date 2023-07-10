#ifndef FETCH_HPP
#define FETCH_HPP

#include <JavaScriptCore/JavaScript.h>
#include <string>

namespace fetch {
    JSValueRef fetch(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
}

#endif
