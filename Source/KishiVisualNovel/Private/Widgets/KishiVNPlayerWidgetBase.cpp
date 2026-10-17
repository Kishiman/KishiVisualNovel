#include "Widgets/KishiVNPlayerWidgetBase.h"

#include "Interfaces/RpyScriptInterpreter.h"
#include "Interfaces/RpyScriptInterpreterBaseManager.h"
#include "Components/Widget.h"
#include "UObject/UnrealType.h"
#include "Rpy/RpySession.h"
#include "Widgets/KishiRpyAudioManagerWidgetBase.h"
#include "Widgets/KishiRpyChoiceManagerWidgetBase.h"
#include "Widgets/KishiRpySceneManagerWidgetBase.h"
#include "Widgets/KishiRpyShowManagerWidgetBase.h"
#include "Widgets/KishiRpyStatementManagerWidgetBase.h"

namespace
{
	template <typename TInterface>
	TScriptInterface<TInterface> MakeWidgetInterface(UUserWidget *Widget)
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

TScriptInterface<IRpyAudioManager> UKishiVNPlayerWidgetBase::GetAudioManager_Implementation() const { return MakeWidgetInterface<IRpyAudioManager>(AudioManager); }
TScriptInterface<IRpyChoiceManager> UKishiVNPlayerWidgetBase::GetChoiceManager_Implementation() const { return MakeWidgetInterface<IRpyChoiceManager>(ChoiceManager); }
TScriptInterface<IRpySceneManager> UKishiVNPlayerWidgetBase::GetSceneManager_Implementation() const { return MakeWidgetInterface<IRpySceneManager>(SceneManager); }
TScriptInterface<IRpyShowManager> UKishiVNPlayerWidgetBase::GetShowManager_Implementation() const { return MakeWidgetInterface<IRpyShowManager>(ShowManager); }
TScriptInterface<IRpyStatementManager> UKishiVNPlayerWidgetBase::GetStatementManager_Implementation() const { return MakeWidgetInterface<IRpyStatementManager>(StatementManager); }

void UKishiVNPlayerWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	WireManagers();
}

void UKishiVNPlayerWidgetBase::WireManagers()
{
	TScriptInterface<IRpyScriptInterpreter> Self;
	Self.SetObject(this);
	Self.SetInterface(Cast<IRpyScriptInterpreter>(this));
	for (UUserWidget *Widget : {AudioManager, ChoiceManager, SceneManager, ShowManager, StatementManager})
	{
		if (UKishiRpyManagerWidgetBase *Manager = Cast<UKishiRpyManagerWidgetBase>(Widget))
		{
			Manager->SetInterpreter(Self);
		}
	}
}

void UKishiVNPlayerWidgetBase::StartUp_Implementation()
{
	WireManagers();
	bStarted = true;
	OnInterpreterStarted();
}

void UKishiVNPlayerWidgetBase::Shutdown_Implementation()
{
	const TArray<UUserWidget *> Managers = {AudioManager, ChoiceManager, SceneManager, ShowManager, StatementManager};
	for (UUserWidget *Widget : Managers)
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

void UKishiVNPlayerWidgetBase::setSaveMenuVisible_Implementation(bool bVisible) const
{
	bSaveMenuVisible = bVisible;
	if (SaveMenu)
	{
		SaveMenu->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		if (bVisible)
		{
			if (UFunction *ListFunction = SaveMenu->FindFunction(FName(TEXT("List"))))
				SaveMenu->ProcessEvent(ListFunction, nullptr);
			if (FBoolProperty *SaveMode = FindFProperty<FBoolProperty>(SaveMenu->GetClass(), TEXT("bSaveMode")))
				SaveMode->SetPropertyValue_InContainer(SaveMenu, true);
		}
	}
	const_cast<UKishiVNPlayerWidgetBase *>(this)->OnSaveMenuVisibilityChanged(bVisible);
}

void UKishiVNPlayerWidgetBase::setLoadMenuVisible_Implementation(bool bVisible) const
{
	bLoadMenuVisible = bVisible;
	if (SaveMenu)
	{
		SaveMenu->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		if (bVisible)
		{
			if (UFunction *ListFunction = SaveMenu->FindFunction(FName(TEXT("List"))))
				SaveMenu->ProcessEvent(ListFunction, nullptr);
			if (FBoolProperty *SaveMode = FindFProperty<FBoolProperty>(SaveMenu->GetClass(), TEXT("bSaveMode")))
				SaveMode->SetPropertyValue_InContainer(SaveMenu, false);
		}
	}
	const_cast<UKishiVNPlayerWidgetBase *>(this)->OnLoadMenuVisibilityChanged(bVisible);
}

void UKishiVNPlayerWidgetBase::setConfigMenuVisible_Implementation(bool bVisible) const
{
	bConfigMenuVisible = bVisible;
	const_cast<UKishiVNPlayerWidgetBase *>(this)->OnConfigMenuVisibilityChanged(bVisible);
}

void UKishiVNPlayerWidgetBase::setBackLogMenuVisible_Implementation(bool bVisible) const
{
	bBackLogMenuVisible = bVisible;
	const_cast<UKishiVNPlayerWidgetBase *>(this)->OnBackLogMenuVisibilityChanged(bVisible);
}

void UKishiVNPlayerWidgetBase::SetSession(URpySession *InSession)
{
	Session = InSession;
	if (IsValid(Session))
	{
		Session->interpreter.SetObject(this);
		Session->interpreter.SetInterface(Cast<IRpyScriptInterpreter>(this));
	}
}

void UKishiVNPlayerWidgetBase::OnInstructionComplete_Implementation(EInstructionRunTimeType Type)
{
	TScriptInterface<IRpyScriptInterpreter> Self;
	Self.SetObject(this);
	Self.SetInterface(Cast<IRpyScriptInterpreter>(this));
	URpyScriptInterpreterImplementation::IOnInstructionComplete_Default(Self, Type);
	OnInstructionCompleted(Type);
}

void UKishiVNPlayerWidgetBase::OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset)
{
	TScriptInterface<IRpyScriptInterpreter> Self;
	Self.SetObject(this);
	Self.SetInterface(Cast<IRpyScriptInterpreter>(this));
	URpyScriptInterpreterImplementation::IOnDialogueViseme_Default(Self, Name, Viseme, TimeOffset);
	OnViseme(Name, Viseme, TimeOffset);
}

void UKishiVNPlayerWidgetBase::OnScriptComplete_Implementation()
{
	OnVNScriptCompleted();
}
