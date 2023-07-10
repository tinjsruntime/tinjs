#include "jsc_util.hpp"

std::string toString(JSContextRef ctx, JSValueRef value)
{
    JSStringRef string = JSValueToStringCopy(ctx, value, nullptr);
    size_t length = JSStringGetMaximumUTF8CStringSize(string);
    char buffer[length];
    JSStringGetUTF8CString(string, buffer, length);
    JSStringRelease(string);
    return std::string(buffer);
}