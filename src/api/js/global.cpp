#include <JavaScriptCore/JavaScript.h>
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include "print.cpp"
#include "require.cpp"
#include "fetch.cpp"
#include "prototype/_js_Array.cpp"

namespace global {
    std::string _maindir = "none";

    void define(JSContextRef context, JSObjectRef *globalObject, std::string dirname, std::string fullname,
                JSValueRef *exception, JSObjectRef *exportsObject, std::string mode) {

        auto exportsKey = JSStringCreateWithUTF8CString("exports");
        JSObjectSetProperty(context, *globalObject, exportsKey, *exportsObject, kJSPropertyAttributeNone, nullptr);
        JSStringRelease(exportsKey);

        auto print = JSStringCreateWithUTF8CString("print");
        JSObjectSetProperty(context, *globalObject, print,
                            JSObjectMakeFunctionWithCallback(context, nullptr, print::printCallback),
                            kJSPropertyAttributeNone, exception);

        auto println = JSStringCreateWithUTF8CString("println");
        JSObjectSetProperty(context, *globalObject, println,
                            JSObjectMakeFunctionWithCallback(context, nullptr, print::printlnCallback),
                            kJSPropertyAttributeNone, exception);
        JSStringRelease(println);

        auto require = JSStringCreateWithUTF8CString("require");
        JSObjectSetProperty(context, *globalObject, require,
                            JSObjectMakeFunctionWithCallback(context, nullptr, require::requireCallback),
                            kJSPropertyAttributeNone, exception);
        JSStringRelease(require);

        auto fetchName = JSStringCreateWithUTF8CString("fetch");
        auto fetchFunction = JSObjectMakeFunctionWithCallback(context, fetchName, fetch::fetch);
        JSObjectSetProperty(context, *globalObject, fetchName, fetchFunction, kJSPropertyAttributeNone, NULL);
        JSStringRelease(fetchName);

        std::string cwd = cppFs::getcwd();
        auto execcwd = JSStringCreateWithUTF8CString("__execcwd");
        auto cwdString = JSStringCreateWithUTF8CString(cwd.c_str());
        JSObjectSetProperty(context, *globalObject, execcwd, JSValueMakeString(context, cwdString),
                            kJSPropertyAttributeNone, exception);

        auto execfilename = JSStringCreateWithUTF8CString("__filename");
        auto filenameString = JSStringCreateWithUTF8CString(fullname.c_str());
        JSObjectSetProperty(context, *globalObject, execfilename, JSValueMakeString(context, filenameString),
                            kJSPropertyAttributeNone, exception);

        auto execdirname = JSStringCreateWithUTF8CString("__dirname");
        auto dirnameString = JSStringCreateWithUTF8CString(dirname.c_str());
        JSObjectSetProperty(context, *globalObject, execdirname, JSValueMakeString(context, dirnameString),
                            kJSPropertyAttributeNone, exception);

        auto execscope = JSStringCreateWithUTF8CString("__scope");
        auto scopeString = JSStringCreateWithUTF8CString(mode.c_str());
        JSObjectSetProperty(context, *globalObject, execscope, JSValueMakeString(context, scopeString),
                            kJSPropertyAttributeNone, exception);

        auto maindir = JSStringCreateWithUTF8CString("__maindir");
        std::string maindirString = dirname;

        if (_maindir == "none") {
            _maindir = dirname;
        } else {
            maindirString = _maindir;
        }

        JSObjectSetProperty(context, *globalObject, maindir, JSValueMakeString(context, JSStringCreateWithUTF8CString(maindirString.c_str())),
                            kJSPropertyAttributeNone, exception);

        // add .random, .first and .last to array prototype
        auto randomName = JSStringCreateWithUTF8CString("random");
        auto firstName = JSStringCreateWithUTF8CString("first");
        auto lastName = JSStringCreateWithUTF8CString("last");

        auto arrayName = JSStringCreateWithUTF8CString("Array");
        auto arrayConstructor = (JSObjectRef) JSObjectGetProperty(context, *globalObject, arrayName, exception);
        JSStringRelease(arrayName);

        auto arrayPrototype = (JSObjectRef) JSObjectGetProperty(context, arrayConstructor,
                                                                JSStringCreateWithUTF8CString("prototype"), exception);

        JSObjectSetProperty(context, arrayPrototype, randomName,
                            JSObjectMakeFunctionWithCallback(context, randomName, ArrayProto::random),
                            kJSPropertyAttributeNone, exception);
        JSObjectSetProperty(context, arrayPrototype, firstName,
                            JSObjectMakeFunctionWithCallback(context, firstName, ArrayProto::first),
                            kJSPropertyAttributeNone, exception);
        JSObjectSetProperty(context, arrayPrototype, lastName,
                            JSObjectMakeFunctionWithCallback(context, lastName, ArrayProto::last),
                            kJSPropertyAttributeNone, exception);
    }
}