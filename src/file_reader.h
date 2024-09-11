#include <napi.h>

class FileReader : public Napi::ObjectWrap<FileReader>
{
public:
    FileReader(const Napi::CallbackInfo &);
    void Read(const Napi::CallbackInfo &);

    static Napi::Function GetClass(Napi::Env);

private:
    std::string _filePath;
};

struct file_reader_data
{
    Napi::FunctionReference callback;
    uv_loop_s *eventLoop;
    Napi::Env env;
};