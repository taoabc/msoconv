/*
* AsyncBase class for n-api
*
* @author taoabc@gmail.com
*/
#pragma once
#include <memory>
#include <node_api.h>

class AsyncBase
{
    // static
private:
    static void execute(napi_env env, void* data)
    {
        AsyncBase* self = static_cast<AsyncBase*>(data);
        self->execute(env);
    }

    static void complete(napi_env env, napi_status status, void* data)
    {
        std::unique_ptr<AsyncBase> self(static_cast<AsyncBase*>(data));
        self->complete(env, status);
    }

public:
    // we need env to do everything
    explicit AsyncBase(napi_env env, napi_value callback, const char* resourceName) : env_(env)
    {
        if (persistCallback(callback)) {
            load(resourceName);
        }
    }

    ~AsyncBase()
    {
        if (work_ != nullptr)
        {
            napi_delete_async_work(env_, work_);
        }
        if (ref_ != nullptr)
        {
            napi_delete_reference(env_, ref_);
        }
    }

protected:
    virtual void execute(napi_env env) = 0; // run in work thread
    
    virtual void complete(napi_env env, napi_status status) = 0;

    napi_status callback(size_t argc, const napi_value* argv, napi_value* result)
    {
        napi_status status;
        do
        {
            napi_value global;
            status = napi_get_global(env_, &global);
            if (status != napi_ok) break;
            napi_value callback;
            status = napi_get_reference_value(env_, ref_, &callback);
            if (status != napi_ok) break;
            status = napi_call_function(env_, global, callback, argc, argv, result);
        } while (false);
        return status;
    }

private:
    // if load failed£¬then will never get destroyed, so we must destroy self when load failed
    bool load(const char* str)
    {
        napi_status status = napi_create_string_latin1(env_, str, NAPI_AUTO_LENGTH, &resourceName_);
        if (status == napi_ok) return load(resourceName_);
        return false;
    }

    bool load(const char16_t* str)
    {
        napi_status status = napi_create_string_utf16(env_, str, NAPI_AUTO_LENGTH, &resourceName_);
        if (status == napi_ok) return load(resourceName_);
        return false;
    }

    bool load(napi_value resourceName)
    {
        napi_status status = napi_create_async_work(env_, nullptr, resourceName, AsyncBase::execute, AsyncBase::complete, this, &work_);
        if (status == napi_ok)
        {
            status = napi_queue_async_work(env_, work_);
        }
        return status == napi_ok;
    }

    bool persistCallback(napi_value callback)
    {
        napi_status status = napi_create_reference(env_, callback, 1, &ref_);
        if (status != napi_ok)
        {
            napi_throw_error(env_, "ERR_UNKNOWN", "Create reference error");
            return false;
        }
        return true;
    }

// member data
protected:
    napi_env env_;

private:
    napi_value resourceName_;
    napi_async_work work_ = nullptr;
    napi_ref ref_ = nullptr;
};
