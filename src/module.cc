#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include "EmiFlake.h"

#include <node.h>

using namespace v8;

void InitAll(Handle<Object> target) {
    HandleScope scope;
    
    EmiFlake::Init(target);
}

NODE_MODULE(emiflake, InitAll)
