#include "Widgets/KishiRpyManagerWidgetBase.h"
#include "Widgets/KishiRpyChoiceManagerWidgetBase.h"
#include "Widgets/KishiRpySceneManagerWidgetBase.h"
#include "Widgets/KishiRpyShowManagerWidgetBase.h"
#include "Widgets/KishiRpyStatementManagerWidgetBase.h"
#include "Widgets/KishiRpyAudioManagerWidgetBase.h"
#include "Interfaces/RpyScriptInterpreter.h"
#define IMPLEMENT_MANAGER_BASE(ClassName)                                                                                                                                                       \
	TScriptInterface<IRpyScriptInterpreter> ClassName::GetInterpreter_Implementation() const { return UKishiRpyManagerWidgetBase::GetInterpreter_Implementation(); }                              \
	void ClassName::OnInstructionComplete_Implementation() { UKishiRpyManagerWidgetBase::OnInstructionComplete_Implementation(); }                         \
	void ClassName::OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) { UKishiRpyManagerWidgetBase::OnDialogueViseme_Implementation(Name, Viseme, TimeOffset); } \
	void ClassName::OnScriptComplete_Implementation() { UKishiRpyManagerWidgetBase::OnScriptComplete_Implementation(); }

IMPLEMENT_MANAGER_BASE(UKishiRpyChoiceManagerWidgetBase)
IMPLEMENT_MANAGER_BASE(UKishiRpySceneManagerWidgetBase)
IMPLEMENT_MANAGER_BASE(UKishiRpyShowManagerWidgetBase)
IMPLEMENT_MANAGER_BASE(UKishiRpyStatementManagerWidgetBase)

#undef IMPLEMENT_MANAGER_BASE

TScriptInterface<IRpyScriptInterpreter> UKishiRpyAudioManagerWidgetBase::GetInterpreter_Implementation() const { return UKishiRpyManagerWidgetBase::GetInterpreter_Implementation(); }
void UKishiRpyAudioManagerWidgetBase::OnInstructionComplete_Implementation() { UKishiRpyManagerWidgetBase::OnInstructionComplete_Implementation(); }
void UKishiRpyAudioManagerWidgetBase::OnScriptComplete_Implementation() { UKishiRpyManagerWidgetBase::OnScriptComplete_Implementation(); }

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
