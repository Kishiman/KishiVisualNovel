#include "Components/RpyComponent.h"

URpyComponent::URpyComponent()
{
}

URpyState *URpyComponent::SaveToState_Implementation(UObject *Outer)
{
  URpyComponentState *ComponentState = NewObject<URpyComponentState>(Outer);
  if (!ComponentState)
  {
    UE_LOG(LogTemp, Error, TEXT("URpyComponent::BaseSaveToState_Implementation: State is not URpyComponentState"));
    return nullptr;
  }
  ComponentState->ComponentName = GetFName();
  if (CurrentSession)
  {
    ComponentState->rpySessionState = Cast<URpySessionState>(CurrentSession->SaveToState_Implementation(Outer));
  }
  return ComponentState;
}

void URpyComponent::LoadFromState_Implementation(const URpyState *State)
{
  auto ComponentState = Cast<URpyComponentState>(State);
  if (!ComponentState)
  {
    UE_LOG(LogTemp, Error, TEXT("URpyComponent::LoadFromState_Implementation: State is not URpyComponentState"));
    return;
  }

  if (ComponentState->rpySessionState)
  {
    if (!CurrentSession)
    {
      CurrentSession = NewObject<URpySession>(this);
    }
    CurrentSession->LoadFromState_Implementation(ComponentState->rpySessionState);
  }
}

void URpyComponent::BeginPlay()
{
  Super::BeginPlay();
}

void URpyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (!CurrentSession)
    return;
}

void URpyComponent::StartScript(URpyScript *Script, FName const &Label)
{
  if (!CurrentInterpreter)
  {
    if (!*InterpreterClass)
    {
      UE_LOG(LogTemp, Error, TEXT("URpyComponent::StartScript: No InterpreterClass set"));
      return;
    }
    CurrentInterpreter = TScriptInterface<IRpyScriptInterpreter>(NewObject<URpyScriptInterpreter>(this, InterpreterClass));
  }
  if (!CurrentSession)
  {
    CurrentSession = NewObject<URpySession>(this);
    CurrentSession->interpreter = CurrentInterpreter;
    CurrentSession->AddScript(Script);
  }
  CurrentSession->StartLabel(Label);
  CurrentSession->OnScriptComplete.AddDynamic(this, &URpyComponent::CloseScript);
}
void URpyComponent::CloseScript()
{
  if (CurrentInterpreter)
  {
    IRpyScriptInterpreter::Execute_Shutdown(CurrentInterpreter.GetObject());
    CurrentInterpreter = nullptr;
  }
  OnScriptComplete.Broadcast();
}