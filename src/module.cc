#define BUILDING_NODE_EXTENSION

#include "EmiFlake.h"

#include <node.h>

using namespace v8;

void InitAll(Handle<Object> target) {
    HandleScope scope;
    
    EmiFlake::Init(target);
}

NODE_MODULE(emiflake, InitAll)
