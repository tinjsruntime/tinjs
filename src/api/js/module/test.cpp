#include "test.hpp"
#include <JavaScriptCore/JavaScript.h>
#include <iostream>
#include <map>
#include <string>
#include "../../cpp/jsc_util.hpp"
#include "test_matchers.cpp"

using namespace jscUtil;

namespace testMod
{
    JSValueRef describeCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                                const JSValueRef arguments[], JSValueRef *exception)
    {
        if (argumentCount != 2)
        {
            auto error = JSStringCreateWithUTF8CString("describe requires 2 arguments");
            *exception = JSValueMakeString(ctx, error);
            JSStringRelease(error);
            return JSValueMakeUndefined(ctx);
        }

        if (!JSValueIsString(ctx, arguments[0]))
        {
            auto error = JSStringCreateWithUTF8CString("describe requires a string argument");
            *exception = JSValueMakeString(ctx, error);
            JSStringRelease(error);
            return JSValueMakeUndefined(ctx);
        }

        if (!JSValueIsObject(ctx, arguments[1]))
        {
            auto error = JSStringCreateWithUTF8CString("describe requires a function argument");
            *exception = JSValueMakeString(ctx, error);
            JSStringRelease(error);
            return JSValueMakeUndefined(ctx);
        }

        auto func = JSValueToObject(ctx, arguments[1], exception);

        auto group = JSStringCreateWithUTF8CString("_group");
        JSObjectSetProperty(ctx, thisObject, group, arguments[0], kJSPropertyAttributeNone, exception);
        JSStringRelease(group);

        JSObjectCallAsFunction(ctx, func, thisObject, 0, NULL, exception);

        return JSValueMakeUndefined(ctx);
    }

    JSValueRef testCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                            const JSValueRef arguments[], JSValueRef *exception)
    {
        auto group = JSStringCreateWithUTF8CString("_group");
        auto groupValue = JSObjectGetProperty(ctx, thisObject, group, exception);
        JSStringRelease(group);

        auto grpstr = toString(ctx, groupValue);

        if (argumentCount != 2)
        {
            auto error = JSStringCreateWithUTF8CString("describe requires 2 arguments");
            *exception = JSValueMakeString(ctx, error);
            JSStringRelease(error);
            return JSValueMakeUndefined(ctx);
        }

        if (!JSValueIsString(ctx, arguments[0]))
        {
            auto error = JSStringCreateWithUTF8CString("describe requires a string argument");
            *exception = JSValueMakeString(ctx, error);
            JSStringRelease(error);
            return JSValueMakeUndefined(ctx);
        }

        if (!JSValueIsObject(ctx, arguments[1]))
        {
            auto error = JSStringCreateWithUTF8CString("describe requires a function argument");
            *exception = JSValueMakeString(ctx, error);
            JSStringRelease(error);
            return JSValueMakeUndefined(ctx);
        }

        auto func = JSValueToObject(ctx, arguments[1], exception);

        auto meta = JSObjectMake(ctx, NULL, NULL);
        auto error = JSStringCreateWithUTF8CString("error");
        auto message = JSStringCreateWithUTF8CString("message");
        JSObjectSetProperty(ctx, meta, error, JSValueMakeBoolean(ctx, false), kJSPropertyAttributeNone, exception);
        JSObjectSetProperty(ctx, meta, message, JSValueMakeString(ctx, JSStringCreateWithUTF8CString("")),
                            kJSPropertyAttributeNone, exception);

        auto _meta = JSStringCreateWithUTF8CString("_meta");
        JSObjectSetProperty(ctx, thisObject, _meta, meta, kJSPropertyAttributeNone, exception);

        JSValueRef nException = nullptr;
        JSObjectCallAsFunction(ctx, func, thisObject, 0, NULL, &nException);

        auto newMeta = JSValueToObject(ctx, JSObjectGetProperty(ctx, thisObject, _meta, exception), exception);
        JSStringRelease(_meta);

        auto errorString = JSStringCreateWithUTF8CString("error");
        auto errorValue = JSObjectGetProperty(ctx, newMeta, errorString, exception);
        JSStringRelease(errorString);

        auto againstr = toString(ctx, arguments[0]);

        if (JSValueToBoolean(ctx, errorValue) || nException != nullptr)
        {
            if (nException != nullptr)
            {
                auto exceptionString = JSValueToStringCopy(ctx, nException, nullptr);

                if (exceptionString != nullptr)
                {
                    std::cout << getColor("red") << "⚠ " << grpstr << getColor("reset") << " > " << getColor("red") << againstr
                      << getColor("reset") << std::endl;

                    std::cout << toString(ctx, nException) << std::endl;
                      JSStringRelease(exceptionString);
                }

                return JSValueMakeUndefined(ctx);
            }
            auto message_String = JSStringCreateWithUTF8CString("message");
            auto messageValue = JSObjectGetProperty(ctx, newMeta, message_String, exception);
            JSStringRelease(message_String);

            auto messageString = toString(ctx, messageValue);

            std::cout << getColor("red") << "✗ " << grpstr << getColor("reset") << " > " << getColor("red") << againstr
                      << getColor("reset") << std::endl;
            std::cout << messageString << std::endl;
            return JSValueMakeUndefined(ctx);
        }

        std::cout << getColor("green") << "✓ " << grpstr << getColor("reset") << " > " << getColor("green") << againstr
                  << getColor("reset") << std::endl;
        return JSValueMakeUndefined(ctx);
    }

    JSValueRef expectCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                              const JSValueRef arguments[], JSValueRef *exception)
    {
        if (argumentCount != 1)
        {
            auto error = JSStringCreateWithUTF8CString("expect requires 1 argument");
            *exception = JSValueMakeString(ctx, error);
            JSStringRelease(error);
            return JSValueMakeUndefined(ctx);
        }

        auto arg = arguments[0];

        auto obj = JSObjectMake(ctx, NULL, NULL);

        auto value = JSStringCreateWithUTF8CString("_value");
        JSObjectSetProperty(ctx, obj, value, arg, kJSPropertyAttributeNone, NULL);

        auto toEqual = JSStringCreateWithUTF8CString("toEqual");
        JSObjectSetProperty(ctx, obj, toEqual, JSObjectMakeFunctionWithCallback(ctx, toEqual, testMatchers::toEqualCallback),
                            kJSPropertyAttributeNone, NULL);

        return obj;
    }

    JSObjectRef init_test_module(JSContextRef context, JSObjectRef *exports)
    {
        JSObjectSetProperty(context, *exports, JSStringCreateWithUTF8CString("describe"),
                            JSObjectMakeFunctionWithCallback(context, JSStringCreateWithUTF8CString("describe"),
                                                             describeCallback),
                            kJSPropertyAttributeNone, NULL);
        JSObjectSetProperty(context, *exports, JSStringCreateWithUTF8CString("test"),
                            JSObjectMakeFunctionWithCallback(context, JSStringCreateWithUTF8CString("test"),
                                                             testCallback),
                            kJSPropertyAttributeNone, NULL);
        JSObjectSetProperty(context, *exports, JSStringCreateWithUTF8CString("expect"),
                            JSObjectMakeFunctionWithCallback(context, JSStringCreateWithUTF8CString("expect"),
                                                             expectCallback),
                            kJSPropertyAttributeNone, NULL);
        return *exports;
    }
}