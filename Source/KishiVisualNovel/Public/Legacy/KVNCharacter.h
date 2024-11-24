#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KVNCharacter.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API UKVNCharacter : public UInterface
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IKVNCharacter
{
    GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    FName GetName()const;
    virtual FName GetName_Implementation()const=0;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    FText GetDisplayName()const;
    virtual FText GetDisplayName_Implementation()const=0;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    FName GetExpression()const;
    virtual FName GetExpression_Implementation()const=0;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    TArray<FName> GetSupportedExpressions()const;
    virtual TArray<FName> GetSupportedExpressions_Implementation()const=0;
};

UCLASS()
class KISHIVISUALNOVEL_API UKVNCharacterLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure)
    static bool HasExpression(const TScriptInterface<IKVNCharacter>& Target,FName Expression);
protected:
private:
};


