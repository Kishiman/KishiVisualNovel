#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libraries/Tree/ArrayTree.h"

class IRpyInterpreter;
class URpyScript;

class RpyInstruction
{
public:
    RpyInstruction(){};
    virtual ~RpyInstruction();
    virtual bool Compile();
    virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) = 0;
    virtual RpyInstruction *GetNext(const TScriptInterface<IRpyInterpreter> &interpreter) = 0;

    //
    URpyScript *script;
    RpyInstruction *parent;
    TArray<RpyInstruction *> children;
    RpyInstruction *next;
};
