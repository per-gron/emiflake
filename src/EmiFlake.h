#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#ifndef emilir_EmiFlake_h
#define emilir_EmiFlake_h

#include "flake.h"

#include <node.h>

class EmiFlake : public node::ObjectWrap {
    friend class EmiConnDelegate;
    friend class EmiSockDelegate;
    friend class EmiBinding;
    
private:
    emilir::flake _flake;
    
    // Private copy constructor and assignment operator
    inline EmiFlake(const EmiFlake& other);
    inline EmiFlake& operator=(const EmiFlake& other);
    
    EmiFlake(const unsigned char *machineNumber,
             size_t machineNumberLength,
             size_t sequenceNumberLength,
             size_t timestampLength,
             unsigned long long timeOffset);
    virtual ~EmiFlake();
    
    static v8::Handle<v8::Value> ByteLength(const v8::Arguments& args);
    static v8::Handle<v8::Value> BitLength(const v8::Arguments& args);
    static v8::Handle<v8::Value> Generate(const v8::Arguments& args);
    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    
public:
    static void Init(v8::Handle<v8::Object> target);
    
    static v8::Persistent<v8::Function> generate;
};

#endif
