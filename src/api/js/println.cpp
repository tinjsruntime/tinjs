#include <iostream>
#include <map>
#include <JavaScriptCore/JavaScript.h>
#include "println.hpp"
#include "../cpp/jsc_util.cpp"

std::map<std::string, std::string> _colors = {
    {"reset", "\033[0m"},
    {"red", "\033[31m"},
    {"green", "\033[32m"},
    {"yellow", "\033[33m"},
    {"blue", "\033[34m"},
    {"magenta", "\033[35m"},
    {"cyan", "\033[36m"},
    {"white", "\033[37m"},
    {"bright", "\033[1m"},
    {"dim", "\033[2m"},
    {"underscore", "\033[4m"},
    {"blink", "\033[5m"},
    {"reverse", "\033[7m"},
    {"hidden", "\033[8m"},
    {"bold", "\033[1m"},
    {"gray", "\033[90m"}
};

void printValue(JSContextRef ctx, JSValueRef value, bool color)
{
    auto str = toString(ctx, value);

    std::cout << str;
}
 
void printArray(JSContextRef ctx, JSObjectRef array, bool color)
{

    JSPropertyNameArrayRef propertyArray = JSObjectCopyPropertyNames(ctx, array);
    size_t count = JSPropertyNameArrayGetCount(propertyArray);

    if (count == 0) {
        std::cout << "[]";
        return;
    }

    std::cout << "[";

    for (size_t i = 0; i < count; i++) {
        JSValueRef element = JSObjectGetPropertyAtIndex(ctx, array, i, nullptr);
        rawPrint(ctx, element, JSValueGetType(ctx, element), true);

        if (i != count - 1) {
            std::cout << ", ";
        }
    }

    std::cout << "]";
}

void printObject(JSContextRef ctx, JSObjectRef object, bool color)
{

    JSPropertyNameArrayRef propertyArray = JSObjectCopyPropertyNames(ctx, object);
    size_t count = JSPropertyNameArrayGetCount(propertyArray);

    if (count == 0) {
        std::cout << "{}";
        return;
    }

    std::cout << "{ ";

    for (size_t i = 0; i < count; i++) {
        JSStringRef propertyName = JSPropertyNameArrayGetNameAtIndex(propertyArray, i);
        JSValueRef propertyValue = JSObjectGetProperty(ctx, object, propertyName, nullptr);

        auto str = toString(ctx, JSValueMakeString(ctx, propertyName));

        std::cout << str << ": ";
        
        rawPrint(ctx, propertyValue, JSValueGetType(ctx, propertyValue), true);

        if (i != count - 1) {
            std::cout << ", ";
        }

        JSStringRelease(propertyName);
    }

    JSPropertyNameArrayRelease(propertyArray);

    std::cout << " }";
}

JSValueRef printlnCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    for (size_t i = 0; i < argumentCount; i++) {
        JSValueRef argument = arguments[i];
        JSType type = JSValueGetType(ctx, argument);

        rawPrint(ctx, argument, type, false);
        std::cout << " ";
    }

    std::cout << std::endl;

    return JSValueMakeUndefined(ctx);
}

void rawPrint(JSContextRef ctx, JSValueRef argument, JSType type, bool colorString) {
        switch (type) {
            case kJSTypeUndefined:
                std::cout << _colors["gray"] << "undefined" << _colors["reset"];
                break;

            case kJSTypeNull:
                std::cout << _colors["gray"] << "null" << _colors["gray"];
                break;

            case kJSTypeBoolean:
                std::cout << _colors["yellow"] << (JSValueToBoolean(ctx, argument) ? "true" : "false") << _colors["reset"];
                break;

            case kJSTypeNumber:
                std::cout << _colors["yellow"] << JSValueToNumber(ctx, argument, nullptr) << _colors["reset"];
                break;

            case kJSTypeString:
                if (colorString == true) {
                    std::cout << _colors["green"] << '"' << toString(ctx, argument) << '"' << _colors["reset"];
                } else {
                    printValue(ctx, argument, true);
                }
                break;

            case kJSTypeObject:
                if (JSValueIsArray(ctx, argument)) {
                    printArray(ctx, JSValueToObject(ctx, argument, nullptr), true);
                } else if (JSObjectIsFunction(ctx, JSValueToObject(ctx, argument, nullptr))) {
                    auto str = toString(ctx, argument);
                    
                    if (str.find("class") != std::string::npos) {
                        std::string className = str.substr(str.find("class ") + 6, str.find("{") - str.find("class ") - 6);
                        std::string trimmedClassName = className.substr(0, className.find(" "));
                        bool isEmpty = trimmedClassName == "";
                        std::string finalClass = "[Class" + (isEmpty ? "]" : " " + trimmedClassName + "]");
                        std::cout << _colors["cyan"] << finalClass << _colors["reset"];
                    } else {
                        std::cout << _colors["magenta"] << "[Function]" << _colors["reset"];
                    }
                } else {
                     printObject(ctx, JSValueToObject(ctx, argument, nullptr), true);
                }
                break;

            default:
                printValue(ctx, argument, false);
                break;
        }
}