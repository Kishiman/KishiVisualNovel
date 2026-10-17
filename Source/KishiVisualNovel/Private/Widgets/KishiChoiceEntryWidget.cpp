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
	if (ChoiceButton)
	{
		ChoiceButton->OnClicked.AddDynamic(this, &UKishiChoiceEntryWidgetBase::HandleClicked);
	}
}

void UKishiChoiceEntryWidgetBase::SetChoice(int32 Index, const FString &Text)
{
	ChoiceIndex = Index;
	ChoiceText = Text;
	if (ChoiceLabel)
	{
		ChoiceLabel->SetText(FText::FromString(Text));
	}
}

void UKishiChoiceEntryWidgetBase::HandleClicked()
{
	OnChoiceClicked.Broadcast(ChoiceIndex);
}
