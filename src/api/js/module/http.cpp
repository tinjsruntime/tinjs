#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include <JavaScriptCore/JavaScript.h>
#include <string>
#include "http.hpp"
#include "../../cpp/jsc_util.hpp"
#include <variant>

using namespace jscUtil;
// http server, not client
namespace httpMod
{
    httplib::Server svr;

    JSObjectRef init_http_module(JSContextRef context, JSObjectRef *exports)
    {
        JSObjectSetProperty(context, *exports, JSStringCreateWithUTF8CString("createServer"),
                            JSObjectMakeFunctionWithCallback(context, JSStringCreateWithUTF8CString("createServer"),
                                                             createServerCallback),
                            kJSPropertyAttributeNone, NULL);

        return *exports;
    }

    JSValueRef createServerCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
    {
        // return object with listen, on and write methods
        auto obj = JSObjectMake(ctx, NULL, NULL);

        // first argument should be port

        JSObjectSetProperty(ctx, obj, JSStringCreateWithUTF8CString("get"),
                            JSObjectMakeFunctionWithCallback(ctx, JSStringCreateWithUTF8CString("get"),
                                                             getCallback),
                            kJSPropertyAttributeNone, NULL);

        JSObjectSetProperty(ctx, obj, JSStringCreateWithUTF8CString("post"),
                            JSObjectMakeFunctionWithCallback(ctx, JSStringCreateWithUTF8CString("post"),
                                                             postCallback),
                            kJSPropertyAttributeNone, NULL);

        JSObjectSetProperty(ctx, obj, JSStringCreateWithUTF8CString("listen"),
                            JSObjectMakeFunctionWithCallback(ctx, JSStringCreateWithUTF8CString("listen"),
                                                             listenCallback),
                            kJSPropertyAttributeNone, NULL);

        return obj;
    }

    JSValueRef get_postCallback(JSContextRef ctx, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception, int pOrC)
    {
        std::string proc = pOrC == 0 ? "get" : "post";

        if (argumentCount != 2)
        {
            auto error = JSStringCreateWithUTF8CString((proc + " requires 2 arguments").c_str());
            *exception = JSValueMakeString(ctx, error);
            JSStringRelease(error);
            return JSValueMakeUndefined(ctx);
        }

        if (!JSValueIsString(ctx, arguments[0]))
        {
            auto error = JSStringCreateWithUTF8CString((proc + " requires first argument to be a string").c_str());
            *exception = JSValueMakeString(ctx, error);
            JSStringRelease(error);
            return JSValueMakeUndefined(ctx);
        }

        if (!JSValueIsObject(ctx, arguments[1]))
        {
            auto error = JSStringCreateWithUTF8CString((proc + " requires second argument to be a function").c_str());
            *exception = JSValueMakeString(ctx, error);
            JSStringRelease(error);
            return JSValueMakeUndefined(ctx);
        }

        auto eventFunction = JSValueToObject(ctx, arguments[1], exception);
        auto c = [&](const std::string &pattern, httplib::Server::Handler handler)
        {
            if (pOrC == 0)
                svr.Get(pattern, handler);
            else
                svr.Post(pattern, handler);
        };

        c(toString(ctx, arguments[0]), [ctx, eventFunction](const httplib::Request &req, httplib::Response &res)
          {
            auto reqobj = JSObjectMake(ctx, NULL, NULL);
            JSObjectSetProperty(ctx, reqobj, JSStringCreateWithUTF8CString("method"),
                                JSValueMakeString(ctx, JSStringCreateWithUTF8CString(req.method.c_str())), kJSPropertyAttributeNone, NULL);
            JSObjectSetProperty(ctx, reqobj, JSStringCreateWithUTF8CString("path"),
                                JSValueMakeString(ctx, JSStringCreateWithUTF8CString(req.path.c_str())), kJSPropertyAttributeNone, NULL);
            JSObjectSetProperty(ctx, reqobj, JSStringCreateWithUTF8CString("body"),
                                JSValueMakeString(ctx, JSStringCreateWithUTF8CString(req.body.c_str())), kJSPropertyAttributeNone, NULL);
            
            auto jsParam = JSObjectMake(ctx, NULL, NULL);

            for (auto &param : req.params) {
                JSObjectSetProperty(ctx, jsParam, JSStringCreateWithUTF8CString(param.first.c_str()),
                                    JSValueMakeString(ctx, JSStringCreateWithUTF8CString(param.second.c_str())), kJSPropertyAttributeNone, NULL);
            }

            JSObjectSetProperty(ctx, reqobj, JSStringCreateWithUTF8CString("params"),
                                jsParam, kJSPropertyAttributeNone, NULL);
            JSObjectSetProperty(ctx, reqobj, JSStringCreateWithUTF8CString("version"),
                                JSValueMakeString(ctx, JSStringCreateWithUTF8CString(req.version.c_str())), kJSPropertyAttributeNone, NULL);
            JSValueRef args[] = {
                reqobj
            };

            auto smthn = JSObjectCallAsFunction(ctx, eventFunction, NULL, 1, args, NULL);
            auto smthnasobj = JSValueToObject(ctx, smthn, NULL);

            // get, type and content
            /**
             * type can be: plain, json, html
            */
            auto type = JSObjectGetProperty(ctx, smthnasobj, JSStringCreateWithUTF8CString("type"), NULL);
            auto content = JSObjectGetProperty(ctx, smthnasobj, JSStringCreateWithUTF8CString("content"), NULL);

            auto typeStr = toString(ctx, type);
            auto contentStr = toString(ctx, content);

            if (typeStr == "plain") {
                res.set_content(contentStr, "text/plain");
            } else if (typeStr == "json") {
                auto _json = jsonAsStr(ctx, content);
                res.set_content(_json, "application/json");
            } else if (typeStr == "html") {
                res.set_content(contentStr, "text/html");
            } else {
                res.set_content("404, Server sent unknown type", "text/plain");
            } });

        return JSValueMakeUndefined(ctx);
    }

    JSValueRef getCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
    {
        return get_postCallback(ctx, argumentCount, arguments, exception, 0);
    }

    JSValueRef postCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
    {
        return get_postCallback(ctx, argumentCount, arguments, exception, 1);
    }

    JSValueRef listenCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
    {
        if (argumentCount != 1)
        {
            auto error = JSStringCreateWithUTF8CString("listen requires 2 arguments");
            *exception = JSValueMakeString(ctx, error);
            JSStringRelease(error);
            return JSValueMakeUndefined(ctx);
        }

        auto port = JSValueToNumber(ctx, arguments[0], exception);

        svr.listen("0.0.0.0", (int)port);
        return JSValueMakeUndefined(ctx);
    }

    JSValueRef existsCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
    {
        return JSValueMakeUndefined(ctx);
    }
}