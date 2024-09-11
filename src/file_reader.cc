#include "file_reader.h"
#include <uv.h>

using namespace Napi;

file_reader_data *fileReaderData;

static uv_buf_t uvBuf;
static char dataBuf[64];

static uv_fs_t openReq;
static uv_fs_t readReq;
static uv_fs_t closeReq;

FileReader::FileReader(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
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
}

void FileReader::Read(const Napi::CallbackInfo &info)
{
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
    fileReaderData->env = env;

    // uv_fs_t *openReq = (uv_fs_t *)malloc(sizeof(uv_fs_t));
    // openReq.data = fileReaderData;

    int fd = uv_fs_open(eventLoop, &openReq, this->_filePath.c_str(), O_RDONLY, 0, on_open);
}

static void on_open(uv_fs_t *req)
{
    // const file_reader_data *fileReaderData = (file_reader_data *)req->data;

    if (req->result < 0)
    {
        Napi::Error::New(fileReaderData->env, "Error when openning file.").ThrowAsJavaScriptException();
        return;
    }
    else
    {
        uvBuf = uv_buf_init(dataBuf, sizeof(dataBuf));

        // uv_fs_t readReq;

        // TODO: not sure this is correct
        // readReq.data = &fileReaderData;

        uv_fs_read(fileReaderData->eventLoop, &readReq, req->result, &uvBuf, 1, -1, on_read);
    }
}

static void on_read(uv_fs_t *req)
{
    // const file_reader_data *fileReaderData = (file_reader_data *)req->data;

    if (req->result < 0)
    {
        Napi::Error::New(fileReaderData->env, "Error when reading file.").ThrowAsJavaScriptException();
        return;
    }
    else if (req->result == 0)
    {
        uv_fs_t closeReq;
        uv_fs_close(fileReaderData->eventLoop, &closeReq, req->result, NULL);
    }
    else
    {
        // TODO: implement send actual file chunk
        fileReaderData->callback.MakeCallback(fileReaderData->env.Global(), {Napi::String::New(fileReaderData->env, "file success read...")});
    }
}

Napi::Function FileReader::GetClass(Napi::Env env)
{
    return DefineClass(env, "FileReader", {
                                              FileReader::InstanceMethod("read", &FileReader::Read),
                                          });
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::String name = Napi::String::New(env, "FileReader");
    exports.Set(name, FileReader::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
