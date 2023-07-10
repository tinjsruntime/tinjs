#include <JavaScriptCore/JavaScript.h>

namespace require {
    JSValueRef requireCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                               const JSValueRef arguments[], JSValueRef *exception);
}
