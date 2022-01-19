#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libraries/Tree/ArrayTree.h"

class IRpyInterpreter;
class URpyScript;

class RpyInstructionBase
{
public:
    RpyInstructionBase(){};
    virtual ~RpyInstructionBase();
    virtual bool Compile();
    virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) = 0;
    virtual RpyInstructionBase *GetNext(const TScriptInterface<IRpyInterpreter> &interpreter) = 0;

    //
    URpyScript *script;
    RpyInstructionBase *parent;
    TArray<RpyInstructionBase *> children;
    RpyInstructionBase *next;
};
