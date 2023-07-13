#ifndef JSC_UTIL_HPP
#define JSC_UTIL_HPP
#include <JavaScriptCore/JavaScript.h>
#include <string>
#include <map>
#include "jsc_util.cpp"
namespace jscUtil {
    std::string jsonAsStr(JSContextRef ctx, JSValueRef value);
    std::string toString(JSContextRef ctx, JSValueRef value);
    int getArrayLength(JSContextRef ctx, JSObjectRef array);
    std::string getColor(std::string color);
    void setError(JSValueRef *exception, std::string message);
}
#endif