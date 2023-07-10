#ifndef REQUIRE_H
#define REQUIRE_H

#include <JavaScriptCore/JavaScript.h>

JSValueRef requireCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);

#endif
