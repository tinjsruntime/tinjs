#include "require.hpp"
#include <iostream>
#include "../cpp/cpp_fs.hpp"
#include "global.hpp"

#include "../cpp/jsc_util.hpp"
#include "module/test.cpp"
#include "module/fs.cpp"

namespace require {
JSValueRef requireCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    if (argumentCount != 1)
    {
        *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString("require expects exactly one argument"));
        return JSValueMakeUndefined(ctx);
    }

    auto path = toString(ctx, arguments[0]);

    std::string finalPath;

    JSObjectRef scope = JSContextGetGlobalObject(ctx);
    JSStringRef scopeKey = JSStringCreateWithUTF8CString("__scope");
    JSValueRef scopeValue = JSObjectGetProperty(ctx, scope, scopeKey, exception);
    JSStringRelease(scopeKey);

    auto scopeStr = toString(ctx, scopeValue);

    if (path[0] == '.')
    {
        JSStringRef dirnameKey = JSStringCreateWithUTF8CString("__dirname");
        JSValueRef dirnameValue = JSObjectGetProperty(ctx, JSContextGetGlobalObject(ctx), dirnameKey, exception);
        JSStringRelease(dirnameKey);

        auto dirnameString = toString(ctx, dirnameValue);
        finalPath = cppFs::absolute(path, dirnameString);
    }
    else
    {
        finalPath = path;
    }

    // if finalpath starts with tin:, its a builtin module
    if (finalPath.substr(0, 4) == "tin:")
    {
        std::string moduleName = finalPath.substr(4);

        if (moduleName == "fs")
        {
            JSObjectRef fsObject = JSObjectMake(ctx, nullptr, nullptr);
            return fsMod::init_fs_module(ctx, &fsObject);
        }
        else if (moduleName == "test")
        {
            if (scopeStr != "test")
            {
                *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString("test module can only be required in test scope (run the cli with ./tin test instead of ./tin run)"));
                return JSValueMakeUndefined(ctx);
            }

            JSObjectRef testObject = JSObjectMake(ctx, nullptr, nullptr);
            return testMod::init_test_module(ctx, &testObject);
        }
        else
        {
            *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString("module not found"));
            return JSValueMakeUndefined(ctx);
        }
    }

    std::string fileContents;
    try
    {
        fileContents = cppFs::readFile(finalPath);
    }
    catch (const std::runtime_error &error)
    {
        *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString(error.what()));
        return JSValueMakeUndefined(ctx);
    }

    // Create a new module object for the required file
    JSObjectRef _globalObject = JSContextGetGlobalObject(ctx);
    JSObjectRef _moduleObject = JSObjectMake(ctx, nullptr, nullptr);
    JSObjectRef _exportsObject = JSObjectMake(ctx, nullptr, nullptr);

    std::string dirname = cppFs::parent(finalPath);
    global::define(ctx, &_globalObject, dirname, finalPath, exception, &_exportsObject, scopeStr);

    // Evaluate the required file
    JSStringRef code = JSStringCreateWithUTF8CString(fileContents.c_str());

    JSValueRef value = JSEvaluateScript(ctx, code, _moduleObject, nullptr, 1, exception);
    JSStringRelease(code);

    // Return the exports object of the required module
    return _exportsObject;
}
}