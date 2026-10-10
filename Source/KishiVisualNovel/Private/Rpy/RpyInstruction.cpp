#include "Rpy/RpyInstruction.h"

namespace
{
	TMap<FName, FRpyInstructionFactory>& InstructionFactories()
	{
		static TMap<FName, FRpyInstructionFactory> Factories;
		return Factories;
	}
}

void FRpyInstructionRegistry::Register(FName Type, FRpyInstructionFactory Factory)
{
	if (!Type.IsNone() && Factory)
	{
		InstructionFactories().Add(Type, MoveTemp(Factory));
	}
}

bool FRpyInstructionRegistry::IsRegistered(FName Type)
{
	return InstructionFactories().Contains(Type);
}

RpyInstruction* FRpyInstructionRegistry::Create(FName Type, URpyScript* Script, FRpyLine* SourceLine, const TMap<FName, FRpyInstructionProp>& Required, const FRpyInstructionProps& Props)
{
	if (FRpyInstructionFactory* Factory = InstructionFactories().Find(Type))
	{
		return (*Factory)(Script, SourceLine, Required, Props);
	}
	return nullptr;
}
