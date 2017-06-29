#include <node.h>

namspace demo{
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Oject;
    using v8::String;
    using v8::Value;

    void Method(const FunctionCallbackInfo<Value>& args){
        Isolate* isolate = args.GetIsolate();
        args.GetReturnValue().Set(String::NewFromUTF8(isolate, "world"));
    }

    void init(Local<Object> exports){
        NODE_SET_METHOD(exports,'hello',Method);
    }

    NODE_MODULE(addon, init)
}