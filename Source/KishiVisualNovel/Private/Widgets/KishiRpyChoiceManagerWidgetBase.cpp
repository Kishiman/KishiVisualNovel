#include "Widgets/KishiRpyChoiceManagerWidgetBase.h"
#include "Widgets/KishiChoiceEntryWidgetBase.h"
#include "Interfaces/RpyScriptInterpreterBaseManager.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Interfaces/LayeredSpriteHandler.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "LevelSequencePlayer.h"
#include "PaperSprite.h"
#include "TimerManager.h"
#include "Rpy/RpySession.h"
#include "Templates/KishiScriptInterface.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"

void UKishiRpyChoiceManagerWidgetBase::SetChoiceManagerState_Implementation(const FRpyChoiceManagerState &State)
{
	IRpyChoiceManager::Execute_Menu(this, State.choices);
}

bool UKishiRpyChoiceManagerWidgetBase::Menu_Implementation(const TArray<FString> &Choices)
{
	ChoiceState.choices = Choices;
	if (!ChoicesPanel || !ChoiceEntryClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot display %d choices without a panel named ChoicesPanel and a ChoiceEntryClass; the menu only works if OnChoicesRequested displays them."), *GetName(), Choices.Num());
	}
	if (ChoicesPanel)
	{
		ChoicesPanel->ClearChildren();
		if (ChoiceEntryClass)
		{
			for (int32 Index = 0; Index < Choices.Num(); ++Index)
			{
				UKishiChoiceEntryWidgetBase *Entry = CreateWidget<UKishiChoiceEntryWidgetBase>(this, ChoiceEntryClass);
				if (!Entry)
				{
					continue;
				}
				Entry->SetChoice(Index, Choices[Index]);
				Entry->OnChoiceClicked.AddDynamic(this, &UKishiRpyChoiceManagerWidgetBase::HandleChoiceClicked);
				ChoicesPanel->AddChild(Entry);
			}
		}
	}
	OnChoicesRequested(Choices);
	return ChoicesPanel != nullptr || ChoiceEntryClass != nullptr;
}

void UKishiRpyChoiceManagerWidgetBase::HandleChoiceClicked(int32 Index)
{
	IRpyChoiceManager::Execute_Choose(this, Index);
}

void UKishiRpyChoiceManagerWidgetBase::Choose_Implementation(int32 Index)
{
	if (!ChoiceState.choices.IsValidIndex(Index))
	{
		return;
	}
	OnChoiceSelected(Index);
	if (IsValid(Interpreter.GetObject()))
	{
		URpySession *Session = IRpyScriptInterpreter::Execute_GetSession(Interpreter.GetObject());
		if (IsValid(Session))
		{
			Session->OnChoice(Index);
		}
	}
}

void UKishiRpyChoiceManagerWidgetBase::Reset_Implementation()
{
	ChoiceState.choices.Reset();
	if (ChoicesPanel)
	{
		ChoicesPanel->ClearChildren();
	}
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}
