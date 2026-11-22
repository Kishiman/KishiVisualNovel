#include "Managers/Choice/RpyChoiceManager.h"

#include "Session/RpySession.h"

void URpyChoiceManagerLibrary::SetChoiceManagerState_Default(const TScriptInterface<IRpyChoiceManager> &Target, const FRpyChoiceManagerState &State)
{
	UObject *Manager = Target.GetObject();
	if (!IsValid(Manager))
	{
		UE_LOG(LogTemp, Error, TEXT("SetChoiceManagerState_Default requires a valid manager."));
		return;
	}
	IRpyChoiceManager::Execute_Menu(Manager, State.choices);
}

bool URpyChoiceManagerLibrary::Choose_Default(const TScriptInterface<IRpyChoiceManager> &Target, int32 Index)
{
	UObject *Manager = Target.GetObject();
	if (!IsValid(Manager))
	{
		UE_LOG(LogTemp, Error, TEXT("Choose_Default requires a valid manager."));
		return false;
	}
	URpySession *Session = URpyScriptInterpreterBaseManagerLibrary::GetSession(TScriptInterface<IRpyScriptInterpreterBaseManager>(Manager));
	if (!IsValid(Session))
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot choose %d, there is no session; the VN player sets the interpreter on its managers."), *Manager->GetName(), Index);
		return false;
	}
	// OnChoice logs an index it rejects; it also returns false when the script ends, which is not an error.
	Session->OnChoice(Index);
	return true;
}
