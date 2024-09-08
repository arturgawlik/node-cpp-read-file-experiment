#include "file_reader.h"
#include <uv.h>

using namespace Napi;

FileReader::FileReader(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "File path is required").ThrowAsJavaScriptException();
        return;
    }

    this->_filePath = info[0].As<Napi::String>().Utf8Value();
}

void FileReader::Read(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
}
