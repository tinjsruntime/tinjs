#include "jsc_util.hpp"
#include <JavaScriptCore/JavaScript.h>
#include <map>

namespace jscUtil {
                std::map<std::string, std::string> colors = {
            {"reset",      "\033[0m"},
            {"red",        "\033[31m"},
            {"green",      "\033[32m"},
            {"yellow",     "\033[33m"},
            {"blue",       "\033[34m"},
            {"magenta",    "\033[35m"},
            {"cyan",       "\033[36m"},
            {"white",      "\033[37m"},
            {"bright",     "\033[1m"},
            {"dim",        "\033[2m"},
            {"underscore", "\033[4m"},
            {"blink",      "\033[5m"},
            {"reverse",    "\033[7m"},
            {"hidden",     "\033[8m"},
            {"bold",       "\033[1m"},
            {"gray",       "\033[90m"}
    };

    std::string toString(JSContextRef ctx, JSValueRef value) {
        auto string = JSValueToStringCopy(ctx, value, nullptr);
        auto length = JSStringGetMaximumUTF8CStringSize(string);
        char buffer[length];
        JSStringGetUTF8CString(string, buffer, length);
        JSStringRelease(string);
        return std::string(buffer);
    }

    std::string jsonAsStr (JSContextRef ctx, JSValueRef value) {
        auto string = JSValueCreateJSONString(ctx, value, 0, nullptr);
        auto length = JSStringGetMaximumUTF8CStringSize(string);
        char buffer[length];
        JSStringGetUTF8CString(string, buffer, length);
        JSStringRelease(string);
        return std::string(buffer);
    }
    
    std::string getColor(std::string color) {
        return colors[color];
    }
    
    int getArrayLength(JSContextRef ctx, JSObjectRef array) {
        auto count = JSObjectGetProperty(ctx, array, JSStringCreateWithUTF8CString("length"), nullptr);
        int length = JSValueToNumber(ctx, count, nullptr);
        return length;
    }

    void setError(JSValueRef *exception, std::string message) {
        auto error = JSStringCreateWithUTF8CString(message.c_str());
        *exception = JSValueMakeString(JSContextGetGlobalContext(nullptr), error);
        JSStringRelease(error);
    }
}