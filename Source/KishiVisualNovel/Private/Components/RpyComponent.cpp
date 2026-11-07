#include "Components/RpyComponent.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/RpyScriptInterpreter.h"

URpyComponent::URpyComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
  SessionClass = URpySession::StaticClass();
}

bool URpyComponent::IsVNActive_Implementation() const
{
  return IsValid(Session);
}

URpySession *URpyComponent::GetCurrentSession_Implementation() const
{
  return Session;
}

void URpyComponent::StartRpyScript_Implementation(URpyScript *Script, FName const &Label)
{
  if (!IsValid(Script) || !SessionClass)
  {
    UE_LOG(LogTemp, Error, TEXT("%s: StartRpyScript needs a script and a SessionClass."), *GetName());
    return;
  }
  if (IsValid(Session))
    IRpyMainComponent::Execute_CloseVN(this);

  ActiveScript = Script;
  Session = NewObject<URpySession>(this, SessionClass);
  if (!IsValid(Session))
    return;
  Session->AddScript(Script);
  if (!InitializeInterpreterForSession(Session))
  {
    Session = nullptr;
    ActiveScript = nullptr;
    return;
  }
  Session->StartLabel(Label);
}

APlayerController *URpyComponent::FindPlayerController() const
{
  if (const APawn *Pawn = Cast<APawn>(GetOwner()))
    if (APlayerController *PawnController = Cast<APlayerController>(Pawn->GetController()))
      return PawnController;
  return GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
}

bool URpyComponent::InitializeInterpreterForSession(URpySession *TargetSession)
{
  if (!IsValid(TargetSession))
    return false;

  APlayerController *PlayerController = FindPlayerController();
  if (!IsValid(VNPlayer) && VNPlayerClass)
  {
    if (PlayerController)
      VNPlayer = CreateWidget<UUserWidget>(PlayerController, VNPlayerClass);
    else
      VNPlayer = CreateWidget<UUserWidget>(GetWorld(), VNPlayerClass);
    if (!IsValid(VNPlayer))
    {
      UE_LOG(LogTemp, Error, TEXT("%s: could not create VN player widget %s."), *GetName(), *GetNameSafe(VNPlayerClass));
      return false;
    }
  }
  if (IsValid(VNPlayer) && VNPlayer->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
    Interpreter = TScriptInterface<IRpyScriptInterpreter>(VNPlayer);

  UObject *InterpreterObject = Interpreter.GetObject();
  if (!IsValid(InterpreterObject) || !InterpreterObject->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
  {
    UE_LOG(LogTemp, Error, TEXT("%s: cannot start a VN session without an IRpyScriptInterpreter. Set VNPlayerClass or Interpreter."), *GetName());
    return false;
  }

  TargetSession->interpreter = Interpreter;
  TargetSession->OnScriptComplete.AddUniqueDynamic(this, &URpyComponent::HandleSessionScriptComplete);
  IRpyScriptInterpreter::Execute_SetSession(InterpreterObject, TargetSession);
  IRpyScriptInterpreter::Execute_StartUp(InterpreterObject);

  if (IsValid(VNPlayer) && !VNPlayer->IsInViewport())
    VNPlayer->AddToViewport();
  if (bSetUIInputMode && IsValid(VNPlayer) && PlayerController)
  {
    PlayerController->bShowMouseCursor = true;
    PlayerController->bEnableClickEvents = true;
    PlayerController->bEnableMouseOverEvents = true;
    UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, VNPlayer);
  }
  return true;
}

void URpyComponent::HandleSessionScriptComplete()
{
  OnScriptComplete.Broadcast();
  if (bCloseOnScriptComplete && IsValid(Session))
    IRpyMainComponent::Execute_CloseVN(this);
}

void URpyComponent::CloseVN_Implementation()
{
  if (IsValid(Session))
    Session->OnScriptComplete.RemoveDynamic(this, &URpyComponent::HandleSessionScriptComplete);
  if (IsValid(Interpreter.GetObject()))
    IRpyScriptInterpreter::Execute_Shutdown(Interpreter.GetObject());
  const bool bHadPlayer = IsValid(VNPlayer);
  if (bHadPlayer)
  {
    VNPlayer->RemoveFromParent();
    // The interpreter was the player widget; the next start creates a new one.
    if (Interpreter.GetObject() == VNPlayer)
      Interpreter = nullptr;
  }
  VNPlayer = nullptr;
  Session = nullptr;
  ActiveScript = nullptr;
  if (bHadPlayer && bSetUIInputMode)
  {
    if (APlayerController *PlayerController = FindPlayerController())
    {
      PlayerController->bShowMouseCursor = false;
      UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
    }
  }
  // Copy: a listener may register another listener while handling the event.
  const TArray<TScriptInterface<IRpyCloseVNEventListener>> Listeners = CloseListeners;
  for (const TScriptInterface<IRpyCloseVNEventListener> &Listener : Listeners)
    if (IsValid(Listener.GetObject()))
      IRpyCloseVNEventListener::Execute_OnCloseVNEvent(Listener.GetObject());
  OnVNClosed.Broadcast();
}

void URpyComponent::AddOnCloseVNEventListener_Implementation(const TScriptInterface<IRpyCloseVNEventListener> &Listener)
{
  if (IsValid(Listener.GetObject()))
    CloseListeners.AddUnique(Listener);
}

void URpyComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  // Do not leave the VN player on screen after the owner leaves the world.
  if (IsValid(VNPlayer))
    VNPlayer->RemoveFromParent();
  VNPlayer = nullptr;
  Session = nullptr;
  ActiveScript = nullptr;
  Super::EndPlay(EndPlayReason);
}

URpyState *URpyComponent::SaveToState_Implementation(UObject *Outer)
{
  URpyComponentState *ComponentState = NewObject<URpyComponentState>(Outer ? Outer : this);
  if (!ComponentState)
    return nullptr;
  ComponentState->ComponentName = GetFName();
  if (IsValid(Session))
    ComponentState->rpySessionState = Cast<URpySessionState>(IRpyStateful::Execute_SaveToState(Session, ComponentState));
  return ComponentState;
}

void URpyComponent::LoadFromState_Implementation(const URpyState *State)
{
  const URpyComponentState *ComponentState = Cast<URpyComponentState>(State);
  if (!ComponentState)
  {
    UE_LOG(LogTemp, Error, TEXT("%s: LoadFromState expects a URpyComponentState."), *GetName());
    return;
  }
  if (!ComponentState->rpySessionState || !SessionClass)
    return;
  if (IsValid(Session))
    IRpyMainComponent::Execute_CloseVN(this);
  Session = NewObject<URpySession>(this, SessionClass);
  if (!InitializeInterpreterForSession(Session))
  {
    Session = nullptr;
    return;
  }
  IRpyStateful::Execute_LoadFromState(Session, ComponentState->rpySessionState);
}
