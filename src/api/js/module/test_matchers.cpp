#include <JavaScriptCore/JavaScript.h>
#include <string>
#include "test_matchers.hpp"
#include "../../cpp/jsc_util.hpp"

using namespace jscUtil;

namespace testMatchers
{
    std::string redExpect(std::string againstr) {
        return getColor("red") + "✗ " + againstr + getColor("reset");
    }

    std::string yellowReceived(std::string valuestr) {
        return getColor("yellow") + " > " + valuestr + getColor("reset");
    }

    JSObjectRef getMeta(JSContextRef ctx, JSObjectRef obj) {
        auto _meta = JSStringCreateWithUTF8CString("_meta");
        auto meta = JSValueToObject(ctx, JSObjectGetProperty(ctx, obj, _meta, nullptr), nullptr);
        JSStringRelease(_meta);
        return meta;
    }

    bool hasErrored(JSContextRef ctx, JSObjectRef meta) {
        auto errorString = JSStringCreateWithUTF8CString("error");
        auto errorValue = JSObjectGetProperty(ctx, meta, errorString, nullptr);
        JSStringRelease(errorString);
        return JSValueToBoolean(ctx, errorValue);
    }

    JSValueRef toEqualCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                               const JSValueRef arguments[], JSValueRef *exception)
    {
        auto meta = getMeta(ctx, thisObject);

        if (hasErrored(ctx, meta)) {
            return JSValueMakeUndefined(ctx);
        }

        if (argumentCount != 1)  {
            setError(exception, "toEqual requires 1 argument");
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

            auto out = redExpect(againstr) + yellowReceived(valuestr);

            JSObjectSetProperty(ctx, nmeta, message, JSValueMakeString(ctx, JSStringCreateWithUTF8CString(out.c_str())),
                                kJSPropertyAttributeNone, nullptr);

            JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), JSStringCreateWithUTF8CString("_meta"), nmeta,
                                kJSPropertyAttributeNone, nullptr);
        }

        return JSValueMakeUndefined(ctx);
    }
}