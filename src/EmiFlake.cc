#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include "EmiFlake.h"

#include <node.h>
#include <node_buffer.h>
#include <stdlib.h>

using namespace v8;

#define THROW_TYPE_ERROR(err)                                 \
  do {                                                        \
    ThrowException(Exception::TypeError(String::New(err)));   \
    return scope.Close(Undefined());                          \
  } while (0)

Persistent<Function> EmiFlake::generate;

EmiFlake::EmiFlake(const unsigned char *machineNumber,
                   size_t machineNumberLength,
                   size_t sequenceNumberLength,
                   size_t timestampLength,
                   unsigned long long timeOffset) :
_flake(machineNumber,
       machineNumberLength,
       sequenceNumberLength,
       timestampLength,
       timeOffset) {}

EmiFlake::~EmiFlake() {}

void EmiFlake::Init(Handle<Object> target) {
    HandleScope scope;
    
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("EmiFlake"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    
    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("generate"),
                                  FunctionTemplate::New(Generate)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("byteLength"),
                                  FunctionTemplate::New(ByteLength)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("bitLength"),
                                  FunctionTemplate::New(BitLength)->GetFunction());

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("EmiFlake"), constructor);
}

Handle<Value> EmiFlake::BitLength(const Arguments& args) {
    HandleScope scope;

    EmiFlake *flake(ObjectWrap::Unwrap<EmiFlake>(args.This()));
    
    return scope.Close(Integer::New(flake->_flake.bit_length()));
}

Handle<Value> EmiFlake::ByteLength(const Arguments& args) {
    HandleScope scope;

    EmiFlake *flake(ObjectWrap::Unwrap<EmiFlake>(args.This()));
    
    return scope.Close(Integer::New(flake->_flake.byte_length()));
}

Handle<Value> EmiFlake::Generate(const Arguments& args) {
    HandleScope scope;

    if (1 != args.Length()) {
        THROW_TYPE_ERROR("Wrong number of arguments");
    }

    if (args[0].IsEmpty() || !args[0]->IsObject()) {
        THROW_TYPE_ERROR("Wrong arguments");
    }
    Local<Object> buffer(args[0]->ToObject());

    size_t dataLen = (buffer.IsEmpty() ?
                      0 :
                      node::Buffer::Length(buffer));
    unsigned char *data = ((unsigned char *)
                           (buffer.IsEmpty() ?
                            NULL :
                            node::Buffer::Data(buffer)));

    EmiFlake *flake(ObjectWrap::Unwrap<EmiFlake>(args.This()));
    int result = flake->_flake.generate(dataLen, data);

    return scope.Close(Integer::New(result));
}

Handle<Value> EmiFlake::New(const Arguments& args) {
    HandleScope scope;
    
    // machineNumber;
    // machineNumberLength;
    // sequenceNumberLength;
    // timestampLength;

    size_t numArgs = args.Length();
    if (5 != numArgs) {
        THROW_TYPE_ERROR("Wrong number of arguments");
    }
    
    // machineNumber
    if (args[0].IsEmpty() || !args[0]->IsObject()) {
        THROW_TYPE_ERROR("Wrong machineNumber argument");
    }
    Local<Object> machineNumberBuffer(args[0]->ToObject());
    
    // machineNumberLength
    if (args[1].IsEmpty() || !args[1]->IsNumber()) {
        THROW_TYPE_ERROR("Wrong machineNumberLength argument");
    }
    Local<Integer> machineNumberLength = Local<Integer>::Cast(args[1]);
    
    // sequenceNumberLength
    if (args[2].IsEmpty() || !args[2]->IsNumber()) {
        THROW_TYPE_ERROR("Wrong sequenceNumberLength argument");
    }
    Local<Integer> sequenceNumberLength(args[2]->ToInteger());
    
    // timestampLength
    if (args[3].IsEmpty() || !args[3]->IsNumber()) {
        THROW_TYPE_ERROR("Wrong timestampLength argument");
    }
    Local<Integer> timestampLength(args[3]->ToInteger());
    
    // timeOffset
    if (args[4].IsEmpty() || !args[4]->IsNumber()) {
        THROW_TYPE_ERROR("Wrong timeOffset argument");
    }
    Local<Integer> timeOffset(args[4]->ToInteger());
    
    size_t dataLen = (machineNumberBuffer.IsEmpty() ?
                      0 :
                      node::Buffer::Length(machineNumberBuffer));
    const unsigned char *data = ((const unsigned char *)
                                 (machineNumberBuffer.IsEmpty() ?
                                  NULL :
                                  node::Buffer::Data(machineNumberBuffer)));

    if (dataLen*8 < machineNumberLength->Value()) {
        THROW_TYPE_ERROR("Machine number buffer shorter than the specified machine number length");
    }

    if (timestampLength->Value() > 64) {
        THROW_TYPE_ERROR("Timestamp lengths > 64 are not supported");
    }

    if (sequenceNumberLength->Value() > 64) {
        THROW_TYPE_ERROR("Sequence number lengths > 64 are not supported");
    }

    EmiFlake* obj = new EmiFlake(data,
                                 machineNumberLength->Value(),
                                 sequenceNumberLength->Value(),
                                 timestampLength->Value(),
                                 timeOffset->Value());
    obj->Wrap(args.This());
    
    return args.This();
}
