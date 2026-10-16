#include "Rpy/RpyInstruction.h"

namespace
{
	struct FRegisteredInstruction
	{
		FRpyInstructionSchema Schema;
		FRpyInstructionFactory Factory;
	};

	TMap<FName, FRegisteredInstruction>& InstructionFactories()
	{
		static TMap<FName, FRegisteredInstruction> Factories;
		return Factories;
	}
}

void FRpyInstructionRegistry::Register(FName Type, const FRpyInstructionSchema& Schema, FRpyInstructionFactory Factory)
{
	if (!Type.IsNone() && Factory)
	{
		FRegisteredInstruction Registered;
		Registered.Schema = Schema;
		Registered.Factory = MoveTemp(Factory);
		InstructionFactories().Add(Type, MoveTemp(Registered));
	}
}

bool FRpyInstructionRegistry::IsRegistered(FName Type)
{
	return InstructionFactories().Contains(Type);
}

const FRpyInstructionSchema* FRpyInstructionRegistry::FindSchema(FName Type)
{
	const FRegisteredInstruction* Registered = InstructionFactories().Find(Type);
	return Registered ? &Registered->Schema : nullptr;
}

URpyInstruction* FRpyInstructionRegistry::Create(FName Type, URpyScript* Script, const TMap<FName, FRpyInstructionProp>& Required, const FRpyInstructionProps& Props)
{
	if (FRegisteredInstruction* Registered = InstructionFactories().Find(Type))
	{
		return Registered->Factory(Script, Required, Props);
	}
	return nullptr;
}

FString URpyInstruction::FormatInstruction(const FString& TypeName, const FString& RequiredFields) const
{
	FString Result = TypeName;
	if (!RequiredFields.IsEmpty()) Result += TEXT(" ") + RequiredFields;
	for (const FRpyInstructionProp& Prop : Props.Values)
		Result += FString::Printf(TEXT(" %s=\"%s\""), *Prop.Name.ToString(), *Prop.Value);
	return Result;
}

FString URpyInstruction::ToString() const
{
	return FormatInstruction(InstructionType.IsNone() ? GetClass()->GetName() : InstructionType.ToString());
}
