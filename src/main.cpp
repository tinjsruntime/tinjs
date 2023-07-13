#include <iostream>
#include <JavaScriptCore/JavaScript.h>
#include "api/cpp/cpp_fs.cpp"
#include "api/js/global.cpp"
#include "api/ext/swc.hpp"
#include <string>

using namespace cppFs;

int main(int argc, char *argv[])
{
    srand(time(NULL));

    if (argc < 3 && argc != 1)
    {
        std::cerr << "Usage: " << argv[0] << " <(run|test)> <filename>" << std::endl;
        return 1;
    }

    std::string mode;

    if (argc == 1)
    {
        mode = "run";
    }
    else
    {
        mode = std::string(argv[1]);
    }

    if (mode != "run" && mode != "test")
    {
        std::cerr << "Usage: " << argv[0] << " <(run|test)> <filename>" << std::endl;

        return 1;
    }

    auto context = JSGlobalContextCreate(0);
    JSValueRef exception = nullptr;

    auto globalObject = JSContextGetGlobalObject(context);
    auto moduleObject = JSObjectMake(context, nullptr, nullptr);
    auto exportsObject = JSObjectMake(context, nullptr, nullptr);

    auto filename = absolute(argv[2], getcwd());
    auto dirname = parent(filename);

    global::define(context, &globalObject, dirname, filename, &exception, &exportsObject, mode);

    auto argv2str = JSStringCreateWithUTF8CString(argv[2]);

    std::string tsPath = "";

    if (argc > 1)
    {

        if (argc > 3)
        {
            std::string arg = argv[3];

            if (arg.substr(0, 5) == "--ts=")
            {
                tsPath = arg.substr(5);
            }

            swc::initTs(tsPath);
        }

        if (tsPath == "" && filename.substr(filename.length() - 3) == ".ts")
        {
            std::cout << "Warning: " << filename << " is a typescript file, but no typescript compiler was specified. "
                      << "This may cause errors." << std::endl;
        }

        std::string fileContents;

        try
        {
            if (!exists(filename)) {
                std::cerr << "Error: File not found at " << filename << std::endl;
                return 1;
            }

            if (tsPath != "")
            {
                if (!exists(tsPath)) {
                    std::cerr << "Error: Typescript compiler not found at " << tsPath << std::endl;
                    return 1;
                }

                fileContents = swc::transpile(filename);
            }
            else
            {
                fileContents = readFile(filename);
            }
        }
        catch (const std::runtime_error &error)
        {
            std::cerr << "Error reading file: " << error.what() << std::endl;
            return 1;
        }

        auto code = JSStringCreateWithUTF8CString(fileContents.c_str());
        auto value = JSEvaluateScript(context, code, moduleObject, argv2str, 1, &exception);

        JSStringRelease(code);

        if (exception != nullptr)
        {
            auto exceptionString = JSValueToStringCopy(context, exception, nullptr);

            if (exceptionString != nullptr)
            {
                std::cerr << "Error: " << toString(context, exception) << std::endl;

                JSStringRelease(exceptionString);
            }

            return 1;
        }
    }
    else
    {
        bool running = true;

        std::cout << "Interactive REPL for tin.js" << std::endl;
        std::cout << "Made with love by tin.js contributors <3" << std::endl;
        std::cout << "Type 'exit' to exit" << std::endl;
        while (running)
        {
            std::cout << "> ";
            std::string input;
            std::getline(std::cin, input);

            if (input == "exit")
            {
                running = false;
                break;
            }

            auto code = JSStringCreateWithUTF8CString(input.c_str());
            auto value = JSEvaluateScript(context, code, moduleObject, argv2str, 1, &exception);

            JSStringRelease(code);

            if (exception != nullptr)
            {
                auto exceptionString = JSValueToStringCopy(context, exception, nullptr);

                if (exceptionString != nullptr)
                {
                    std::cerr << "Error: " << toString(context, exception) << std::endl;
                    JSStringRelease(exceptionString);
                }

                exception = nullptr;
            }
            else
            {
                print::rawPrint(context, value, JSValueGetType(context, value), true);
                std::cout << std::endl;
            }
        }

        return 0;
    }
    JSGlobalContextRelease(context);

    return 0;
}
