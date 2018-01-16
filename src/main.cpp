#include <string>
#include <memory>
#include <nan.h>
#include "msoconv.h"

struct ConvBaton {
    uv_work_t req;
    Nan::Callback callback;
    std::string src;
    std::string dest;
    std::string type;
    int code;
};

void ConvAsync(uv_work_t* req)
{
    ConvBaton* baton = (ConvBaton*)req->data;
    baton->code = Conv(baton->src, baton->dest, baton->type);
}

void ConvComplete(uv_work_t* req, int status)
{
    Nan::HandleScope scope;
    // we need clear req->data after complete function called
    std::unique_ptr<ConvBaton> baton((ConvBaton*)req->data);
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {
        Nan::New<v8::Number>(baton->code)
    };
    baton->callback.Call(argc, argv);
}

NAN_METHOD(Conv)
{
    Nan::HandleScope scope;
    if (info.Length() >= 4) {
        try {
            ConvBaton* baton = new ConvBaton();
            baton->req.data = baton;
            baton->callback.Reset(v8::Local<v8::Function>::Cast(info[3]));
            baton->src = *v8::String::Utf8Value(info[0]);
            baton->dest = *v8::String::Utf8Value(info[1]);
            baton->type = *v8::String::Utf8Value(info[2]);
            uv_queue_work(uv_default_loop(), &baton->req, ConvAsync, ConvComplete);
        } catch (const std::exception& ex) {
            Nan::ThrowError(ex.what());
        }
    }
}

void Init(v8::Handle<v8::Object> exports)
{
    // node::AtExit(UnInitialize);
    // Initialize();
    Nan::SetMethod(exports, "conv", Conv);
}

NODE_MODULE(msoconv, Init)