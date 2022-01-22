#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libraries/Tree/ArrayTree.h"

class IRpyInterpreter;
class URpyScript;
struct FRpyLine;

class RpyInstruction
{
public:
    RpyInstruction(URpyScript *script,FRpyLine *rpyLine) : script(script),rpyLine(rpyLine){};
    virtual ~RpyInstruction();
    virtual bool Compile();
    virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) = 0;
    virtual RpyInstruction *GetNext(const TScriptInterface<IRpyInterpreter> &interpreter);

    //
    URpyScript *script;
    FRpyLine *rpyLine;

    RpyInstruction *parent;
    TArray<RpyInstruction *> children;
    RpyInstruction *next;
};
