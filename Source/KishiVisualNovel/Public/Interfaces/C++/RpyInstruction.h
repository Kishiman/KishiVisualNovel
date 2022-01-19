#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libraries/Tree/ArrayTree.h"

#include "RpyInstruction.generated.h"

class IRpyInterpreter;

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class KISHIVISUALNOVEL_API URpyInstruction : public UArrayTree
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyInstruction
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) = 0;

    UFUNCTION(BlueprintCallable)
    virtual TScriptInterface<IRpyInstruction> GetNext(const TScriptInterface<IRpyInterpreter> &interpreter) = 0;
};
UCLASS()
class KISHIVISUALNOVEL_API URpyInstructionBase : public UObject, public IRpyInstruction, public IArrayTree
{
    GENERATED_BODY()
public:
    // IRpyInstruction
    virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
    virtual TScriptInterface<IRpyInstruction> GetNext(const TScriptInterface<IRpyInterpreter> &interpreter) override;

    // IArrayTree
    virtual TScriptInterface<IBaseTree> GetParentTree_Implementation() const override;
    virtual TArray<TScriptInterface<IBaseTree>> GetDirectChildren_Implementation() const override;
    virtual TScriptInterface<IArrayTree> GetChild_Implementation(uint8 index) const override;

    // compile
    virtual void Compile();

    //
    URpyInstructionBase* parent;
    TArray<URpyInstructionBase*> children;
    URpyInstructionBase* next;
};
UCLASS()
class KISHIVISUALNOVEL_API URpyInstructionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
protected:
private:
};
