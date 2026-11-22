#include "Player/KishiVNPlayerWidgetBase.h"

#include "Player/RpyScriptInterpreter.h"
#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Save/RpySaveMenu.h"
#include "Components/Widget.h"
#include "Blueprint/WidgetTree.h"
#include "UObject/UnrealType.h"
#include "Session/RpySession.h"
#include "Player/KishiRpyWidgetReflection.h"

namespace
{
	template <typename TManagerInterface>
	UUserWidget *FindManagerWidget(const TArray<UWidget *> &Widgets, const TCHAR *RoleName, bool bWarnOnMissing)
	{
		UUserWidget *FoundWidget = nullptr;
		for (UWidget *Widget : Widgets)
		{
			if (!IsValid(Widget) || !Widget->GetClass()->ImplementsInterface(TManagerInterface::UClassType::StaticClass()))
			{
				continue;
			}

			UUserWidget *ManagerWidget = Cast<UUserWidget>(Widget);
			if (!ManagerWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("VN Player Interpreter: %s manager interface is implemented by '%s', which is not a UserWidget."), RoleName, *Widget->GetName());
				continue;
			}

			if (FoundWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("VN Player Interpreter: Multiple %s manager widgets found ('%s' and '%s'); using the first."), RoleName, *FoundWidget->GetName(), *ManagerWidget->GetName());
				continue;
			}

			FoundWidget = ManagerWidget;
		}

		if (!FoundWidget && bWarnOnMissing)
		{
			UE_LOG(LogTemp, Warning, TEXT("VN Player Interpreter: No %s manager widget found. Add a UserWidget that implements the %s manager interface to this widget tree."), RoleName, RoleName);
		}
		return FoundWidget;
	}

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
	if (!IsDesignTime())
		WarnOnInterfaceEventOverrides(this);
	DiscoverManagers(false);
	WireManagers();
}

void UKishiVNPlayerWidgetBase::DiscoverManagers(bool bWarnOnMissing)
{
	TArray<UWidget *> Widgets;
	if (WidgetTree)
	{
		WidgetTree->GetAllWidgets(Widgets);
	}

	AudioManager = FindManagerWidget<IRpyAudioManager>(Widgets, TEXT("Audio"), bWarnOnMissing);
	ChoiceManager = FindManagerWidget<IRpyChoiceManager>(Widgets, TEXT("Choice"), bWarnOnMissing);
	SceneManager = FindManagerWidget<IRpySceneManager>(Widgets, TEXT("Scene"), bWarnOnMissing);
	ShowManager = FindManagerWidget<IRpyShowManager>(Widgets, TEXT("Show"), bWarnOnMissing);
	StatementManager = FindManagerWidget<IRpyStatementManager>(Widgets, TEXT("Statement"), bWarnOnMissing);

	// The save menu is optional. A widget named SaveMenu is bound; otherwise find one by interface.
	if (!IsValid(SaveMenu) || !SaveMenu->GetClass()->ImplementsInterface(URpySaveMenu::StaticClass()))
	{
		if (UUserWidget *FoundSaveMenu = FindManagerWidget<IRpySaveMenu>(Widgets, TEXT("Save menu"), false))
			SaveMenu = FoundSaveMenu;
	}
}

void UKishiVNPlayerWidgetBase::WireManagers()
{
	TScriptInterface<IRpyScriptInterpreter> Self;
	Self.SetObject(this);
	Self.SetInterface(Cast<IRpyScriptInterpreter>(this));
	for (UUserWidget *Widget : {AudioManager, ChoiceManager, SceneManager, ShowManager, StatementManager})
	{
		if (IsValid(Widget) && Widget->GetClass()->ImplementsInterface(URpyScriptInterpreterBaseManager::StaticClass()))
		{
			IRpyScriptInterpreterBaseManager::Execute_SetInterpreter(Widget, Self);
		}
	}
	if (IsValid(SaveMenu) && SaveMenu->GetClass()->ImplementsInterface(URpySaveMenu::StaticClass()))
	{
		IRpySaveMenu::Execute_SetInterpreter(SaveMenu, Self);
	}
}

void UKishiVNPlayerWidgetBase::StartUp_Implementation()
{
	DiscoverManagers(true);
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
	if (bSaveMenuVisible)
		IRpyScriptInterpreter::Execute_setSaveMenuVisible(this, false);
	if (bLoadMenuVisible)
		IRpyScriptInterpreter::Execute_setLoadMenuVisible(this, false);
	bStarted = false;
	Session = nullptr;
	OnInterpreterShutdown();
}

void UKishiVNPlayerWidgetBase::UpdateSaveMenu(ERpySaveMenuMode MenuMode, bool bVisible) const
{
	if (!IsValid(SaveMenu))
		return;
	if (!SaveMenu->GetClass()->ImplementsInterface(URpySaveMenu::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("%s: save menu %s does not implement IRpySaveMenu. Use a widget derived from VN Save Menu Base."), *GetName(), *GetNameSafe(SaveMenu->GetClass()));
		return;
	}
	if (bVisible)
		IRpySaveMenu::Execute_HandleOpen(SaveMenu, MenuMode);
	// Save and load share the menu: hiding one must not close the other.
	else if (IRpySaveMenu::Execute_GetSaveMenuMode(SaveMenu) == MenuMode)
		IRpySaveMenu::Execute_HandleClose(SaveMenu);
}

void UKishiVNPlayerWidgetBase::setSaveMenuVisible_Implementation(bool bVisible) const
{
	UKishiVNPlayerWidgetBase *MutableThis = const_cast<UKishiVNPlayerWidgetBase *>(this);
	if (bVisible && bLoadMenuVisible)
	{
		bLoadMenuVisible = false;
		MutableThis->OnLoadMenuVisibilityChanged(false);
	}
	bSaveMenuVisible = bVisible;
	UpdateSaveMenu(ERpySaveMenuMode::Save, bVisible);
	MutableThis->OnSaveMenuVisibilityChanged(bVisible);
}

void UKishiVNPlayerWidgetBase::setLoadMenuVisible_Implementation(bool bVisible) const
{
	UKishiVNPlayerWidgetBase *MutableThis = const_cast<UKishiVNPlayerWidgetBase *>(this);
	if (bVisible && bSaveMenuVisible)
	{
		bSaveMenuVisible = false;
		MutableThis->OnSaveMenuVisibilityChanged(false);
	}
	bLoadMenuVisible = bVisible;
	UpdateSaveMenu(ERpySaveMenuMode::Load, bVisible);
	MutableThis->OnLoadMenuVisibilityChanged(bVisible);
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

void UKishiVNPlayerWidgetBase::SetSession_Implementation(URpySession *InSession)
{
	Session = InSession;
	if (IsValid(Session))
	{
		Session->interpreter.SetObject(this);
		Session->interpreter.SetInterface(Cast<IRpyScriptInterpreter>(this));
	}
}

void UKishiVNPlayerWidgetBase::HandleInstructionComplete_Implementation()
{
	TScriptInterface<IRpyScriptInterpreter> Self;
	Self.SetObject(this);
	Self.SetInterface(Cast<IRpyScriptInterpreter>(this));
	OnInstructionCompleted();
	URpyScriptInterpreterImplementation::IHandleInstructionComplete_Default(Self);
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
