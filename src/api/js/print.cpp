#include <iostream>
#include <map>
#include <JavaScriptCore/JavaScript.h>
#include "print.hpp"
#include "../cpp/jsc_util.cpp"

using namespace jscUtil;
namespace print
{
    void printValue(JSContextRef ctx, JSValueRef value, bool color)
    {
        auto str = toString(ctx, value);

        std::cout << str;
    }

    void printArray(JSContextRef ctx, JSObjectRef array, bool color)
    {
        int length = getArrayLength(ctx, array);

        if (length == 0)
        {
            std::cout << "[]";
            return;
        }

        std::cout << "[";

        for (size_t i = 0; i < length; i++)
        {
            auto element = JSObjectGetPropertyAtIndex(ctx, array, i, nullptr);
            rawPrint(ctx, element, JSValueGetType(ctx, element), true);

            if (i != length - 1)
            {
                std::cout << ", ";
            }
        }

        std::cout << "]";
    }

    void printObject(JSContextRef ctx, JSObjectRef object, bool color)
    {

        auto propertyArray = JSObjectCopyPropertyNames(ctx, object);
        size_t count = JSPropertyNameArrayGetCount(propertyArray);

        if (count == 0)
        {
            std::cout << "{}";
            return;
        }

        std::cout << "{ ";

        for (size_t i = 0; i < count; i++)
        {
            auto propertyName = JSPropertyNameArrayGetNameAtIndex(propertyArray, i);
            auto propertyValue = JSObjectGetProperty(ctx, object, propertyName, nullptr);

            auto str = toString(ctx, JSValueMakeString(ctx, propertyName));

            std::cout << str << ": ";

            rawPrint(ctx, propertyValue, JSValueGetType(ctx, propertyValue), true);

            if (i != count - 1)
            {
                std::cout << ", ";
            }

            JSStringRelease(propertyName);
        }

        JSPropertyNameArrayRelease(propertyArray);

        std::cout << " }";
    }

    JSValueRef printCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                               const JSValueRef arguments[], JSValueRef *exception)
    {
        for (size_t i = 0; i < argumentCount; i++)
        {
            auto argument = arguments[i];
            auto type = JSValueGetType(ctx, argument);

            rawPrint(ctx, argument, type, false);
            std::cout << " ";
        }

        return JSValueMakeUndefined(ctx);
    }

    JSValueRef printlnCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                               const JSValueRef arguments[], JSValueRef *exception)
    {
        auto ret = printCallback(ctx, function, thisObject, argumentCount, arguments, exception);

        std::cout << std::endl;

        return ret;
    }

    void rawPrint(JSContextRef ctx, JSValueRef argument, JSType type, bool colorString)
    {
        switch (type)
        {
        case kJSTypeUndefined:
            std::cout << colors["gray"] << "undefined" << colors["reset"];
            break;

        case kJSTypeNull:
            std::cout << colors["gray"] << "null" << colors["gray"];
            break;

        case kJSTypeBoolean:
            std::cout << colors["yellow"] << (JSValueToBoolean(ctx, argument) ? "true" : "false")
                      << colors["reset"];
            break;

        case kJSTypeNumber:
            std::cout << colors["yellow"] << JSValueToNumber(ctx, argument, nullptr) << colors["reset"];
            break;

        case kJSTypeString:
            if (colorString == true)
            {
                std::cout << colors["green"] << '"' << toString(ctx, argument) << '"' << colors["reset"];
            }
            else
            {
                printValue(ctx, argument, true);
            }
            break;

        case kJSTypeObject:
            if (JSValueIsArray(ctx, argument))
            {
                printArray(ctx, JSValueToObject(ctx, argument, nullptr), true);
            }
            else if (JSObjectIsFunction(ctx, JSValueToObject(ctx, argument, nullptr)))
            {
                auto str = toString(ctx, argument);

                if (str.find("class") != std::string::npos)
                {
                    std::string className = str.substr(str.find("class ") + 6,
                                                       str.find("{") - str.find("class ") - 6);
                    std::string trimmedClassName = className.substr(0, className.find(" "));
                    bool isEmpty = trimmedClassName == "";
                    std::string finalClass = "[Class" + (isEmpty ? "]" : " " + trimmedClassName + "]");
                    std::cout << colors["cyan"] << finalClass << colors["reset"];
                }
                else
                {
                    std::cout << colors["magenta"] << "[Function]" << colors["reset"];
                }
            }
            else
            {
                printObject(ctx, JSValueToObject(ctx, argument, nullptr), true);
            }
            break;

        default:
            printValue(ctx, argument, false);
            break;
        }
    }
}