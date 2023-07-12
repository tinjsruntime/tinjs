#include "fetch.hpp"
#include <JavaScriptCore/JavaScript.h>
#include <curl/curl.h>
#include <string>
#include <map>
#include "../cpp/jsc_util.hpp"

using namespace jscUtil;

namespace fetch
{
    JSValueRef jsonify(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                       const JSValueRef arguments[], JSValueRef *exception)
    {
        // Get the response object from the function call
        auto responseProp = JSStringCreateWithUTF8CString("response");
        auto responseValue = JSObjectGetProperty(ctx, thisObject, responseProp, NULL);
        auto responseString = JSValueToStringCopy(ctx, responseValue, NULL);
        size_t responseLength = JSStringGetMaximumUTF8CStringSize(responseString);
        char response[responseLength];
        JSStringGetUTF8CString(responseString, response, responseLength);

        // Parse the response as JSON
        auto json = JSStringCreateWithUTF8CString(response);
        auto jsonValue = JSValueMakeFromJSONString(ctx, json);
        return jsonValue;
    }

    size_t writeCallback(char *contents, size_t size, size_t nmemb, std::string *response)
    {
        size_t totalSize = size * nmemb;
        response->append(contents, totalSize);
        return totalSize;
    }

    JSValueRef fetch(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
                     const JSValueRef arguments[], JSValueRef *exception)
    {
        if (argumentCount < 1)
        {
            *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString(
                                                    "fetch function expects atleast one argument"));
            return JSValueMakeUndefined(ctx);
        }

        if (!JSValueIsString(ctx, arguments[0]))
        {
            *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString("fetch expects a string as the first argument"));
            return JSValueMakeUndefined(ctx);
        }

        std::string method = "GET";
        std::string body = "";
        std::map<std::string, std::string> headers;

        if (argumentCount > 1 && JSValueIsObject(ctx, arguments[1]))
        {
            auto _method = JSStringCreateWithUTF8CString("method");
            auto methodValue = JSObjectGetProperty(ctx, JSValueToObject(ctx, arguments[1], NULL), _method, NULL);
            auto methodString = toString(ctx, methodValue);

            method = methodString;

            auto _body = JSStringCreateWithUTF8CString("body");
            auto bodyValue = JSObjectGetProperty(ctx, JSValueToObject(ctx, arguments[1], NULL), _body, NULL);
            auto bodyString = toString(ctx, bodyValue);

            body = bodyString;

            auto _headers = JSStringCreateWithUTF8CString("headers");
            auto headersValue = JSObjectGetProperty(ctx, JSValueToObject(ctx, arguments[1], NULL), _headers, NULL);

            if (JSValueIsObject(ctx, headersValue))
            {
                auto propertyArray = JSObjectCopyPropertyNames(ctx, JSValueToObject(ctx, headersValue, NULL));
                size_t count = JSPropertyNameArrayGetCount(propertyArray);

                for (int i = 0; i < count; i++)
                {
                    auto propertyName = JSPropertyNameArrayGetNameAtIndex(propertyArray, i);
                    auto propertyValue = JSObjectGetProperty(ctx, JSValueToObject(ctx, headersValue, NULL), propertyName, NULL);
                    auto propertyString = toString(ctx, propertyValue);

                    headers[toString(ctx, JSValueMakeString(ctx, propertyName))] = propertyString;
                }
            }
        }
        // Get the URL argument from the function call
        auto urlString = JSValueToStringCopy(ctx, arguments[0], NULL);
        auto urlLength = JSStringGetMaximumUTF8CStringSize(urlString);
        char url[urlLength];
        JSStringGetUTF8CString(urlString, url, urlLength);

        std::string urlstr = std::string(url);
        // Initialize libcurl
        curl_global_init(CURL_GLOBAL_DEFAULT);
        CURL *curl = curl_easy_init();

        curl_easy_setopt(curl, CURLOPT_URL, urlstr.c_str());

        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        if (method == "POST")
        {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);

            if (body != "")
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }

        struct curl_slist *chunk = NULL;
        for (auto const &header : headers)
        {
            std::string headerString = header.first + ": " + header.second;
            chunk = curl_slist_append(chunk, headerString.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        // Perform the fetch request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            // Throw an exception if the request fails
            *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString(curl_easy_strerror(res)));
        }

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        // Clean up libcurl
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        /*
         *   Return the response as a object:
         *   {
         *      status: (int) status code,
         *      response: (string) response body,
         *      json: (function) returns the response as a JSON object (JSON.parse)
         *  }
         */
        auto responseObj = JSObjectMake(ctx, NULL, NULL);
        auto statusProp = JSStringCreateWithUTF8CString("status");
        auto responseProp = JSStringCreateWithUTF8CString("response");
        auto jsonProp = JSStringCreateWithUTF8CString("json");
        auto statusValue = JSValueMakeNumber(ctx, (double)http_code);
        auto responseValue = JSStringCreateWithUTF8CString(response.c_str());
        JSObjectSetProperty(ctx, responseObj, statusProp, statusValue, kJSPropertyAttributeNone, NULL);
        JSObjectSetProperty(ctx, responseObj, responseProp, JSValueMakeString(ctx, responseValue),
                            kJSPropertyAttributeNone, NULL);
        JSObjectSetProperty(ctx, responseObj, jsonProp, JSObjectMakeFunctionWithCallback(ctx, nullptr, jsonify),
                            kJSPropertyAttributeNone, NULL);
        return responseObj;
    }
}