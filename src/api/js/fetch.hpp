#ifndef FETCH_H
#define FETCH_H

#include <JavaScriptCore/JavaScript.h>
#include <string>

JSValueRef fetchc(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

#endif
