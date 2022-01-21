
#include "C++/RpyInstructions.h"
#include "C++/RpyCompilers.h"

SayCompiler::SayCompiler()
{
    this->query = std::regex("^\"(.+)\" \"(.*)\"$");
};
RpyInstruction *SayCompiler::GetRpyInstruction(URpyScript *script, FRpyLine line, TArray<FString> params)
{
    return new SayInstruction(FName(*params[1]), params[2]);
};
