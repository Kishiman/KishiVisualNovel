#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libraries/Tree/ArrayTree.h"

#include "RpyInstruction.generated.h"

class IRpyInterpreter;
class URpyScript;

class KISHIVISUALNOVEL_API RpyInstructionBase : public IArrayTree
{
public:
    RpyInstructionBase(URpyScript* script)script(script){};
    virtual ~RpyInstructionBase();
    virtual bool Compile();
    virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) = 0;
    virtual RpyInstructionBase* GetNext(const TScriptInterface<IRpyInterpreter> &interpreter) = 0;

    // IArrayTree
    virtual TScriptInterface<IBaseTree> GetParentTree_Implementation() const override;
    virtual TArray<TScriptInterface<IBaseTree>> GetDirectChildren_Implementation() const override;
    virtual TScriptInterface<IArrayTree> GetChild_Implementation(uint8 index) const override;

    //
    URpyScript* script;
    RpyInstructionBase* parent;
    TArray<RpyInstructionBase*> children;
    RpyInstructionBase* next;
};
