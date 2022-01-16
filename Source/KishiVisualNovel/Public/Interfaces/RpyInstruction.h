#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RpyInstruction.generated.h"

class IRpyInterpreter;

UINTERFACE(BlueprintType) class KISHIVISUALNOVEL_API URpyInstruction : public UInterface
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyInstruction
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Execute(const TScriptInterface<IRpyInterpreter> &GameMode);
    virtual bool Execute_Implementation(const TScriptInterface<IRpyInterpreter> &GameMode) = 0;
    // virtual TArray<FName> GetSupportedExpressions_Implementation()const=0;
};

UCLASS()
class KISHIVISUALNOVEL_API URpyInstructionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    // UFUNCTION(BlueprintPure)
    // static bool HasExpression(const TScriptInterface<IRpyInstruction>& Target,FName Expression);
protected:
private:
};
