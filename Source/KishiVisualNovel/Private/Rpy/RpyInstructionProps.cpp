#include "Rpy/RpyInstructionProps.h"

const FRpyInstructionProp* FRpyInstructionProps::Find(FName Name) const
{
	return Values.FindByPredicate([Name](const FRpyInstructionProp& Prop) { return Prop.Name == Name; });
}

FRpyInstructionProp& FRpyInstructionProps::FindOrAdd(FName Name, const TCHAR* Type)
{
	if (FRpyInstructionProp* Existing = Values.FindByPredicate([Name](const FRpyInstructionProp& Prop) { return Prop.Name == Name; }))
	{
		Existing->Type = Type;
		return *Existing;
	}
	FRpyInstructionProp& Added = Values.AddDefaulted_GetRef();
	Added.Name = Name;
	Added.Type = Type;
	return Added;
}

bool FRpyInstructionProps::GetString(FName Name, FString& OutValue) const
{
	const FRpyInstructionProp* Prop = Find(Name);
	if (!Prop || (Prop->Type != TEXT("string") && Prop->Type != TEXT("name") && Prop->Type != TEXT("asset"))) return false;
	OutValue = Prop->Value;
	return true;
}

bool FRpyInstructionProps::GetBool(FName Name, bool& OutValue) const
{
	const FRpyInstructionProp* Prop = Find(Name);
	if (!Prop || Prop->Type != TEXT("bool")) return false;
	if (Prop->Value == TEXT("true")) { OutValue = true; return true; }
	if (Prop->Value == TEXT("false")) { OutValue = false; return true; }
	return false;
}

bool FRpyInstructionProps::GetFloat(FName Name, float& OutValue) const
{
	const FRpyInstructionProp* Prop = Find(Name);
	if (!Prop || (Prop->Type != TEXT("float") && Prop->Type != TEXT("integer"))) return false;
	OutValue = FCString::Atof(*Prop->Value);
	return true;
}

bool FRpyInstructionProps::GetInteger(FName Name, int32& OutValue) const
{
	const FRpyInstructionProp* Prop = Find(Name);
	if (!Prop || Prop->Type != TEXT("integer")) return false;
	OutValue = FCString::Atoi(*Prop->Value);
	return true;
}

bool FRpyInstructionProps::GetName(FName Name, FName& OutValue) const
{
	const FRpyInstructionProp* Prop = Find(Name);
	if (!Prop || Prop->Type != TEXT("name")) return false;
	OutValue = FName(*Prop->Value);
	return true;
}

bool FRpyInstructionProps::GetAsset(FName Name, FString& OutValue) const
{
	const FRpyInstructionProp* Prop = Find(Name);
	if (!Prop || Prop->Type != TEXT("asset")) return false;
	OutValue = Prop->Value;
	return true;
}

bool FRpyInstructionProps::GetVector(FName Name, FVector& OutValue) const
{
	const FRpyInstructionProp* Prop = Find(Name);
	if (!Prop || Prop->Type != TEXT("vector")) return false;
	TArray<FString> Parts;
	Prop->Value.ParseIntoArray(Parts, TEXT(","), true);
	if (Parts.Num() != 3) return false;
	OutValue.X = FCString::Atof(*Parts[0]);
	OutValue.Y = FCString::Atof(*Parts[1]);
	OutValue.Z = FCString::Atof(*Parts[2]);
	return true;
}

void FRpyInstructionProps::SetString(FName Name, const FString& Value) { FindOrAdd(Name, TEXT("string")).Value = Value; }
void FRpyInstructionProps::SetBool(FName Name, bool Value) { FindOrAdd(Name, TEXT("bool")).Value = Value ? TEXT("true") : TEXT("false"); }
void FRpyInstructionProps::SetFloat(FName Name, float Value) { FindOrAdd(Name, TEXT("float")).Value = FString::SanitizeFloat(Value); }
void FRpyInstructionProps::SetInteger(FName Name, int32 Value) { FindOrAdd(Name, TEXT("integer")).Value = FString::FromInt(Value); }
void FRpyInstructionProps::SetName(FName Name, FName Value) { FindOrAdd(Name, TEXT("name")).Value = Value.ToString(); }
void FRpyInstructionProps::SetAsset(FName Name, const FString& Value) { FindOrAdd(Name, TEXT("asset")).Value = Value; }
void FRpyInstructionProps::SetVector(FName Name, FVector Value) { FindOrAdd(Name, TEXT("vector")).Value = FString::Printf(TEXT("%f,%f,%f"), Value.X, Value.Y, Value.Z); }

bool FRpyInstructionProps::Remove(FName Name)
{
	const int32 Index = Values.IndexOfByPredicate([Name](const FRpyInstructionProp& Prop) { return Prop.Name == Name; });
	if (Index == INDEX_NONE) return false;
	Values.RemoveAt(Index);
	return true;
}

FRpyInstructionProps FRpyInstructionProps::Clone() const
{
	FRpyInstructionProps Result;
	Result.Values = Values;
	return Result;
}
