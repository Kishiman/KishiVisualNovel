#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Rpy/RpyInstructionProps.h"
#include "RpyInstructionPropsLibrary.generated.h"

/** Blueprint accessors for the C++ FRpyInstructionProps value type. */
UCLASS()
class KISHIVISUALNOVEL_API URpyInstructionPropsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Visual Novel|Script|Props")
	static bool GetString(const FRpyInstructionProps& Props, FName Name, FString& OutValue);

	UFUNCTION(BlueprintPure, Category = "Visual Novel|Script|Props")
	static bool GetBool(const FRpyInstructionProps& Props, FName Name, bool& OutValue);

	UFUNCTION(BlueprintPure, Category = "Visual Novel|Script|Props")
	static bool GetFloat(const FRpyInstructionProps& Props, FName Name, float& OutValue);

	UFUNCTION(BlueprintPure, Category = "Visual Novel|Script|Props")
	static bool GetInteger(const FRpyInstructionProps& Props, FName Name, int32& OutValue);

	UFUNCTION(BlueprintPure, Category = "Visual Novel|Script|Props")
	static bool GetName(const FRpyInstructionProps& Props, FName Name, FName& OutValue);

	UFUNCTION(BlueprintPure, Category = "Visual Novel|Script|Props")
	static bool GetAsset(const FRpyInstructionProps& Props, FName Name, FString& OutValue);

	UFUNCTION(BlueprintPure, Category = "Visual Novel|Script|Props")
	static bool GetVector(const FRpyInstructionProps& Props, FName Name, FVector& OutValue);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Script|Props")
	static void SetString(UPARAM(ref) FRpyInstructionProps& Props, FName Name, const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Script|Props")
	static void SetBool(UPARAM(ref) FRpyInstructionProps& Props, FName Name, bool Value);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Script|Props")
	static void SetFloat(UPARAM(ref) FRpyInstructionProps& Props, FName Name, float Value);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Script|Props")
	static void SetInteger(UPARAM(ref) FRpyInstructionProps& Props, FName Name, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Script|Props")
	static void SetName(UPARAM(ref) FRpyInstructionProps& Props, FName Name, FName Value);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Script|Props")
	static void SetAsset(UPARAM(ref) FRpyInstructionProps& Props, FName Name, const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Script|Props")
	static void SetVector(UPARAM(ref) FRpyInstructionProps& Props, FName Name, FVector Value);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Script|Props")
	static bool Remove(UPARAM(ref) FRpyInstructionProps& Props, FName Name);

	UFUNCTION(BlueprintPure, Category = "Visual Novel|Script|Props")
	static FRpyInstructionProps Clone(const FRpyInstructionProps& Props);
};
