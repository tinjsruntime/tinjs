#include "fetch.hpp"
#include <JavaScriptCore/JavaScript.h>
#include <curl/curl.h>
#include <string>

namespace fetch {
JSValueRef jsonify(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    // Get the response object from the function call
    JSStringRef responseProp = JSStringCreateWithUTF8CString("response");
    JSValueRef responseValue = JSObjectGetProperty(ctx, thisObject, responseProp, NULL);
    JSStringRef responseString = JSValueToStringCopy(ctx, responseValue, NULL);
    size_t responseLength = JSStringGetMaximumUTF8CStringSize(responseString);
    char response[responseLength];
    JSStringGetUTF8CString(responseString, response, responseLength);

    // Parse the response as JSON
    JSStringRef json = JSStringCreateWithUTF8CString(response);
    JSValueRef jsonValue = JSValueMakeFromJSONString(ctx, json);
    return jsonValue;
}

size_t writeCallback(char *contents, size_t size, size_t nmemb, std::string *response)
{
    size_t totalSize = size * nmemb;
    response->append(contents, totalSize);
    return totalSize;
}

JSValueRef fetch(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    if (argumentCount != 1 || !JSValueIsString(ctx, arguments[0]))
    {
        // Throw an exception if the fetch function is called with invalid arguments
        *exception = JSValueMakeString(ctx, JSStringCreateWithUTF8CString("fetch function expects a single string URL argument"));
        return JSValueMakeUndefined(ctx);
    }

    // Get the URL argument from the function call
    JSStringRef urlString = JSValueToStringCopy(ctx, arguments[0], NULL);
    size_t urlLength = JSStringGetMaximumUTF8CStringSize(urlString);
    char url[urlLength];
    JSStringGetUTF8CString(urlString, url, urlLength);

    std::string urlstr = std::string(url);
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();

    std::cout << "Fetching " << urlstr << std::endl;
    // Set the URL for the fetch request
    curl_easy_setopt(curl, CURLOPT_URL, urlstr.c_str());

    // Set the callback function to receive the response
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

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
    JSObjectRef responseObj = JSObjectMake(ctx, NULL, NULL);
    JSStringRef statusProp = JSStringCreateWithUTF8CString("status");
    JSStringRef responseProp = JSStringCreateWithUTF8CString("response");
    JSStringRef jsonProp = JSStringCreateWithUTF8CString("json");
    JSValueRef statusValue = JSValueMakeNumber(ctx, (double)http_code);
    JSStringRef responseValue = JSStringCreateWithUTF8CString(response.c_str());
    JSObjectSetProperty(ctx, responseObj, statusProp, statusValue, kJSPropertyAttributeNone, NULL);
    JSObjectSetProperty(ctx, responseObj, responseProp, JSValueMakeString(ctx, responseValue), kJSPropertyAttributeNone, NULL);
    JSObjectSetProperty(ctx, responseObj, jsonProp, JSObjectMakeFunctionWithCallback(ctx, nullptr, jsonify), kJSPropertyAttributeNone, NULL);
    return responseObj;
}
}