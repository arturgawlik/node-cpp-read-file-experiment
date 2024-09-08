#pragma once

#include <napi.h>

class CppReadFileExperiment : public Napi::ObjectWrap<CppReadFileExperiment>
{
public:
    CppReadFileExperiment(const Napi::CallbackInfo&);
    Napi::Value Greet(const Napi::CallbackInfo&);

    static Napi::Function GetClass(Napi::Env);

private:
    std::string _greeterName;
};
