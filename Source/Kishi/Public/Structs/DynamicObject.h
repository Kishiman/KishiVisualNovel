#pragma once

#include "CoreMinimal.h"

#include "DynamicObject.generated.h"

USTRUCT(BlueprintType)
struct FIntegerParam
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int value;
};
USTRUCT(BlueprintType)
struct FFloatParam
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float value;
};
USTRUCT(BlueprintType)
struct FVectorParam
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector value;
};
USTRUCT(BlueprintType)
struct FBoolParam
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool value;
};

USTRUCT(BlueprintType)
struct FNameParam
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName value;
};

USTRUCT(BlueprintType)
struct FStringParam
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString value;
};

USTRUCT(BlueprintType)
struct FDynamicObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, bool> bools;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, FString> strings;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, int> ints;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, float> floats;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, FVector> vectors;
};