#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libraries/Tree/ArrayTree.h"
#include "Rpy/RpyScript.h"
#include "Rpy/RpyInstructionProps.h"

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

class RpyInstruction
{
public:
    RpyInstruction(URpyScript *script, FRpyLine *rpyLine) : script(script), rpyLine(rpyLine ? *rpyLine : FRpyLine()) {};
    virtual ~RpyInstruction() = default;
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
    virtual RpyInstruction *GetNext(URpySession *session)
    {
        return GetNextInTree();
    };
    RpyInstruction *GetNextInTree()
    {
        return this->children.Num() > 0 ? children[0] : this->next;
    };
    //
    URpyScript *script = nullptr;
    /** Source/debug metadata owned by this runtime instruction. */
    FRpyLine rpyLine;

    /** Stable identifier from XML, used by save games and diagnostics. */
    FName InstructionId;

    /** All command-specific optional values live here. */
    FRpyInstructionProps Props;

    RpyInstruction *parent = nullptr;
    TArray<RpyInstruction *> children;
    RpyInstruction *next = nullptr;
};

using FRpyInstructionFactory = TFunction<RpyInstruction*(URpyScript*, FRpyLine*, const TMap<FName, FRpyInstructionProp>&, const FRpyInstructionProps&)>;

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
    static RpyInstruction* Create(FName Type, URpyScript* Script, FRpyLine* SourceLine, const TMap<FName, FRpyInstructionProp>& Required, const FRpyInstructionProps& Props);
};
