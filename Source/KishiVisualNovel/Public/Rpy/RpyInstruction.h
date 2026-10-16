#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libraries/Tree/ArrayTree.h"
#include "Rpy/RpyScript.h"
#include "Rpy/RpyInstructionProps.h"

#include "RpyInstruction.generated.h"

class IRpyScriptInterpreter;
class URpySession;
enum class RpyInstructionType : uint8
{
    None = 0,
    Base = 1 << 0,
    If = 1 << 1,
    Else = 1 << 2,
    Menu = 1 << 3,
    Choice = 1 << 4,
};
inline RpyInstructionType operator|(RpyInstructionType a, RpyInstructionType b)
{
    return static_cast<RpyInstructionType>(static_cast<uint8>(a) | static_cast<uint8>(b));
};
inline RpyInstructionType operator&(RpyInstructionType a, RpyInstructionType b)
{
    return static_cast<RpyInstructionType>(static_cast<uint8>(a) | static_cast<uint8>(b));
};
constexpr bool operator!(RpyInstructionType value)
{
    return static_cast<bool>(static_cast<uint8>(value));
}

UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API URpyInstruction : public UObject
{
    GENERATED_BODY()

public:
    void Initialize(URpyScript *InScript)
    {
        script = InScript;
    }
    virtual FString ToString() const;
    virtual EInstructionRunTimeType RunTimeType() const
    {
        return EInstructionRunTimeType::NONE;
    }
    virtual RpyInstructionType Type() const
    {
        return RpyInstructionType::Base;
    }
    bool isOfType(RpyInstructionType type) const
    {
        uint8 myType = static_cast<uint8>(this->Type());
        return !!(myType & static_cast<uint8>(type));
    }
    virtual bool Compile()
    {
        return true;
    }
    virtual bool Execute(URpySession *session)
    {
        return true;
    };
    virtual URpyInstruction *GetNext(URpySession *session)
    {
        return GetNextInTree();
    };
    /** Returns all statically possible successors for graphing and analysis. */
    virtual TArray<URpyInstruction *> GetPossibleNextInstructions() const
    {
        TArray<URpyInstruction *> Result;
        if (URpyInstruction *Next = GetNextInTree())
            Result.Add(Next);
        return Result;
    }
    virtual TArray<FString> GetPossibleNextInstructionLabels() const
    {
        TArray<FString> Result;
        const TArray<URpyInstruction *> Successors = GetPossibleNextInstructions();
        for (int32 Index = 0; Index < Successors.Num(); ++Index)
            Result.Add(TEXT("next"));
        return Result;
    }
    virtual bool IsCallInstruction() const { return false; }
    virtual bool IsControlFlowBoundary() const { return false; }
    URpyInstruction *GetNextInTree() const
    {
        return this->children.Num() > 0 ? children[0] : this->next;
    };
    FString FormatInstruction(const FString& TypeName, const FString& RequiredFields = FString()) const;
    //
    UPROPERTY(Transient, BlueprintReadOnly, Category = "Instruction")
    URpyScript *script = nullptr;
    /** XML command name, used by generic instructions and diagnostics. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Instruction")
    FName InstructionType;

    /** Stable identifier from XML, used by save games and diagnostics. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Instruction")
    FName InstructionId;

    /** All command-specific optional values live here. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
    FRpyInstructionProps Props;

    UPROPERTY(Transient)
    URpyInstruction *parent = nullptr;
    UPROPERTY(Transient)
    TArray<URpyInstruction *> children;
    UPROPERTY(Transient)
    URpyInstruction *next = nullptr;
};

using FRpyInstructionFactory = TFunction<URpyInstruction*(URpyScript*, const TMap<FName, FRpyInstructionProp>&, const FRpyInstructionProps&)>;

struct FRpyInstructionSchema
{
	/** Required attribute names and their scalar XML types. */
	TMap<FName, FString> RequiredFields;
	/** Optional attributes exposed through FRpyInstructionProps and their scalar XML types. */
	TMap<FName, FString> OptionalProps;
};

/** C++ extension point for game-specific XML command types. */
class KISHIVISUALNOVEL_API FRpyInstructionRegistry
{
public:
	static void Register(FName Type, const FRpyInstructionSchema& Schema, FRpyInstructionFactory Factory);
    static bool IsRegistered(FName Type);
	static const FRpyInstructionSchema* FindSchema(FName Type);
    static URpyInstruction* Create(FName Type, URpyScript* Script, const TMap<FName, FRpyInstructionProp>& Required, const FRpyInstructionProps& Props);
};
