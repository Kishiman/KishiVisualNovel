#include "Widgets/KishiRpyStatementManagerWidget.h"
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

void UKishiRpyStatementManagerWidget::SetStatementManagerState_Implementation(const FRpyStatementManagerState& State)
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

bool UKishiRpyStatementManagerWidget::Say_Implementation(FName Name, const FString& Statement)
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
	return TaggedStatement.Length > 0;
}

void UKishiRpyStatementManagerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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
	LastViseme = UStringUtils::getVisemeBetween(TaggedStatement, PreviousStatementCharactersNum, StatementCharactersNum);
	const float Remaining = TextSpeed > 0.0f ? FMath::Max(0.0f, static_cast<float>(TaggedStatement.Length - LastViseme.taggedIndex)) / TextSpeed : 0.0f;
	if (IsValid(Interpreter.GetObject()))
	{
		IRpyScriptInterpreter::Execute_OnDialogueViseme(Interpreter.GetObject(), StatementState.name, LastViseme.type, Remaining);
	}
}

FReply UKishiRpyStatementManagerWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		CompleteOrRevealStatement();
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UKishiRpyStatementManagerWidget::UpdateVisibleStatement()
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

void UKishiRpyStatementManagerWidget::CompleteOrRevealStatement()
{
	if (StatementCharactersNum < TaggedStatement.Length)
	{
		PreviousStatementCharactersNum = StatementCharactersNum;
		StatementCharactersNum = TaggedStatement.Length;
		CharacterAccumulator = static_cast<float>(StatementCharactersNum);
		UpdateVisibleStatement();
		return;
	}
	if (IsValid(Interpreter.GetObject()))
	{
		IRpyScriptInterpreter::Execute_OnInstructionComplete(Interpreter.GetObject(), EInstructionRunTimeType::SAY);
	}
}

void UKishiRpyStatementManagerWidget::Reset_Implementation()
{
	StatementState = FRpyStatementManagerState();
	TaggedStatement = FTaggedString();
	StatementCharactersNum = 0;
	PreviousStatementCharactersNum = 0;
	CharacterAccumulator = 0.0f;
	UpdateVisibleStatement();
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}
