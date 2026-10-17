#include "Widgets/KishiVNPlayerWidget.h"

#include "Interfaces/RpyScriptInterpreter.h"
#include "Interfaces/RpyScriptInterpreterBaseManager.h"
#include "Components/Widget.h"
#include "UObject/UnrealType.h"
#include "Rpy/RpySession.h"
#include "Widgets/KishiRpyAudioManagerWidget.h"
#include "Widgets/KishiRpyChoiceManagerWidget.h"
#include "Widgets/KishiRpySceneManagerWidget.h"
#include "Widgets/KishiRpyShowManagerWidget.h"
#include "Widgets/KishiRpyStatementManagerWidget.h"

namespace
{
	template <typename TInterface>
	TScriptInterface<TInterface> MakeWidgetInterface(UUserWidget* Widget)
	{
		TScriptInterface<TInterface> Result;
		if (IsValid(Widget) && Widget->GetClass()->ImplementsInterface(TInterface::UClassType::StaticClass()))
		{
			Result.SetObject(Widget);
			Result.SetInterface(Cast<TInterface>(Widget));
		}
		return Result;
	}
}

TScriptInterface<IRpyAudioManager> UKishiVNPlayerWidget::GetAudioManager_Implementation() const { return MakeWidgetInterface<IRpyAudioManager>(AudioManager); }
TScriptInterface<IRpyChoiceManager> UKishiVNPlayerWidget::GetChoiceManager_Implementation() const { return MakeWidgetInterface<IRpyChoiceManager>(ChoiceManager); }
TScriptInterface<IRpySceneManager> UKishiVNPlayerWidget::GetSceneManager_Implementation() const { return MakeWidgetInterface<IRpySceneManager>(SceneManager); }
TScriptInterface<IRpyShowManager> UKishiVNPlayerWidget::GetShowManager_Implementation() const { return MakeWidgetInterface<IRpyShowManager>(ShowManager); }
TScriptInterface<IRpyStatementManager> UKishiVNPlayerWidget::GetStatementManager_Implementation() const { return MakeWidgetInterface<IRpyStatementManager>(StatementManager); }

void UKishiVNPlayerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	WireManagers();
}

void UKishiVNPlayerWidget::WireManagers()
{
	TScriptInterface<IRpyScriptInterpreter> Self;
	Self.SetObject(this);
	Self.SetInterface(Cast<IRpyScriptInterpreter>(this));
	for (UUserWidget* Widget : { AudioManager, ChoiceManager, SceneManager, ShowManager, StatementManager })
	{
		if (UKishiRpyManagerWidgetBase* Manager = Cast<UKishiRpyManagerWidgetBase>(Widget))
		{
			Manager->SetInterpreter(Self);
		}
	}
}

void UKishiVNPlayerWidget::StartUp_Implementation()
{
	WireManagers();
	bStarted = true;
	OnInterpreterStarted();
}

void UKishiVNPlayerWidget::Shutdown_Implementation()
{
	const TArray<UUserWidget*> Managers = { AudioManager, ChoiceManager, SceneManager, ShowManager, StatementManager };
	for (UUserWidget* Widget : Managers)
	{
		if (IsValid(Widget) && Widget->GetClass()->ImplementsInterface(URpyScriptInterpreterBaseManager::StaticClass()))
		{
			IRpyScriptInterpreterBaseManager::Execute_Reset(Widget);
		}
	}
	bStarted = false;
	Session = nullptr;
	OnInterpreterShutdown();
}

void UKishiVNPlayerWidget::setSaveMenuVisible_Implementation(bool bVisible) const
{
	bSaveMenuVisible = bVisible;
	if (SaveMenu)
	{
		SaveMenu->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		if (bVisible)
		{
			if (UFunction* ListFunction = SaveMenu->FindFunction(FName(TEXT("List")))) SaveMenu->ProcessEvent(ListFunction, nullptr);
			if (FBoolProperty* SaveMode = FindFProperty<FBoolProperty>(SaveMenu->GetClass(), TEXT("bSaveMode"))) SaveMode->SetPropertyValue_InContainer(SaveMenu, true);
		}
	}
	const_cast<UKishiVNPlayerWidget*>(this)->OnSaveMenuVisibilityChanged(bVisible);
}

void UKishiVNPlayerWidget::setLoadMenuVisible_Implementation(bool bVisible) const
{
	bLoadMenuVisible = bVisible;
	if (SaveMenu)
	{
		SaveMenu->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		if (bVisible)
		{
			if (UFunction* ListFunction = SaveMenu->FindFunction(FName(TEXT("List")))) SaveMenu->ProcessEvent(ListFunction, nullptr);
			if (FBoolProperty* SaveMode = FindFProperty<FBoolProperty>(SaveMenu->GetClass(), TEXT("bSaveMode"))) SaveMode->SetPropertyValue_InContainer(SaveMenu, false);
		}
	}
	const_cast<UKishiVNPlayerWidget*>(this)->OnLoadMenuVisibilityChanged(bVisible);
}

void UKishiVNPlayerWidget::setConfigMenuVisible_Implementation(bool bVisible) const
{
	bConfigMenuVisible = bVisible;
	const_cast<UKishiVNPlayerWidget*>(this)->OnConfigMenuVisibilityChanged(bVisible);
}

void UKishiVNPlayerWidget::setBackLogMenuVisible_Implementation(bool bVisible) const
{
	bBackLogMenuVisible = bVisible;
	const_cast<UKishiVNPlayerWidget*>(this)->OnBackLogMenuVisibilityChanged(bVisible);
}

void UKishiVNPlayerWidget::SetSession(URpySession* InSession)
{
	Session = InSession;
	if (IsValid(Session))
	{
		Session->interpreter.SetObject(this);
		Session->interpreter.SetInterface(Cast<IRpyScriptInterpreter>(this));
	}
}

void UKishiVNPlayerWidget::OnInstructionComplete_Implementation(EInstructionRunTimeType Type)
{
	TScriptInterface<IRpyScriptInterpreter> Self;
	Self.SetObject(this);
	Self.SetInterface(Cast<IRpyScriptInterpreter>(this));
	URpyScriptInterpreterImplementation::IOnInstructionComplete_Default(Self, Type);
	OnInstructionCompleted(Type);
}

void UKishiVNPlayerWidget::OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset)
{
	TScriptInterface<IRpyScriptInterpreter> Self;
	Self.SetObject(this);
	Self.SetInterface(Cast<IRpyScriptInterpreter>(this));
	URpyScriptInterpreterImplementation::IOnDialogueViseme_Default(Self, Name, Viseme, TimeOffset);
	OnViseme(Name, Viseme, TimeOffset);
}

void UKishiVNPlayerWidget::OnScriptComplete_Implementation()
{
	OnVNScriptCompleted();
}
