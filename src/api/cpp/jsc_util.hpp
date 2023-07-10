#ifndef JSC_UTIL_HPP
#define JSC_UTIL_HPP

#include <JavaScriptCore/JavaScript.h>
#include <string>

std::string toString(JSContextRef ctx, JSValueRef value);
int getArrayLength(JSContextRef ctx, JSObjectRef array);

#endif