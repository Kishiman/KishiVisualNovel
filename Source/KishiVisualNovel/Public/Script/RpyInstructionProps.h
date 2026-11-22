#pragma once

#include "CoreMinimal.h"
#include "RpyInstructionProps.generated.h"

/** Optional, typed extension properties shared by every XML instruction. */
USTRUCT(BlueprintType)
struct KISHIVISUALNOVEL_API FRpyInstructionProp
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Novel|Script")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Novel|Script")
	FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Novel|Script")
	FString Value;
};

/** Optional instruction data. Required command arguments do not belong here. */
USTRUCT(BlueprintType)
struct KISHIVISUALNOVEL_API FRpyInstructionProps
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Novel|Script")
	TArray<FRpyInstructionProp> Values;

	bool GetString(FName Name, FString& OutValue) const;

	bool GetBool(FName Name, bool& OutValue) const;

	bool GetFloat(FName Name, float& OutValue) const;

	bool GetInteger(FName Name, int32& OutValue) const;

	bool GetName(FName Name, FName& OutValue) const;

	bool GetAsset(FName Name, FString& OutValue) const;

	bool GetVector(FName Name, FVector& OutValue) const;

	void SetString(FName Name, const FString& Value);

	void SetBool(FName Name, bool Value);

	void SetFloat(FName Name, float Value);

	void SetInteger(FName Name, int32 Value);

	void SetName(FName Name, FName Value);

	void SetAsset(FName Name, const FString& Value);

	void SetVector(FName Name, FVector Value);

	bool Remove(FName Name);

	FRpyInstructionProps Clone() const;

private:
	const FRpyInstructionProp* Find(FName Name) const;
	FRpyInstructionProp& FindOrAdd(FName Name, const TCHAR* Type);
};
