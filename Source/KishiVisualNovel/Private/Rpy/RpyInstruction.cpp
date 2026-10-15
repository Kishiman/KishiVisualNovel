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

RpyInstruction* FRpyInstructionRegistry::Create(FName Type, URpyScript* Script, FRpyLine* SourceLine, const TMap<FName, FRpyInstructionProp>& Required, const FRpyInstructionProps& Props)
{
	if (FRegisteredInstruction* Registered = InstructionFactories().Find(Type))
	{
		return Registered->Factory(Script, SourceLine, Required, Props);
	}
	return nullptr;
}
