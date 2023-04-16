#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libraries/Tree/ArrayTree.h"
#include "DataAssets/RpyScript.h"

class IRpyInterpreter;
class URpySession;
enum class RpyInstructionType : uint8
{
    None = 0,
    Base = 1 << 0,
    If = 1 << 1,
    Else = 1 << 2,
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
    RpyInstruction(URpyScript *script, FRpyLine *rpyLine) : script(script), rpyLine(rpyLine){};
    virtual ~RpyInstruction() = default;
    virtual RpyInstructionType Type() const
    {
        return RpyInstructionType::Base;
    }
    virtual bool Compile()
    {
        return true;
    }
    virtual bool Execute(URpySession *session, bool &autoExecuteNext)
    {
        UE_LOG(LogTemp, Display, TEXT("Executing rpy:%s"), (*this->rpyLine->line));
        autoExecuteNext=true;
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
    FRpyLine *rpyLine = nullptr;

    RpyInstruction *parent = nullptr;
    TArray<RpyInstruction *> children;
    RpyInstruction *next = nullptr;
};
