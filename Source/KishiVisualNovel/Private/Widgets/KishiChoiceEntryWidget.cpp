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

void UKishiChoiceEntryWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (UButton *Button = GetChoiceButton())
	{
		Button->OnClicked.AddUniqueDynamic(this, &UKishiChoiceEntryWidgetBase::HandleClicked);
	}
}

void UKishiChoiceEntryWidgetBase::SetChoice(int32 Index, const FString &Text)
{
	ChoiceIndex = Index;
	ChoiceText = Text;
	if (UTextBlock *Label = GetChoiceLabel())
	{
		Label->SetText(FText::FromString(Text));
	}
}

UButton *UKishiChoiceEntryWidgetBase::GetChoiceButton_Implementation() const
{
	if (!ChoiceButton)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: GetChoiceButton is required. Override it or name a Button widget ChoiceButton."), *GetNameSafe(GetClass()));
	}
	return ChoiceButton;
}

UTextBlock *UKishiChoiceEntryWidgetBase::GetChoiceLabel_Implementation() const
{
	if (!ChoiceLabel)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: GetChoiceLabel is required. Override it or name a TextBlock widget ChoiceLabel."), *GetNameSafe(GetClass()));
	}
	return ChoiceLabel;
}

void UKishiChoiceEntryWidgetBase::HandleClicked()
{
	OnChoiceClicked.Broadcast(ChoiceIndex);
}
