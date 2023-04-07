#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libraries/Tree/ArrayTree.h"
#include "DataAssets/RpyScript.h"

class IRpyInterpreter;

class RpyInstruction
{
public:
    RpyInstruction(URpyScript* script, FRpyLine* rpyLine) : script(script), rpyLine(rpyLine) {};
    virtual ~RpyInstruction() = default;
    virtual bool Compile() {
        return true;
    }
    virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
        UE_LOG(LogTemp, Display, TEXT("Executing rpy:%s"), (*this->rpyLine->line));
        return true;
    };
    virtual RpyInstruction* GetNext(const TScriptInterface<IRpyInterpreter>& interpreter) {
        return GetNextInTree();
    };
    RpyInstruction* GetNextInTree() {
        return this->children.Num()>0?children[0]: this->next;
    };
    //
    URpyScript* script = nullptr;
    FRpyLine* rpyLine = nullptr;

    RpyInstruction* parent = nullptr;
    TArray<RpyInstruction*> children;
    RpyInstruction* next = nullptr;
};
