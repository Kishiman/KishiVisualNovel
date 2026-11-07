#include "Widgets/KishiRpyStatementManagerWidgetBase.h"
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
#include "Templates/KishiScriptInterface.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"

void UKishiRpyStatementManagerWidgetBase::SetStatementManagerState_Implementation(const FRpyStatementManagerState &State)
{
	StatementState = State;
	SpeakerName = State.name.ToString();
	UStringUtils::StringToTaggedString(State.statement, TaggedStatement);
	StatementCharactersNum = TaggedStatement.Length;
	PreviousStatementCharactersNum = StatementCharactersNum;
	CharacterAccumulator = 0.0f;
	UpdateVisibleStatement();
	OnStatementRequested(StatementState.name, StatementState.statement);
}

bool UKishiRpyStatementManagerWidgetBase::Say_Implementation(FName Name, const FString &Statement)
{
	StatementState.name = Name;
	StatementState.statement = Statement;
	SpeakerName = Name.ToString();
	UStringUtils::StringToTaggedString(Statement, TaggedStatement);
	StatementCharactersNum = 0;
	PreviousStatementCharactersNum = 0;
	CharacterAccumulator = 0.0f;
	LastViseme = FMouthViseme();
	UpdateVisibleStatement();
	OnStatementRequested(Name, Statement);
	// Wait: the session advances when the player clicks through the line (CompleteOrRevealStatement).
	return false;
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
	// Keep the last spoken viseme through Neutral characters, so the mouth holds its shape until
	// the speaker has been silent for a while, as WGT_RPYStatementManager does.
	const FMouthViseme Viseme = UStringUtils::getVisemeBetween(TaggedStatement, PreviousStatementCharactersNum, StatementCharactersNum);
	if (Viseme.type != EMouthViseme::Neutral)
	{
		LastViseme = Viseme;
	}
	// The offset is the time since the last spoken viseme; a fully revealed line reports a long silence.
	const bool bLineComplete = StatementCharactersNum >= TaggedStatement.Length;
	const float TimeOffset = bLineComplete ? 60.0f
										   : (TextSpeed > 0.0f ? FMath::Max(0, StatementCharactersNum - LastViseme.taggedIndex) / TextSpeed : 60.0f);
	if (IsValid(Interpreter.GetObject()))
	{
		IRpyScriptInterpreter::Execute_OnDialogueViseme(Interpreter.GetObject(), StatementState.name, Viseme.type, TimeOffset);
	}
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
	if (IsValid(Interpreter.GetObject()))
	{
		IRpyScriptInterpreter::Execute_HandleInstructionComplete(Interpreter.GetObject());
	}
}

void UKishiRpyStatementManagerWidgetBase::Reset_Implementation()
{
	StatementState = FRpyStatementManagerState();
	TaggedStatement = FTaggedString();
	StatementCharactersNum = 0;
	PreviousStatementCharactersNum = 0;
	CharacterAccumulator = 0.0f;
	UpdateVisibleStatement();
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}
