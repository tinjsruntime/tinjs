#pragma once
#include <JavaScriptCore/JavaScript.h>

namespace fsMod {
    JSObjectRef init_fs_module(JSContextRef context, JSObjectRef *exports);
}