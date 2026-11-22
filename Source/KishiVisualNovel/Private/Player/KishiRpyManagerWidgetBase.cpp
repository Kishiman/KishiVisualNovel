#include "Player/KishiRpyManagerWidgetBase.h"
#include "Managers/Choice/KishiRpyChoiceManagerWidgetBase.h"
#include "Managers/Scene/KishiRpySceneManagerWidgetBase.h"
#include "Managers/Show/KishiRpyShowManagerWidgetBase.h"
#include "Managers/Statement/KishiRpyStatementManagerWidgetBase.h"
#include "Managers/Audio/KishiRpyAudioManagerWidgetBase.h"
#include "Player/RpyScriptInterpreter.h"
#include "Player/KishiRpyWidgetReflection.h"
#define IMPLEMENT_MANAGER_BASE(ClassName)                                                                                                                                                       \
	TScriptInterface<IRpyScriptInterpreter> ClassName::GetInterpreter_Implementation() const { return UKishiRpyManagerWidgetBase::GetInterpreter_Implementation(); }                              \
	void ClassName::OnInstructionComplete_Implementation() { UKishiRpyManagerWidgetBase::OnInstructionComplete_Implementation(); }                         \
	void ClassName::OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) { UKishiRpyManagerWidgetBase::OnDialogueViseme_Implementation(Name, Viseme, TimeOffset); } \
	void ClassName::OnScriptComplete_Implementation() { UKishiRpyManagerWidgetBase::OnScriptComplete_Implementation(); }

IMPLEMENT_MANAGER_BASE(UKishiRpyChoiceManagerWidgetBase)
IMPLEMENT_MANAGER_BASE(UKishiRpySceneManagerWidgetBase)
IMPLEMENT_MANAGER_BASE(UKishiRpyStatementManagerWidgetBase)

#undef IMPLEMENT_MANAGER_BASE

TScriptInterface<IRpyScriptInterpreter> UKishiRpyAudioManagerWidgetBase::GetInterpreter_Implementation() const { return UKishiRpyManagerWidgetBase::GetInterpreter_Implementation(); }
void UKishiRpyAudioManagerWidgetBase::OnInstructionComplete_Implementation() { UKishiRpyManagerWidgetBase::OnInstructionComplete_Implementation(); }
void UKishiRpyAudioManagerWidgetBase::OnScriptComplete_Implementation() { UKishiRpyManagerWidgetBase::OnScriptComplete_Implementation(); }

TScriptInterface<IRpyScriptInterpreter> UKishiRpyShowManagerWidgetBase::GetInterpreter_Implementation() const { return UKishiRpyManagerWidgetBase::GetInterpreter_Implementation(); }
void UKishiRpyShowManagerWidgetBase::OnInstructionComplete_Implementation() { UKishiRpyManagerWidgetBase::OnInstructionComplete_Implementation(); }
void UKishiRpyShowManagerWidgetBase::OnScriptComplete_Implementation() { UKishiRpyManagerWidgetBase::OnScriptComplete_Implementation(); }

void UKishiRpyManagerWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!IsDesignTime())
		WarnOnInterfaceEventOverrides(this);
}

void UKishiRpyManagerWidgetBase::OnInstructionComplete_Implementation()
{
	OnManagerInstructionComplete();
}
void UKishiRpyManagerWidgetBase::OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset)
{
	OnManagerViseme(Name, Viseme, TimeOffset);
}
void UKishiRpyManagerWidgetBase::OnScriptComplete_Implementation()
{
	OnManagerScriptComplete();
}
void UKishiRpyManagerWidgetBase::Reset_Implementation()
{
	OnManagerReset();
}
