#include <napi.h>

class FileReader : public Napi::ObjectWrap<FileReader>
{
public:
    FileReader(const Napi::CallbackInfo &);
    void Read(const Napi::CallbackInfo &);

private:
    std::string _filePath;
};
