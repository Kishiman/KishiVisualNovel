#include "Actors/RpyMainActorBase.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Interfaces/RpyStatefulActor.h"
#include "Rpy/RpyScript.h"

ARpyMainActorBase::ARpyMainActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	SessionClass = URpySession::StaticClass();
}

bool ARpyMainActorBase::IsActive_Implementation() const
{
	return IsValid(Session);
}

URpySession *ARpyMainActorBase::GetCurrentSession_Implementation() const
{
	return Session;
}

void ARpyMainActorBase::StartRpyScript_Implementation(URpyScript *Script, FName const &Label)
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
	Session->AddScript(Script);
	if (!InitializeInterpreterForSession(Session))
	{
		Session = nullptr;
		return;
	}
	Session->StartLabel(Label);
}

bool ARpyMainActorBase::InitializeInterpreterForSession(URpySession *TargetSession)
{
	if (!IsValid(TargetSession))
	{
		return false;
	}

	APlayerController *PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!IsValid(VNPlayer) && VNPlayerClass)
	{
		VNPlayer = CreateWidget<UUserWidget>(PlayerController, VNPlayerClass);
		if (!IsValid(VNPlayer))
		{
			UE_LOG(LogTemp, Error, TEXT("ARpyMainActorBase: Failed to create VN player widget."));
			return false;
		}
	}

	if (IsValid(VNPlayer) && VNPlayer->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
	{
		Interpreter.SetObject(VNPlayer);
		Interpreter.SetInterface(Cast<IRpyScriptInterpreter>(VNPlayer));
	}

	UObject *InterpreterObject = Interpreter.GetObject();
	if (!IsValid(InterpreterObject) || !InterpreterObject->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("ARpyMainActorBase: Cannot initialize a VN session without an RPY script interpreter."));
		return false;
	}

	TargetSession->interpreter = Interpreter;
	IRpyScriptInterpreter::Execute_SetSession(InterpreterObject, TargetSession);
	IRpyScriptInterpreter::Execute_StartUp(InterpreterObject);

	if (IsValid(VNPlayer) && !VNPlayer->IsInViewport())
	{
		VNPlayer->AddToViewport();
	}
	if (IsValid(VNPlayer) && PlayerController)
	{
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, VNPlayer);
	}

	return true;
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
	for (const TScriptInterface<IRpyCloseVNEventListener> &Listener : CloseListeners)
	{
		if (IsValid(Listener.GetObject()))
		{
			IRpyCloseVNEventListener::Execute_OnCloseVNEvent(Listener.GetObject());
		}
	}
	OnVNClosed();
}

void ARpyMainActorBase::AddOnCloseVNEventListener_Implementation(const TScriptInterface<IRpyCloseVNEventListener> &Listener)
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

URpyState *ARpyMainActorBase::SaveToState_Implementation(UObject *Outer)
{
	URpyMainActorState *State = NewObject<URpyMainActorState>(Outer ? Outer : this);
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
		State->rpySessionState = Cast<URpySessionState>(IRpyStateful::Execute_SaveToState(Session, State));
	}
	return State;
}

void ARpyMainActorBase::LoadFromState_Implementation(const URpyState *State)
{
	const URpyMainActorState *MainState = Cast<URpyMainActorState>(State);
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
		if (!InitializeInterpreterForSession(Session))
		{
			Session = nullptr;
			return;
		}
		IRpyStateful::Execute_LoadFromState(Session, MainState->rpySessionState);
	}
}
