#ifndef FS_MODULE_H
#define FS_MODULE_H

#include <JavaScriptCore/JavaScript.h>

namespace fsMod {
    JSObjectRef init_fs_module(JSContextRef context, JSObjectRef *exports);
}

#endif