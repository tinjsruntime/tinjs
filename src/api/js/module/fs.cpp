#include "fs.hpp"
#include <JavaScriptCore/JavaScript.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "../../cpp/jsc_util.hpp"

// readFileCallback

JSValueRef readFileCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    // check if argumentCount is 1
    if (argumentCount != 1)
    {
        JSStringRef error = JSStringCreateWithUTF8CString("readFile requires 1 argument");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    // check if argument is a string
    if (!JSValueIsString(ctx, arguments[0]))
    {
        JSStringRef error = JSStringCreateWithUTF8CString("readFile requires a string argument");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    auto str = toString(ctx, arguments[0]);

    struct stat buffer2;

    if (stat(str.c_str(), &buffer2) != 0)
    {
        JSStringRef error = JSStringCreateWithUTF8CString("file does not exist");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    std::ifstream file(str);

    std::stringstream stream;
    stream << file.rdbuf();
    std::string contents = stream.str();

    file.close();

    JSStringRef result = JSStringCreateWithUTF8CString(contents.c_str());
    return JSValueMakeString(ctx, result);
}

// writeFileCallback

JSValueRef writeFileCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    // check if argumentCount is 2
    if (argumentCount != 2)
    {
        JSStringRef error = JSStringCreateWithUTF8CString("writeFile requires 2 arguments");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    // check if argument is a string
    if (!JSValueIsString(ctx, arguments[0]) && !JSValueIsString(ctx, arguments[1]))
    {
        JSStringRef error = JSStringCreateWithUTF8CString("writeFile requires both arguments to be strings");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    auto str = toString(ctx, arguments[0]);
    auto str2 = toString(ctx, arguments[1]);

    std::ofstream file(str);

    file << str2;

    file.close();

    return JSValueMakeUndefined(ctx);
}

// appendFileCallback

JSValueRef appendFileCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    // check if argumentCount is 2
    if (argumentCount != 2)
    {
        JSStringRef error = JSStringCreateWithUTF8CString("appendFile requires 2 arguments");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    // check if argument is a string
    if (!JSValueIsString(ctx, arguments[0]) && !JSValueIsString(ctx, arguments[1]))
    {
        JSStringRef error = JSStringCreateWithUTF8CString("appendFile requires both arguments to be strings");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    auto str = toString(ctx, arguments[0]);
    auto str2 = toString(ctx, arguments[1]);

    std::ofstream file(str, std::ios_base::app);

    file << str2;

    file.close();

    return JSValueMakeUndefined(ctx);
}

// rmFileCallback

JSValueRef rmFileCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    // check if argumentCount is 1
    if (argumentCount != 1)
    {
        JSStringRef error = JSStringCreateWithUTF8CString("rmFile requires 1 argument");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    // check if argument is a string
    if (!JSValueIsString(ctx, arguments[0]))
    {
        JSStringRef error = JSStringCreateWithUTF8CString("rmFile requires a string argument");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    auto str = toString(ctx, arguments[0]);

    struct stat buffer2;

    if (stat(str.c_str(), &buffer2) != 0)
    {
        JSStringRef error = JSStringCreateWithUTF8CString("file does not exist");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    remove(str.c_str());

    return JSValueMakeUndefined(ctx);
}

JSValueRef existsCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    // check if argumentCount is 1
    if (argumentCount != 1)
    {
        JSStringRef error = JSStringCreateWithUTF8CString("exists requires 1 argument");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    // check if argument is a string
    if (!JSValueIsString(ctx, arguments[0]))
    {
        JSStringRef error = JSStringCreateWithUTF8CString("exists requires a string argument");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    auto str = toString(ctx, arguments[0]);

    struct stat buffer2;

    if (stat(str.c_str(), &buffer2) != 0)
    {
        return JSValueMakeBoolean(ctx, false);
    }

    return JSValueMakeBoolean(ctx, true);
}

JSObjectRef init_fs_module(JSContextRef context, JSObjectRef *exports)
{
    // add readFile function to exports
    JSStringRef readFile = JSStringCreateWithUTF8CString("readFile");
    JSObjectSetProperty(context, *exports, readFile, JSObjectMakeFunctionWithCallback(context, nullptr, readFileCallback), kJSPropertyAttributeNone, nullptr);
    JSStringRelease(readFile);

    JSStringRef writeFile = JSStringCreateWithUTF8CString("writeFile");
    JSObjectSetProperty(context, *exports, writeFile, JSObjectMakeFunctionWithCallback(context, nullptr, writeFileCallback), kJSPropertyAttributeNone, nullptr);
    JSStringRelease(writeFile);

    JSStringRef appendFile = JSStringCreateWithUTF8CString("appendFile");
    JSObjectSetProperty(context, *exports, appendFile, JSObjectMakeFunctionWithCallback(context, nullptr, appendFileCallback), kJSPropertyAttributeNone, nullptr);
    JSStringRelease(appendFile);

    JSStringRef rmFile = JSStringCreateWithUTF8CString("rmFile");
    JSObjectSetProperty(context, *exports, rmFile, JSObjectMakeFunctionWithCallback(context, nullptr, rmFileCallback), kJSPropertyAttributeNone, nullptr);
    JSStringRelease(rmFile);

    JSStringRef exists = JSStringCreateWithUTF8CString("exists");
    JSObjectSetProperty(context, *exports, exists, JSObjectMakeFunctionWithCallback(context, nullptr, existsCallback), kJSPropertyAttributeNone, nullptr);
    JSStringRelease(exists);
    return *exports;
}