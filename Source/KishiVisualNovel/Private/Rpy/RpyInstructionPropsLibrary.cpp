#include "Rpy/RpyInstructionPropsLibrary.h"

bool URpyInstructionPropsLibrary::GetString(const FRpyInstructionProps& Props, FName Name, FString& OutValue) { return Props.GetString(Name, OutValue); }
bool URpyInstructionPropsLibrary::GetBool(const FRpyInstructionProps& Props, FName Name, bool& OutValue) { return Props.GetBool(Name, OutValue); }
bool URpyInstructionPropsLibrary::GetFloat(const FRpyInstructionProps& Props, FName Name, float& OutValue) { return Props.GetFloat(Name, OutValue); }
bool URpyInstructionPropsLibrary::GetInteger(const FRpyInstructionProps& Props, FName Name, int32& OutValue) { return Props.GetInteger(Name, OutValue); }
bool URpyInstructionPropsLibrary::GetName(const FRpyInstructionProps& Props, FName Name, FName& OutValue) { return Props.GetName(Name, OutValue); }
bool URpyInstructionPropsLibrary::GetAsset(const FRpyInstructionProps& Props, FName Name, FString& OutValue) { return Props.GetAsset(Name, OutValue); }
bool URpyInstructionPropsLibrary::GetVector(const FRpyInstructionProps& Props, FName Name, FVector& OutValue) { return Props.GetVector(Name, OutValue); }

void URpyInstructionPropsLibrary::SetString(FRpyInstructionProps& Props, FName Name, const FString& Value) { Props.SetString(Name, Value); }
void URpyInstructionPropsLibrary::SetBool(FRpyInstructionProps& Props, FName Name, bool Value) { Props.SetBool(Name, Value); }
void URpyInstructionPropsLibrary::SetFloat(FRpyInstructionProps& Props, FName Name, float Value) { Props.SetFloat(Name, Value); }
void URpyInstructionPropsLibrary::SetInteger(FRpyInstructionProps& Props, FName Name, int32 Value) { Props.SetInteger(Name, Value); }
void URpyInstructionPropsLibrary::SetName(FRpyInstructionProps& Props, FName Name, FName Value) { Props.SetName(Name, Value); }
void URpyInstructionPropsLibrary::SetAsset(FRpyInstructionProps& Props, FName Name, const FString& Value) { Props.SetAsset(Name, Value); }
void URpyInstructionPropsLibrary::SetVector(FRpyInstructionProps& Props, FName Name, FVector Value) { Props.SetVector(Name, Value); }
bool URpyInstructionPropsLibrary::Remove(FRpyInstructionProps& Props, FName Name) { return Props.Remove(Name); }
FRpyInstructionProps URpyInstructionPropsLibrary::Clone(const FRpyInstructionProps& Props) { return Props.Clone(); }
