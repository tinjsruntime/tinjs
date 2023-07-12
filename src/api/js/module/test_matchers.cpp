#include <JavaScriptCore/JavaScript.h>
#include <string>
#include "test_matchers.hpp"
#include "../../cpp/jsc_util.hpp"

using namespace jscUtil;

namespace testMatchers
{
    JSValueRef toEqualCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                               const JSValueRef arguments[], JSValueRef *exception)
    {
        auto _meta = JSStringCreateWithUTF8CString("_meta");
        auto meta = JSValueToObject(ctx, JSObjectGetProperty(ctx, thisObject, _meta, nullptr), nullptr);
        JSStringRelease(_meta);

        auto error = JSStringCreateWithUTF8CString("error");
        auto errorValue = JSObjectGetProperty(ctx, meta, error, nullptr);
        JSStringRelease(error);

        if (JSValueToBoolean(ctx, errorValue))
        {
            return JSValueMakeUndefined(ctx);
        }

        if (argumentCount != 1)
        {
            auto __error = JSStringCreateWithUTF8CString("toEqual requires 1 argument");
            *exception = JSValueMakeString(ctx, __error);
            JSStringRelease(__error);
            return JSValueMakeUndefined(ctx);
        }

        auto arg = arguments[0];
        auto value = JSObjectGetProperty(ctx, thisObject, JSStringCreateWithUTF8CString("_value"), nullptr);

        bool ret = JSValueIsEqual(ctx, arg, value, nullptr);

        if (!ret)
        {
            auto nmeta = JSObjectMake(ctx, NULL, NULL);
            auto error = JSStringCreateWithUTF8CString("error");
            auto message = JSStringCreateWithUTF8CString("message");
            JSObjectSetProperty(ctx, nmeta, error, JSValueMakeBoolean(ctx, true), kJSPropertyAttributeNone, nullptr);

            auto againstr = toString(ctx, arguments[0]);
            auto valuestr = toString(ctx, value);

            std::string out = getColor("red") + getColor("bold") + "Expected: " + getColor("reset") +
                              getColor("red") + againstr + "\n" +
                              getColor("yellow") + getColor("bold") + "Received: " + getColor("reset") + getColor("yellow") +
                              valuestr + getColor("reset");

            JSObjectSetProperty(ctx, nmeta, message, JSValueMakeString(ctx, JSStringCreateWithUTF8CString(out.c_str())),
                                kJSPropertyAttributeNone, nullptr);

            JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), JSStringCreateWithUTF8CString("_meta"), nmeta,
                                kJSPropertyAttributeNone, nullptr);
        }

        return JSValueMakeUndefined(ctx);
    }
}