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

// readFileCallback

JSValueRef readFileCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    // check if argumentCount is 1
    if (argumentCount != 1) {
        JSStringRef error = JSStringCreateWithUTF8CString("readFile requires 1 argument");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    // check if argument is a string
    if (!JSValueIsString(ctx, arguments[0])) {
        JSStringRef error = JSStringCreateWithUTF8CString("readFile requires a string argument");
        *exception = JSValueMakeString(ctx, error);
        JSStringRelease(error);
        return JSValueMakeUndefined(ctx);
    }

    auto str = toString(ctx, arguments[0]);

    struct stat buffer2;

    if (stat(str.c_str(), &buffer2) != 0) {
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


JSObjectRef init_fs_module(JSContextRef context, JSObjectRef *exports) {
    // add readFile function to exports
    JSStringRef readFile = JSStringCreateWithUTF8CString("readFile");
    JSObjectSetProperty(context, *exports, readFile, JSObjectMakeFunctionWithCallback(context, nullptr, readFileCallback), kJSPropertyAttributeNone, nullptr);
    JSStringRelease(readFile);
    return *exports;
}