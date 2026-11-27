#include "Managers/Statement/KishiRpyStatementManagerWidgetBase.h"
#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Player/RpyScriptInterpreter.h"
#include "Showables/LayeredSpriteHandler.h"
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
#include "Templates/KishiScriptInterface.h"
#include "Save/BaseRpyGameInstance.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"

void UKishiRpyStatementManagerWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (SaveButton)
		SaveButton->OnClicked.AddUniqueDynamic(this, &UKishiRpyStatementManagerWidgetBase::HandleSaveButtonClicked);
	if (LoadButton)
		LoadButton->OnClicked.AddUniqueDynamic(this, &UKishiRpyStatementManagerWidgetBase::HandleLoadButtonClicked);
	if (QSaveButton)
		QSaveButton->OnClicked.AddUniqueDynamic(this, &UKishiRpyStatementManagerWidgetBase::HandleQuickSaveButtonClicked);
	if (QLoadButton)
		QLoadButton->OnClicked.AddUniqueDynamic(this, &UKishiRpyStatementManagerWidgetBase::HandleQuickLoadButtonClicked);
	// Clear the designer's placeholder speaker and text, so nothing shows before the first line.
	if (!IsDesignTime())
		UpdateVisibleStatement();
}

void UKishiRpyStatementManagerWidgetBase::HandleSaveButtonClicked()
{
	URpyStatementManagerLibrary::ToggleSaveMenu(this);
}

void UKishiRpyStatementManagerWidgetBase::HandleLoadButtonClicked()
{
	URpyStatementManagerLibrary::ToggleLoadMenu(this);
}

void UKishiRpyStatementManagerWidgetBase::HandleQuickSaveButtonClicked()
{
	URpyStatementManagerLibrary::QuickSave(this);
}

void UKishiRpyStatementManagerWidgetBase::HandleQuickLoadButtonClicked()
{
	URpyStatementManagerLibrary::QuickLoad(this);
}

void UKishiRpyStatementManagerWidgetBase::SetStatementManagerState_Implementation(const FRpyStatementManagerState &State)
{
	StatementState = State;
	SpeakerName = URpyStatementManagerLibrary::GetSpeakerDisplayName(State.name);
	UStringUtils::StringToTaggedString(State.statement, TaggedStatement);
	StatementCharactersNum = TaggedStatement.Length;
	PreviousStatementCharactersNum = StatementCharactersNum;
	CharacterAccumulator = 0.0f;
	UpdateVisibleStatement();
	OnStatementRequested(StatementState.name, StatementState.statement);
}

ERpyInstructionResult UKishiRpyStatementManagerWidgetBase::Say_Implementation(FName Name, const FString &Statement)
{
	SayInstruction = URpyScriptInterpreterBaseManagerLibrary::GetExecutingInstruction(this);
	StatementState.name = Name;
	StatementState.statement = Statement;
	SpeakerName = URpyStatementManagerLibrary::GetSpeakerDisplayName(Name);
	UStringUtils::StringToTaggedString(Statement, TaggedStatement);
	StatementCharactersNum = 0;
	PreviousStatementCharactersNum = 0;
	CharacterAccumulator = 0.0f;
	LastViseme = FMouthViseme();
	UpdateVisibleStatement();
	OnStatementRequested(Name, Statement);
	// The session advances when the player clicks through the line (CompleteOrRevealStatement).
	return ERpyInstructionResult::Wait;
}

void UKishiRpyStatementManagerWidgetBase::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	const EInstructionRunTimeType RunType = URpyScriptInterpreterBaseManagerLibrary::GetRunTimeType(
			TScriptInterface<IRpyScriptInterpreterBaseManager>(this));
	if (RunType != EInstructionRunTimeType::SAY || StatementCharactersNum >= TaggedStatement.Length)
	{
		return;
	}
	CharacterAccumulator += FMath::Max(0.0f, InDeltaTime) * FMath::Max(0.0f, TextSpeed);
	const int32 NewCount = FMath::Min(TaggedStatement.Length, FMath::FloorToInt(CharacterAccumulator));
	if (NewCount <= StatementCharactersNum)
	{
		return;
	}
	PreviousStatementCharactersNum = StatementCharactersNum;
	StatementCharactersNum = NewCount;
	UpdateVisibleStatement();
	EmitDialogueViseme();
}

void UKishiRpyStatementManagerWidgetBase::EmitDialogueViseme()
{
	URpyStatementManagerLibrary::EmitDialogueViseme(this, StatementState.name, TaggedStatement, PreviousStatementCharactersNum, StatementCharactersNum, TextSpeed, LastViseme);
}

FReply UKishiRpyStatementManagerWidgetBase::NativeOnMouseButtonDown(const FGeometry &InGeometry, const FPointerEvent &InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		CompleteOrRevealStatement();
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UKishiRpyStatementManagerWidgetBase::UpdateVisibleStatement()
{
	if (NameText)
	{
		NameText->SetText(FText::FromString(SpeakerName));
	}
	if (StatmentText)
	{
		StatmentText->SetText(FText::FromString(UStringUtils::SubString(TaggedStatement, 0, StatementCharactersNum)));
	}
}

void UKishiRpyStatementManagerWidgetBase::CompleteOrRevealStatement()
{
	// Outside dialogue, a click skips what the script waits for: a transition or a pause.
	const EInstructionRunTimeType RunType = URpyScriptInterpreterBaseManagerLibrary::GetRunTimeType(TScriptInterface<IRpyScriptInterpreterBaseManager>(this));
	if (RunType != EInstructionRunTimeType::SAY)
	{
		URpyScriptInterpreterBaseManagerLibrary::RequestSkip(this);
		return;
	}
	if (StatementCharactersNum < TaggedStatement.Length)
	{
		PreviousStatementCharactersNum = StatementCharactersNum;
		StatementCharactersNum = TaggedStatement.Length;
		CharacterAccumulator = static_cast<float>(StatementCharactersNum);
		UpdateVisibleStatement();
		// Close the speaker's mouth when the rest of the line is revealed at once.
		LastViseme = FMouthViseme();
		EmitDialogueViseme();
		return;
	}
	URpyScriptInterpreterBaseManagerLibrary::ReportInstructionComplete(this, SayInstruction.Get());
}

void UKishiRpyStatementManagerWidgetBase::Reset_Implementation()
{
	StatementState = FRpyStatementManagerState();
	SpeakerName.Reset();
	TaggedStatement = FTaggedString();
	StatementCharactersNum = 0;
	PreviousStatementCharactersNum = 0;
	CharacterAccumulator = 0.0f;
	UpdateVisibleStatement();
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}
