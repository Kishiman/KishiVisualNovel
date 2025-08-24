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
    TMap<FName, FName> names;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, FString> strings;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, int> ints;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, float> floats;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, FVector> vectors;

    static void Assign(FDynamicObject &To, const FDynamicObject &From)
    {
        for (auto pair : From.bools)
            To.bools.Add(pair);
        for (auto pair : From.names)
            To.names.Add(pair);
    }
    static void AssignIfNotExist(FDynamicObject &To, const FDynamicObject &From)
    {
        for (auto pair : From.bools)
            if (!To.bools.Contains(pair.Key))
                To.bools.Add(pair);
        for (auto pair : From.names)
            if (!To.names.Contains(pair.Key))
                To.names.Add(pair);
    }

    void setValue(const FName &name, bool value)
    {
        if (bools.Contains(name))
            bools[name] = value;
        else
            bools.Add(name, value);
    }
    void setValue(const FName &name, FName value)
    {
        if (names.Contains(name))
            names[name] = value;
        else
            names.Add(name, value);
    }
    void setValue(const FName &name, FString value)
    {
        if (strings.Contains(name))
            strings[name] = value;
        else
            strings.Add(name, value);
    }
    void setValue(const FName &name, int value)
    {
        if (ints.Contains(name))
            ints[name] = value;
        else
            ints.Add(name, value);
    }
    void setValue(const FName &name, float value)
    {
        if (floats.Contains(name))
            floats[name] = value;
        else
            floats.Add(name, value);
    }
    void setValue(const FName &name, FVector value)
    {
        if (vectors.Contains(name))
            vectors[name] = value;
        else
            vectors.Add(name, value);
    }
};