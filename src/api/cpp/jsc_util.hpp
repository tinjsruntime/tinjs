#include <JavaScriptCore/JavaScript.h>
#include <string>

namespace jscUtil {
    std::string toString(JSContextRef ctx, JSValueRef value);

    int getArrayLength(JSContextRef ctx, JSObjectRef array);
}
