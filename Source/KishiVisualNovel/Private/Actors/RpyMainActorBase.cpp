#include "Actors/RpyMainActorBase.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Interfaces/RpyStatefulActor.h"
#include "Rpy/RpyScript.h"
#include "Widgets/KishiVNPlayerWidget.h"

ARpyMainActorBase::ARpyMainActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	SessionClass = URpySession::StaticClass();
}

bool ARpyMainActorBase::IsActive_Implementation() const
{
	return IsValid(Session);
}

URpySession* ARpyMainActorBase::GetCurrentSession_Implementation() const
{
	return Session;
}

void ARpyMainActorBase::StartRpyScript_Implementation(URpyScript* Script, FName const& Label)
{
	if (!IsValid(Script) || !SessionClass)
	{
		return;
	}

	if (IsValid(Session))
	{
		CloseVN_Implementation();
	}

	ActiveScript = Script;
	Session = NewObject<URpySession>(this, SessionClass);
	if (!IsValid(Session))
	{
		return;
	}
	Session->interpreter = Interpreter;
	Session->AddScript(Script);
	if (IsValid(Interpreter.GetObject()))
	{
		IRpyScriptInterpreter::Execute_StartUp(Interpreter.GetObject());
	}
	if (VNPlayerClass)
	{
		APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
		VNPlayer = CreateWidget<UUserWidget>(PlayerController, VNPlayerClass);
		if (VNPlayer)
		{
			if (VNPlayer->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
			{
				Interpreter.SetObject(VNPlayer);
				Interpreter.SetInterface(Cast<IRpyScriptInterpreter>(VNPlayer));
				Session->interpreter = Interpreter;
			}
			if (UKishiVNPlayerWidget* NativePlayer = Cast<UKishiVNPlayerWidget>(VNPlayer))
			{
				NativePlayer->SetSession(Session);
			}
			VNPlayer->AddToViewport();
			if (PlayerController)
			{
				PlayerController->bShowMouseCursor = true;
				PlayerController->bEnableClickEvents = true;
				PlayerController->bEnableMouseOverEvents = true;
				UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, VNPlayer);
			}
		}
	}
	Session->StartLabel(Label);
}

void ARpyMainActorBase::CloseVN_Implementation()
{
	if (IsValid(Interpreter.GetObject()))
	{
		IRpyScriptInterpreter::Execute_Shutdown(Interpreter.GetObject());
	}
	if (IsValid(VNPlayer))
	{
		VNPlayer->RemoveFromParent();
	}
	VNPlayer = nullptr;
	Session = nullptr;
	ActiveScript = nullptr;
	for (const TScriptInterface<IRpyCloseVNEventListener>& Listener : CloseListeners)
	{
		if (IsValid(Listener.GetObject()))
		{
			IRpyCloseVNEventListener::Execute_OnCloseVNEvent(Listener.GetObject());
		}
	}
	OnVNClosed();
}

void ARpyMainActorBase::AddOnCloseVNEventListener_Implementation(const TScriptInterface<IRpyCloseVNEventListener>& Listener)
{
	if (IsValid(Listener.GetObject()))
	{
		CloseListeners.AddUnique(Listener);
	}
}

TSubclassOf<URpyState> ARpyMainActorBase::GetStateClass_Implementation()
{
	return URpyMainActorState::StaticClass();
}

URpyState* ARpyMainActorBase::SaveToState_Implementation(UObject* Outer)
{
	URpyMainActorState* State = NewObject<URpyMainActorState>(Outer ? Outer : this);
	if (!State)
	{
		return nullptr;
	}
	TScriptInterface<IRpyStatefulActor> StatefulTarget;
	StatefulTarget.SetObject(this);
	StatefulTarget.SetInterface(Cast<IRpyStatefulActor>(this));
	URpyStatefulActorLibrary::BaseSaveToState(StatefulTarget, State);
	if (IsValid(Session))
	{
		State->rpySessionState = Cast<URpySessionState>(Session->SaveToState(State));
	}
	return State;
}

void ARpyMainActorBase::LoadFromState_Implementation(const URpyState* State)
{
	const URpyMainActorState* MainState = Cast<URpyMainActorState>(State);
	if (!MainState)
	{
		return;
	}
	TScriptInterface<IRpyStatefulActor> StatefulTarget;
	StatefulTarget.SetObject(this);
	StatefulTarget.SetInterface(Cast<IRpyStatefulActor>(this));
	URpyStatefulActorLibrary::BaseLoadFromState(StatefulTarget, MainState);
	if (MainState->rpySessionState && SessionClass)
	{
		Session = NewObject<URpySession>(this, SessionClass);
		Session->LoadFromState(MainState->rpySessionState);
		Session->interpreter = Interpreter;
	}
}
