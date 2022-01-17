#pragma once

#include "CoreMinimal.h"

#include "Params.generated.h"


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
struct FParams
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSet<FName> NameFlags;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName,bool> Booleans;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName,FName> Names;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName,int> Integers;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName,float> Floats;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName,FVector> Vectors;
};