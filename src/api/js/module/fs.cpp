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

using namespace jscUtil;

namespace fsMod {

    JSValueRef readFileCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                                const JSValueRef arguments[], JSValueRef *exception) {
        if (argumentCount != 1) {
            setError(exception, "readFile requires 1 argument");
            return JSValueMakeUndefined(ctx);
        }

        if (!JSValueIsString(ctx, arguments[0])) {
            setError(exception, "readFile requires first argument to be a string");
            return JSValueMakeUndefined(ctx);
        }

        auto str = toString(ctx, arguments[0]);

        struct stat buffer2;

        if (stat(str.c_str(), &buffer2) != 0) {
            setError(exception, "File does not exist");
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

    JSValueRef writeFileCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                                 const JSValueRef arguments[], JSValueRef *exception) {
        if (argumentCount != 2) {
            setError(exception, "writeFile requires 2 arguments");
            return JSValueMakeUndefined(ctx);
        }

        if (!JSValueIsString(ctx, arguments[0]) && !JSValueIsString(ctx, arguments[1])) {
            setError(exception, "writeFile requires both arguments to be strings");
            return JSValueMakeUndefined(ctx);
        }

        auto str = toString(ctx, arguments[0]);
        auto str2 = toString(ctx, arguments[1]);

        std::ofstream file(str);

        file << str2;

        file.close();

        return JSValueMakeUndefined(ctx);
    }

    JSValueRef appendFileCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                                  const JSValueRef arguments[], JSValueRef *exception) {
        if (argumentCount != 2) {
            setError(exception, "appendFile requires 2 arguments");
            return JSValueMakeUndefined(ctx);
        }

        if (!JSValueIsString(ctx, arguments[0]) && !JSValueIsString(ctx, arguments[1])) {
            setError(exception, "appendFile requires both arguments to be strings");
            return JSValueMakeUndefined(ctx);
        }

        auto str = toString(ctx, arguments[0]);
        auto str2 = toString(ctx, arguments[1]);

        std::ofstream file(str, std::ios_base::app);

        file << str2;

        file.close();

        return JSValueMakeUndefined(ctx);
    }

    JSValueRef rmFileCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                              const JSValueRef arguments[], JSValueRef *exception) {
        if (argumentCount != 1) {
            setError(exception, "rmFile requires 1 argument");
            return JSValueMakeUndefined(ctx);
        }

        if (!JSValueIsString(ctx, arguments[0])) {
            setError(exception, "rmFile requires a string argument");
            return JSValueMakeUndefined(ctx);
        }

        auto str = toString(ctx, arguments[0]);

        struct stat buffer2;

        if (stat(str.c_str(), &buffer2) != 0) {
            setError(exception, "File does not exist");
            return JSValueMakeUndefined(ctx);
        }

        remove(str.c_str());

        return JSValueMakeUndefined(ctx);
    }

    JSValueRef existsCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                              const JSValueRef arguments[], JSValueRef *exception) {
        if (argumentCount != 1) {
            setError(exception, "exists requires 1 argument");
            return JSValueMakeUndefined(ctx);
        }

        if (!JSValueIsString(ctx, arguments[0])) {
            setError(exception, "exists requires a string argument");
            return JSValueMakeUndefined(ctx);
        }

        auto str = toString(ctx, arguments[0]);

        struct stat buffer2;

        if (stat(str.c_str(), &buffer2) != 0) {
            return JSValueMakeBoolean(ctx, false);
        }

        return JSValueMakeBoolean(ctx, true);
    }

    JSObjectRef init_fs_module(JSContextRef context, JSObjectRef *exports) {
        JSStringRef readFile = JSStringCreateWithUTF8CString("readFile");
        JSObjectSetProperty(context, *exports, readFile,
                            JSObjectMakeFunctionWithCallback(context, nullptr, readFileCallback),
                            kJSPropertyAttributeNone, nullptr);
        JSStringRelease(readFile);

        JSStringRef writeFile = JSStringCreateWithUTF8CString("writeFile");
        JSObjectSetProperty(context, *exports, writeFile,
                            JSObjectMakeFunctionWithCallback(context, nullptr, writeFileCallback),
                            kJSPropertyAttributeNone, nullptr);
        JSStringRelease(writeFile);

        JSStringRef appendFile = JSStringCreateWithUTF8CString("appendFile");
        JSObjectSetProperty(context, *exports, appendFile,
                            JSObjectMakeFunctionWithCallback(context, nullptr, appendFileCallback),
                            kJSPropertyAttributeNone, nullptr);
        JSStringRelease(appendFile);

        JSStringRef rmFile = JSStringCreateWithUTF8CString("rmFile");
        JSObjectSetProperty(context, *exports, rmFile,
                            JSObjectMakeFunctionWithCallback(context, nullptr, rmFileCallback),
                            kJSPropertyAttributeNone, nullptr);
        JSStringRelease(rmFile);

        JSStringRef exists = JSStringCreateWithUTF8CString("exists");
        JSObjectSetProperty(context, *exports, exists,
                            JSObjectMakeFunctionWithCallback(context, nullptr, existsCallback),
                            kJSPropertyAttributeNone, nullptr);
        JSStringRelease(exists);
        return *exports;
    }
}