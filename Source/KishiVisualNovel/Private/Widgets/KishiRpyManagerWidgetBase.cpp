#include "Widgets/KishiRpyManagerWidgetBase.h"
#include "Widgets/KishiRpyChoiceManagerWidget.h"
#include "Widgets/KishiRpySceneManagerWidget.h"
#include "Widgets/KishiRpyShowManagerWidget.h"
#include "Widgets/KishiRpyStatementManagerWidget.h"
#include "Widgets/KishiRpyAudioManagerWidget.h"
#include "Interfaces/RpyScriptInterpreter.h"
#define IMPLEMENT_MANAGER_BASE(ClassName) \
TScriptInterface<IRpyScriptInterpreter> ClassName::GetInterpreter_Implementation() const { return UKishiRpyManagerWidgetBase::GetInterpreter_Implementation(); } \
void ClassName::OnInstructionComplete_Implementation(EInstructionRunTimeType Type) { UKishiRpyManagerWidgetBase::OnInstructionComplete_Implementation(Type); } \
void ClassName::OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) { UKishiRpyManagerWidgetBase::OnDialogueViseme_Implementation(Name, Viseme, TimeOffset); } \
void ClassName::OnScriptComplete_Implementation() { UKishiRpyManagerWidgetBase::OnScriptComplete_Implementation(); }

IMPLEMENT_MANAGER_BASE(UKishiRpyChoiceManagerWidget)
IMPLEMENT_MANAGER_BASE(UKishiRpySceneManagerWidget)
IMPLEMENT_MANAGER_BASE(UKishiRpyShowManagerWidget)
IMPLEMENT_MANAGER_BASE(UKishiRpyStatementManagerWidget)

#undef IMPLEMENT_MANAGER_BASE

TScriptInterface<IRpyScriptInterpreter> UKishiRpyAudioManagerWidget::GetInterpreter_Implementation() const { return UKishiRpyManagerWidgetBase::GetInterpreter_Implementation(); }
void UKishiRpyAudioManagerWidget::OnInstructionComplete_Implementation(EInstructionRunTimeType Type) { UKishiRpyManagerWidgetBase::OnInstructionComplete_Implementation(Type); }
void UKishiRpyAudioManagerWidget::OnScriptComplete_Implementation() { UKishiRpyManagerWidgetBase::OnScriptComplete_Implementation(); }

void UKishiRpyManagerWidgetBase::OnInstructionComplete_Implementation(EInstructionRunTimeType Type)
{
	OnManagerInstructionComplete(Type);
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

