#include <JavaScriptCore/JavaScript.h>
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include "println.cpp"
#include "require.cpp"
#include "fetch.cpp"
#include "prototype/_js_Array.cpp"

void DefineGlobal(JSContextRef context, JSObjectRef *globalObject, std::string dirname, std::string fullname, JSValueRef *exception, JSObjectRef *exportsObject, std::string mode)
{
    // Create module system
    JSObjectRef moduleObject = JSObjectMake(context, nullptr, nullptr);
    JSStringRef moduleKey = JSStringCreateWithUTF8CString("module");
    JSObjectSetProperty(context, JSContextGetGlobalObject(context), moduleKey, moduleObject, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(moduleKey);

    // Create exports object for the current module
    JSStringRef exportsKey = JSStringCreateWithUTF8CString("exports");
    JSObjectSetProperty(context, moduleObject, exportsKey, *exportsObject, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(exportsKey);

    // add println function to global context
    JSStringRef println = JSStringCreateWithUTF8CString("println");
    JSObjectSetProperty(context, *globalObject, println, JSObjectMakeFunctionWithCallback(context, nullptr, printlnCallback), kJSPropertyAttributeNone, exception);
    JSStringRelease(println);

    JSStringRef require = JSStringCreateWithUTF8CString("require");
    JSObjectSetProperty(context, *globalObject, require, JSObjectMakeFunctionWithCallback(context, nullptr, requireCallback), kJSPropertyAttributeNone, exception);
    JSStringRelease(require);

    JSStringRef fetchName = JSStringCreateWithUTF8CString("fetch");
    JSObjectRef fetchFunction = JSObjectMakeFunctionWithCallback(context, fetchName, fetchc);
    JSObjectSetProperty(context, *globalObject, fetchName, fetchFunction, kJSPropertyAttributeNone, NULL);
    JSStringRelease(fetchName);

    std::string cwd = _getcwd();
    JSStringRef execcwd = JSStringCreateWithUTF8CString("__execcwd");
    JSStringRef cwdString = JSStringCreateWithUTF8CString(cwd.c_str());
    JSObjectSetProperty(context, *globalObject, execcwd, JSValueMakeString(context, cwdString), kJSPropertyAttributeNone, exception);

    JSStringRef execfilename = JSStringCreateWithUTF8CString("__filename");
    JSStringRef filenameString = JSStringCreateWithUTF8CString(fullname.c_str());
    JSObjectSetProperty(context, *globalObject, execfilename, JSValueMakeString(context, filenameString), kJSPropertyAttributeNone, exception);

    JSStringRef execdirname = JSStringCreateWithUTF8CString("__dirname");
    JSStringRef dirnameString = JSStringCreateWithUTF8CString(dirname.c_str());
    JSObjectSetProperty(context, *globalObject, execdirname, JSValueMakeString(context, dirnameString), kJSPropertyAttributeNone, exception);

    JSStringRef execscope = JSStringCreateWithUTF8CString("__scope");
    JSStringRef scopeString = JSStringCreateWithUTF8CString(mode.c_str());
    JSObjectSetProperty(context, *globalObject, execscope, JSValueMakeString(context, scopeString), kJSPropertyAttributeNone, exception);

    // add .random, .first and .last to array prototype
    JSStringRef randomName = JSStringCreateWithUTF8CString("random");
    JSStringRef firstName = JSStringCreateWithUTF8CString("first");
    JSStringRef lastName = JSStringCreateWithUTF8CString("last");

    JSStringRef arrayName = JSStringCreateWithUTF8CString("Array");
    JSObjectRef arrayConstructor = (JSObjectRef)JSObjectGetProperty(context, *globalObject, arrayName, exception);
    JSStringRelease(arrayName);

    JSObjectRef arrayPrototype = (JSObjectRef)JSObjectGetProperty(context, arrayConstructor, JSStringCreateWithUTF8CString("prototype"), exception);

    JSObjectSetProperty(context, arrayPrototype, randomName, JSObjectMakeFunctionWithCallback(context, randomName, random), kJSPropertyAttributeNone, exception);
    JSObjectSetProperty(context, arrayPrototype, firstName, JSObjectMakeFunctionWithCallback(context, firstName, first), kJSPropertyAttributeNone, exception);
    JSObjectSetProperty(context, arrayPrototype, lastName, JSObjectMakeFunctionWithCallback(context, lastName, last), kJSPropertyAttributeNone, exception);
}