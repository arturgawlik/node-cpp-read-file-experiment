#include "file_reader.h"
#include <uv.h>
#include <iostream>

using namespace Napi;

file_reader_data *fileReaderData;

static uv_buf_t uvBuf;
static char dataBuf[64];

static uv_fs_t openReq;
static uv_fs_t readReq;
static uv_fs_t closeReq;

static Napi::HandleScope create_handle_scope(Napi::Env env)
{
    /*
        This creates new handle scope.
        It's necessery when creating some JavaScript accessed objects.
        For example creating string that will be available from JS.
        HandleScope need's to be available on stack (it's need to be returned from this function) otherwise it will be available for Scope's.
    */
    return Napi::HandleScope(env);
}

FileReader::FileReader(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
    // std::cout << "FileReader:FileReader - start\n";
    Napi::Env env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "File path must be provided").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "File path is must be a string").ThrowAsJavaScriptException();
        return;
    }

    this->_filePath = info[0].As<Napi::String>().Utf8Value();
    // std::cout << "FileReader:FileReader - end\n";
}

static void on_read(uv_fs_t *req)
{
    // std::cout << "on_read - start\n";
    Napi::HandleScope scope = create_handle_scope(fileReaderData->callback.Env());

    if (req->result < 0)
    {
        Napi::Error::New(fileReaderData->callback.Env(), "Error when reading file.").ThrowAsJavaScriptException();
        return;
    }
    else if (req->result == 0)
    {
        uv_fs_t closeReq;
        uv_fs_close(fileReaderData->eventLoop, &closeReq, req->result, NULL);
    }
    else
    {
        // std::cout << "on_read - before callback\n";
        fileReaderData->callback.MakeCallback(fileReaderData->callback.Env().Global(), {Napi::String::New(fileReaderData->callback.Env(), dataBuf)});

        // schedule next chunk read
        uv_fs_read(fileReaderData->eventLoop, &readReq, openReq.result, &uvBuf, 1, -1, on_read);
    }
    // std::cout << "on_read - end\n";
}

static void on_open(uv_fs_t *req)
{
    // std::cout << "on_open - start\n";
    Napi::HandleScope scope = create_handle_scope(fileReaderData->callback.Env());

    if (req->result < 0)
    {
        Napi::Error::New(fileReaderData->callback.Env(), "Error when openning file.").ThrowAsJavaScriptException();
        return;
    }
    else
    {
        uvBuf = uv_buf_init(dataBuf, sizeof(dataBuf));

        uv_fs_read(fileReaderData->eventLoop, &readReq, req->result, &uvBuf, 1, -1, on_read);
    }
    // std::cout << "on_open - end\n";
}

void FileReader::Read(const Napi::CallbackInfo &info)
{
    // std::cout << "FileReader:Read - start\n";
    Napi::Env env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Callback must be provided").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsFunction())
    {
        Napi::TypeError::New(env, "Callback must be a function").ThrowAsJavaScriptException();
        return;
    }

    Napi::Function callback = info[0].As<Napi::Function>();

    uv_loop_t *eventLoop;
    napi_status status = napi_get_uv_event_loop(env, &eventLoop);
    NAPI_THROW_IF_FAILED(env, status);

    fileReaderData = (file_reader_data *)malloc(sizeof(file_reader_data));
    fileReaderData->callback = Napi::Persistent(callback);
    fileReaderData->eventLoop = eventLoop;

    int fd = uv_fs_open(eventLoop, &openReq, this->_filePath.c_str(), O_RDONLY, 0, on_open);
    // std::cout << "FileReader:Read - end\n";
}

Napi::Function FileReader::GetClass(Napi::Env env)
{
    return DefineClass(env, "FileReader", {
                                              FileReader::InstanceMethod("read", &FileReader::Read),
                                          });
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    // std::cout << "before first string\n";
    Napi::String name = Napi::String::New(env, "FileReader");
    exports.Set(name, FileReader::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
