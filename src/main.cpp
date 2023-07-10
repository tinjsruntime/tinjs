#include <iostream>
#include <JavaScriptCore/JavaScript.h>
#include "api/cpp/cpp_fs.cpp"
#include "api/js/global.cpp"
#include <string>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <(run|test)> <filename>" << std::endl;
        return 1;
    }

    auto mode = std::string(argv[1]);

    if (mode != "run" && mode != "test")
    {
        std::cerr << "Usage: " << argv[0] << " <(run|test)> <filename>" << std::endl;
        return 1;
    }

    JSGlobalContextRef context = JSGlobalContextCreate(0);
    JSValueRef exception = nullptr;

    auto globalObject = JSContextGetGlobalObject(context);
    auto moduleObject = JSObjectMake(context, nullptr, nullptr);
    auto exportsObject = JSObjectMake(context, nullptr, nullptr);

    auto filename = absolute(argv[2], _getcwd());
    auto dirname = parent(filename);
    DefineGlobal(context, &globalObject, dirname, filename, &exception, &exportsObject, mode);

    std::string fileContents;

    try
    {
        fileContents = readFile(filename);
    }
    catch (const std::runtime_error &error)
    {
        std::cerr << "Error reading file: " << error.what() << std::endl;
        return 1;
    }

    JSStringRef code = JSStringCreateWithUTF8CString(fileContents.c_str());
    JSValueRef value = JSEvaluateScript(context, code, moduleObject, JSStringCreateWithUTF8CString(argv[2]), 1, &exception);
    JSStringRelease(code);

    if (exception != nullptr)
    {
        JSStringRef exceptionString = JSValueToStringCopy(context, exception, nullptr);

        if (exceptionString != nullptr)
        {
            auto err = toString(context, exception);
            std::cerr << "Error: " << err << std::endl;
            JSStringRelease(exceptionString);
        }

        return 1;
    }

    JSGlobalContextRelease(context);
    return 0;
}
