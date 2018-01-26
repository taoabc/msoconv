#include <string>
#include <memory>
#include <nan.h>
#include "msoconv.h"

class ConvWorker : public Nan::AsyncWorker {
public:
    explicit ConvWorker(Nan::Callback* callback,
                       const std::string& src,
                       const std::string& dest,
                       const std::string& type)
        : Nan::AsyncWorker(callback),
        src_(src),
        dest_(dest),
        type_(type)
    {

    }

    virtual void WorkComplete()
    {
        Nan::HandleScope scope;
        const int argc = 1;
        v8::Local<v8::Value> argv[argc] = {
            Nan::New<v8::Number>(code_)
        };
        callback->Call(argc, argv);
    }
    
    virtual void Execute()
    {
        code_ = Conv(src_, dest_, type_);
    }
private:
    int code_;
    std::string src_;
    std::string dest_;
    std::string type_;
};

NAN_METHOD(Conv)
{
    Nan::HandleScope scope;
    if (info.Length() >= 4) {
        try {
            Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[3]).ToLocalChecked());
            std::string src = *v8::String::Utf8Value(info[0]);
            std::string dest = *v8::String::Utf8Value(info[1]);
            std::string type = *v8::String::Utf8Value(info[2]);
            Nan::AsyncQueueWorker(new ConvWorker(callback, src, dest, type));
        } catch (const std::exception& ex) {
            Nan::ThrowError(ex.what());
        }
    }
}

void Init(v8::Handle<v8::Object> exports)
{
    Nan::SetMethod(exports, "conv", Conv);
}

NODE_MODULE(msoconv, Init)

//struct ConvBaton {
//    uv_work_t req;
//    Nan::Callback callback;
//    std::string src;
//    std::string dest;
//    std::string type;
//    int code;
//};
//
//void ConvAsync(uv_work_t* req)
//{
//    ConvBaton* baton = (ConvBaton*)req->data;
//    baton->code = Conv(baton->src, baton->dest, baton->type);
//}
//
//void ConvComplete(uv_work_t* req, int status)
//{
//    Nan::HandleScope scope;
//    // we need clear req->data after complete function called
//    std::unique_ptr<ConvBaton> baton((ConvBaton*)req->data);
//    const int argc = 1;
//    v8::Local<v8::Value> argv[argc] = {
//        Nan::New<v8::Number>(baton->code)
//    };
//    baton->callback.Call(argc, argv);
//}
//
//ConvBaton* baton = new ConvBaton();
//baton->req.data = baton;
//baton->callback.Reset(v8::Local<v8::Function>::Cast(info[3]));
//baton->src = *v8::String::Utf8Value(info[0]);
//baton->dest = *v8::String::Utf8Value(info[1]);
//baton->type = *v8::String::Utf8Value(info[2]);
//uv_queue_work(uv_default_loop(), &baton->req, ConvAsync, ConvComplete);
