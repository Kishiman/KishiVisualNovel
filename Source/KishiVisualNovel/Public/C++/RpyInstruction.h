#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libraries/Tree/ArrayTree.h"
#include "DataAssets/RpyScript.h"

class IRpyInterpreter;

class RpyInstruction
{
public:
    RpyInstruction(URpyScript *script, FRpyLine *rpyLine) : script(script), rpyLine(rpyLine){};
    virtual ~RpyInstruction();
    virtual bool Compile();
    virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) = 0;
    virtual RpyInstruction *GetNext(const TScriptInterface<IRpyInterpreter> &interpreter);

    //
    URpyScript *script = nullptr;
    FRpyLine *rpyLine = nullptr;

    RpyInstruction *parent = nullptr;
    TArray<RpyInstruction *> children;
    RpyInstruction *next = nullptr;
};
