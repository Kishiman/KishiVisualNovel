#include "Managers/Statement/RpyStatementManager.h"

#include "Player/RpyScriptInterpreter.h"
#include "Save/BaseRpyGameInstance.h"

namespace
{
	/** The manager's interpreter, or null after logging why there is none. */
	UObject *GetInterpreterObject(const TScriptInterface<IRpyStatementManager> &Target, const TCHAR *Purpose)
	{
		UObject *Manager = Target.GetObject();
		if (!IsValid(Manager))
		{
			UE_LOG(LogTemp, Error, TEXT("A statement manager is required to %s."), Purpose);
			return nullptr;
		}
		UObject *InterpreterObject = IRpyScriptInterpreterBaseManager::Execute_GetInterpreter(Manager).GetObject();
		if (IsValid(InterpreterObject) && InterpreterObject->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
			return InterpreterObject;
		UE_LOG(LogTemp, Error, TEXT("%s: no interpreter to %s; the VN player sets it on its managers."), *Manager->GetName(), Purpose);
		return nullptr;
	}
}

FString URpyStatementManagerLibrary::GetSpeakerDisplayName(FName Name)
{
	// FName(NAME_None).ToString() is "None", which would be shown as the speaker of an unattributed line.
	return Name.IsNone() ? FString() : Name.ToString();
}

bool URpyStatementManagerLibrary::ToggleSaveMenu(const TScriptInterface<IRpyStatementManager> &Target)
{
	UObject *InterpreterObject = GetInterpreterObject(Target, TEXT("open the save menu"));
	if (!InterpreterObject)
		return false;
	IRpyScriptInterpreter::Execute_SetSaveMenuVisible(InterpreterObject, !IRpyScriptInterpreter::Execute_GetSaveMenuVisible(InterpreterObject));
	return true;
}

bool URpyStatementManagerLibrary::ToggleLoadMenu(const TScriptInterface<IRpyStatementManager> &Target)
{
	UObject *InterpreterObject = GetInterpreterObject(Target, TEXT("open the load menu"));
	if (!InterpreterObject)
		return false;
	IRpyScriptInterpreter::Execute_SetLoadMenuVisible(InterpreterObject, !IRpyScriptInterpreter::Execute_GetLoadMenuVisible(InterpreterObject));
	return true;
}

bool URpyStatementManagerLibrary::QuickSave(UObject *WorldContextObject)
{
	UBaseRpyGameInstance *GameInstance = UBaseRpyGameInstance::GetBaseRpyGameInstance(WorldContextObject);
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot quick save, the Game Instance Class must derive from BaseRpyGameInstance."), *GetNameSafe(WorldContextObject));
		return false;
	}
	return GameInstance->QuickSave();
}

bool URpyStatementManagerLibrary::QuickLoad(UObject *WorldContextObject)
{
	UBaseRpyGameInstance *GameInstance = UBaseRpyGameInstance::GetBaseRpyGameInstance(WorldContextObject);
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot quick load, the Game Instance Class must derive from BaseRpyGameInstance."), *GetNameSafe(WorldContextObject));
		return false;
	}
	GameInstance->QuickLoad();
	return true;
}

void URpyStatementManagerLibrary::EmitDialogueViseme(const TScriptInterface<IRpyStatementManager> &Target, FName Speaker, const FTaggedString &Statement, int32 PreviousCount,
													 int32 Count, float TextSpeed, FMouthViseme &LastViseme)
{
	// Keep the last spoken viseme through Neutral characters, so the mouth holds its shape until the speaker has been silent for a while.
	const FMouthViseme Viseme = UStringUtils::getVisemeBetween(Statement, PreviousCount, Count);
	if (Viseme.type != EMouthViseme::Neutral)
	{
		LastViseme = Viseme;
	}
	// The offset is the time since the last spoken viseme; a fully revealed line reports a long silence.
	const bool bLineComplete = Count >= Statement.Length;
	const float TimeOffset = bLineComplete ? 60.0f : (TextSpeed > 0.0f ? FMath::Max(0, Count - LastViseme.taggedIndex) / TextSpeed : 60.0f);
	UObject *Manager = Target.GetObject();
	if (!IsValid(Manager))
		return;
	UObject *InterpreterObject = IRpyScriptInterpreterBaseManager::Execute_GetInterpreter(Manager).GetObject();
	if (IsValid(InterpreterObject) && InterpreterObject->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
	{
		IRpyScriptInterpreter::Execute_OnDialogueViseme(InterpreterObject, Speaker, Viseme.type, TimeOffset);
	}
}
