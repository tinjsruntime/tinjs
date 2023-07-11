#include "require.hpp"
#include <iostream>
#include "../cpp/cpp_fs.hpp"
#include "global.hpp"
#include <json/json.h>
#include "../cpp/jsc_util.hpp"
#include "module/test.cpp"
#include "module/fs.cpp"

namespace require
{
    std::map<std::string, Json::Value> pkgCache;
    std::map<std::string, JSValueRef> moduleCache;
    std::string __maindir = "none";
    
    Json::Value parseJson(std::string contents)
    {
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(contents, root);

        if (!parsingSuccessful)
        {
            throw std::runtime_error("Error parsing package.json");
        }

        return root;
    }

    Json::Value getPackageJson(std::string dirname, JSContextRef ctx, JSValueRef *exception)
    {
        // get __maindir from global
        auto packageJsonPath = cppFs::absolute("pkg.tin.json", __maindir);

        if (pkgCache.find(packageJsonPath) != pkgCache.end())
        {
            return pkgCache[packageJsonPath];
        }

        std::string packageJsonString;

        try
        {
            packageJsonString = cppFs::readFile(packageJsonPath);
        }
        catch (const std::runtime_error &error)
        {
            *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString(error.what()));
            return "";
        }

        Json::Value packageJson;

        try
        {
            packageJson = parseJson(packageJsonString);
        }
        catch (const std::runtime_error &error)
        {
            *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString(error.what()));
            return "";
        }

        pkgCache[packageJsonPath] = packageJson;
        return packageJson;
    }

    JSValueRef requireCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                               const JSValueRef arguments[], JSValueRef *exception)
    {
        if (argumentCount != 1)
        {
            *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString("require expects exactly one argument"));
            return JSValueMakeUndefined(ctx);
        }

        auto path = toString(ctx, arguments[0]);

        if (__maindir == "none") {
            auto maindirKey = JSStringCreateWithUTF8CString("__maindir");
            auto maindirValue = JSObjectGetProperty(ctx, JSContextGetGlobalObject(ctx), maindirKey, exception);
            JSStringRelease(maindirKey);
            __maindir = toString(ctx, maindirValue);
        }

        auto scope = JSContextGetGlobalObject(ctx);
        auto scopeKey = JSStringCreateWithUTF8CString("__scope");
        auto scopeValue = JSObjectGetProperty(ctx, scope, scopeKey, exception);
        auto scopeStr = toString(ctx, scopeValue);

        JSStringRelease(scopeKey);

        auto dirnameKey = JSStringCreateWithUTF8CString("__dirname");
        auto dirnameValue = JSObjectGetProperty(ctx, JSContextGetGlobalObject(ctx), dirnameKey, exception);
        JSStringRelease(dirnameKey);

        auto dirnameString = toString(ctx, dirnameValue);

        std::string finalPath;

        if (path[0] == '@')
        {
            auto packageName = path.substr(1);
            auto packageJson = getPackageJson(__maindir, ctx, exception);
            auto _pkg = packageJson[packageName];

            if (moduleCache.find(packageName) != moduleCache.end())
            {
                return moduleCache[packageName];
            }
            
            if (_pkg == Json::nullValue)
            {
                *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString("package not found"));
                return JSValueMakeUndefined(ctx);
            }


            if (_pkg["deps"] != Json::nullValue)
            {
                std::cout << "requiring deps" << std::endl;
                /**
                 * deps is:
                 *  {
                 *      "dependency": "version" (version does not matter for now)
                 *  }
                */

                for (const auto &dep : _pkg["deps"].getMemberNames())
                {

                    std::cout << "Key: " << dep << std::endl;
                    auto depPath = "@" + std::string(dep.c_str());

                    std::cout << "requiring dep: " << depPath << std::endl;
                    // args
                    JSValueRef args[] = { JSValueMakeString(ctx, JSStringCreateWithUTF8CString(depPath.c_str())) };
                    auto depValue = requireCallback(ctx, function, thisObject, 1, args, exception);

                    if (JSValueIsUndefined(ctx, depValue))
                    {
                        return JSValueMakeUndefined(ctx);
                    }
                }
            }

            finalPath = cppFs::absolute(_pkg["main"].asString(), __maindir);
        }

        if (finalPath == "")
        {
            if (path[0] == '.')
            {
                finalPath = cppFs::absolute(path, dirnameString);
            }
            else
            {
                finalPath = path;
            }
        }

        // if finalpath starts with tin:, its a builtin module
        if (finalPath.substr(0, 4) == "tin:")
        {
            std::string moduleName = finalPath.substr(4);

            if (moduleName == "fs")
            {
                auto fsObject = JSObjectMake(ctx, nullptr, nullptr);
                return fsMod::init_fs_module(ctx, &fsObject);
            }
            else if (moduleName == "test")
            {
                if (scopeStr != "test")
                {
                    *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString(
                                                            "test module can only be required in test scope (run the cli with ./tin test instead of ./tin run)"));
                    return JSValueMakeUndefined(ctx);
                }

                auto testObject = JSObjectMake(ctx, nullptr, nullptr);
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
        auto _globalObject = JSContextGetGlobalObject(ctx);
        auto _moduleObject = JSObjectMake(ctx, nullptr, nullptr);
        auto _exportsObject = JSObjectMake(ctx, nullptr, nullptr);
        auto dirname = cppFs::parent(finalPath);

        global::define(ctx, &_globalObject, dirname, finalPath, exception, &_exportsObject, scopeStr);

        // Evaluate the required file
        auto code = JSStringCreateWithUTF8CString(fileContents.c_str());
        auto value = JSEvaluateScript(ctx, code, _moduleObject, nullptr, 1, exception);

        JSStringRelease(code);

        // return the old global object

        // Return the default of exports object of the required module
        // get default property of exports object
        auto exportsKey = JSStringCreateWithUTF8CString("default");
        auto exportsValue = JSObjectGetProperty(ctx, _exportsObject, exportsKey, exception);

        if (JSValueIsUndefined(ctx, exportsValue))
        {
            // if default property is undefined, return the exports object
            exportsValue = _exportsObject;
        }

        JSStringRelease(exportsKey);

        moduleCache[finalPath] = exportsValue;
        return exportsValue;
    }
}