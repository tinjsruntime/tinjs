#include "_js_Array.hpp"
#include <iostream>
#include <vector>
#include <JavaScriptCore/JavaScript.h>
#include "../../cpp/jsc_util.hpp"

namespace ArrayProto
{
    JSValueRef random(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
    {
        int length = getArrayLength(ctx, thisObject);
        int random = rand() % length;
        JSValueRef randomValue = JSObjectGetPropertyAtIndex(ctx, thisObject, random, exception);
        return randomValue;
    }

    JSValueRef first(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
    {
        JSValueRef first = JSObjectGetPropertyAtIndex(ctx, thisObject, 0, exception);
        return first;
    }

    JSValueRef last(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
    {
        return JSObjectGetPropertyAtIndex(ctx, thisObject, getArrayLength(ctx, thisObject) - 1, exception);
    }
}