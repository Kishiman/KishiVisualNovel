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

namespace
{
	// FName(NAME_None).ToString() is "None", which would be shown as the speaker of an
	// unattributed line. Narration has no speaker, so it displays nothing.
	FString SpeakerDisplayName(FName Name)
	{
		return Name.IsNone() ? FString() : Name.ToString();
	}
}

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
}

UObject *UKishiRpyStatementManagerWidgetBase::GetInterpreterObject() const
{
	UObject *InterpreterObject = Interpreter.GetObject();
	if (IsValid(InterpreterObject) && InterpreterObject->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
		return InterpreterObject;
	UE_LOG(LogTemp, Error, TEXT("%s: no interpreter to open the save or load menu; the VN player sets it on its managers."), *GetName());
	return nullptr;
}

void UKishiRpyStatementManagerWidgetBase::HandleSaveButtonClicked()
{
	if (UObject *InterpreterObject = GetInterpreterObject())
		IRpyScriptInterpreter::Execute_setSaveMenuVisible(InterpreterObject, !IRpyScriptInterpreter::Execute_getSaveMenuVisible(InterpreterObject));
}

void UKishiRpyStatementManagerWidgetBase::HandleLoadButtonClicked()
{
	if (UObject *InterpreterObject = GetInterpreterObject())
		IRpyScriptInterpreter::Execute_setLoadMenuVisible(InterpreterObject, !IRpyScriptInterpreter::Execute_getLoadMenuVisible(InterpreterObject));
}

void UKishiRpyStatementManagerWidgetBase::HandleQuickSaveButtonClicked()
{
	if (UBaseRpyGameInstance *GameInstance = UBaseRpyGameInstance::GetBaseRpyGameInstance(this))
		GameInstance->QuickSave();
	else
		UE_LOG(LogTemp, Error, TEXT("%s: cannot quick save, the Game Instance Class must derive from BaseRpyGameInstance."), *GetName());
}

void UKishiRpyStatementManagerWidgetBase::HandleQuickLoadButtonClicked()
{
	if (UBaseRpyGameInstance *GameInstance = UBaseRpyGameInstance::GetBaseRpyGameInstance(this))
		GameInstance->QuickLoad();
	else
		UE_LOG(LogTemp, Error, TEXT("%s: cannot quick load, the Game Instance Class must derive from BaseRpyGameInstance."), *GetName());
}

void UKishiRpyStatementManagerWidgetBase::SetStatementManagerState_Implementation(const FRpyStatementManagerState &State)
{
	StatementState = State;
	SpeakerName = SpeakerDisplayName(State.name);
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
	SpeakerName = SpeakerDisplayName(Name);
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
