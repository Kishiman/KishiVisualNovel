#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libraries/Tree/ArrayTree.h"
#include "DataAssets/RpyScript.h"

class IRpyInterpreter;
class URpySession;

class RpyInstruction
{
public:
    RpyInstruction(URpyScript *script, FRpyLine *rpyLine) : script(script), rpyLine(rpyLine){};
    virtual ~RpyInstruction() = default;
    virtual bool Compile()
    {
        return true;
    }
    virtual bool Execute(URpySession *session)
    {
        UE_LOG(LogTemp, Display, TEXT("Executing rpy:%s"), (*this->rpyLine->line));
        return true;
    };
    virtual RpyInstruction *GetNext(URpySession *session)
    {
        return this->next;
    };
    //
    URpyScript *script = nullptr;
    FRpyLine *rpyLine = nullptr;

    RpyInstruction *parent = nullptr;
    TArray<RpyInstruction *> children;
    RpyInstruction *next = nullptr;
};
