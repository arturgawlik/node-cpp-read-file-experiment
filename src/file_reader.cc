#include "file_reader.h"
#include <uv.h>

using namespace Napi;

static uv_buf_t uvBuf;
static char dataBuf[64];

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

    file_reader_data *fileReaderData = (file_reader_data *)malloc(sizeof(file_reader_data));
    fileReaderData->callback = Napi::Persistent(callback);
    fileReaderData->eventLoop = eventLoop;
    fileReaderData->env = env;

    uv_fs_t openReq;
    openReq.data = fileReaderData;

    int fd = uv_fs_open(eventLoop, &openReq, this->_filePath.c_str(), O_RDONLY, 0, on_open);
}

static void on_open(uv_fs_t *req)
{
    const file_reader_data *fileReaderData = (file_reader_data *)req->data;

    if (req->result < 0)
    {
        Napi::Error::New(fileReaderData->env, "Error when openning file.").ThrowAsJavaScriptException();
        return;
    }
    else
    {
        uvBuf = uv_buf_init(dataBuf, sizeof(dataBuf));

        uv_fs_t readReq;

        // TODO: not sure this is correct
        readReq.data = &fileReaderData;

        uv_fs_read(fileReaderData->eventLoop, &readReq, req->result, &uvBuf, 1, -1, on_read);
    }
}

static void on_read(uv_fs_t *req)
{
    // TODO
}
