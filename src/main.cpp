#include <string>
#include <memory>
#include <node_api.h>
#include "async-base.h"
#include "msoconv.h"
#include "string-operate.h"

class ConvWorker : public AsyncBase
{
public:
    explicit ConvWorker(
        napi_env env,
        napi_value callback,
        const std::string& src,
        const std::string dest,
        const std::string& type) :
            AsyncBase(env, callback, "conv-worker"),
            src_(src),
            dest_(dest), 
            type_(type)
    {}

private:
    virtual void execute(napi_env env) override // run in work thread
    {
        code_ = Conv(src_, dest_, type_);
    }

    virtual void complete(napi_env env, napi_status status) override
    {
        napi_value value;
        napi_status s = napi_create_int32(env, code_, &value);
        if (s == napi_ok)
        {
            napi_value result;
            callback(1, &value, &result);
        }
    }

private:
    int code_ = 0;
    std::string src_;
    std::string dest_;
    std::string type_;
};

std::string getUtf8(napi_env env, napi_value value)
{
    size_t outSize = 0;
    napi_status status = napi_get_value_string_utf8(env, value, nullptr, 0, &outSize);
    if (outSize > 0)
    {
        size_t bufferSize = outSize + 1;
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(bufferSize);
        status = napi_get_value_string_utf8(env, value, buffer.get(), bufferSize, &outSize);
        if (status != napi_ok) return std::string();
        return std::string(buffer.get(), outSize);
    }
    return std::string();
}

std::string getAnsi(napi_env env, napi_value value)
{
    return ult::Utf8ToAnsi(getUtf8(env, value));
}

napi_value conv(napi_env env, napi_callback_info info)
{
    napi_status status;
    do
    {
        const size_t argvNum = 4;
        size_t argc = argvNum;
        napi_value argv[argvNum];
        status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
        if (status != napi_ok || argc < argvNum)
        {
            napi_throw_type_error(env, "ERR_INVALID_ARG_TYPE", "Wrong number of arguments");
            break;
        }
        std::string src = getAnsi(env, argv[0]);
        std::string dest = getAnsi(env, argv[1]);
        std::string type = getAnsi(env, argv[2]);
        auto worker = new ConvWorker(env, argv[3], src, dest, type);
    } while (false);

    napi_value value;
    napi_get_undefined(env, &value);
    return value;
}

napi_value Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_property_descriptor desc[] = {
        { "conv", nullptr, conv, nullptr, nullptr, nullptr, napi_default, nullptr },
    };
    status = napi_define_properties(env, exports, 1, desc);
    if (status != napi_ok) return nullptr;
    return exports;
}

NAPI_MODULE(msoconv, Init)