#include <JavaScriptCore/JavaScript.h>
#include <string>

namespace ArrayProto {
    JSValueRef random(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                      const JSValueRef arguments[], JSValueRef *exception);

    JSValueRef first(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                     const JSValueRef arguments[], JSValueRef *exception);

    JSValueRef last(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                    const JSValueRef arguments[], JSValueRef *exception);
}