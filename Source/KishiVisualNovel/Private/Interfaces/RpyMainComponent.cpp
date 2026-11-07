#include "Interfaces/RpyMainComponent.h"

#include "GameFramework/Actor.h"
#include "Rpy/RpyScript.h"
#include "Rpy/RpySession.h"

TScriptInterface<IRpyMainComponent> URpyMainComponentLibrary::FindMainComponent(AActor *Actor)
{
  if (!IsValid(Actor))
    return nullptr;
  for (UActorComponent *Component : Actor->GetComponents())
    if (IsValid(Component) && Component->GetClass()->ImplementsInterface(URpyMainComponent::StaticClass()))
      return Component;
  return nullptr;
}

UObject *URpyMainComponentLibrary::FindVNHost(AActor *Actor)
{
  if (!IsValid(Actor))
    return nullptr;
  if (Actor->GetClass()->ImplementsInterface(URpyMainActor::StaticClass()))
    return Actor;
  return FindMainComponent(Actor).GetObject();
}

bool URpyMainComponentLibrary::StartRpyScriptOnActor(AActor *Actor, URpyScript *Script, FName Label)
{
  UObject *Host = FindVNHost(Actor);
  if (!Host)
  {
    UE_LOG(LogTemp, Error, TEXT("StartRpyScriptOnActor: %s implements neither IRpyMainActor nor has an IRpyMainComponent."), *GetNameSafe(Actor));
    return false;
  }
  if (Host == Actor)
    IRpyMainActor::Execute_StartRpyScript(Host, Script, Label);
  else
    IRpyMainComponent::Execute_StartRpyScript(Host, Script, Label);
  return true;
}

void URpyMainComponentLibrary::CloseVNOnActor(AActor *Actor)
{
  UObject *Host = FindVNHost(Actor);
  if (!Host)
    return;
  if (Host == Actor)
    IRpyMainActor::Execute_CloseVN(Host);
  else
    IRpyMainComponent::Execute_CloseVN(Host);
}

bool URpyMainComponentLibrary::IsVNActiveOnActor(AActor *Actor)
{
  UObject *Host = FindVNHost(Actor);
  if (!Host)
    return false;
  return Host == Actor ? IRpyMainActor::Execute_IsActive(Host) : IRpyMainComponent::Execute_IsVNActive(Host);
}

URpySession *URpyMainComponentLibrary::GetCurrentSessionOnActor(AActor *Actor)
{
  UObject *Host = FindVNHost(Actor);
  if (!Host)
    return nullptr;
  return Host == Actor ? IRpyMainActor::Execute_GetCurrentSession(Host) : IRpyMainComponent::Execute_GetCurrentSession(Host);
}

bool URpyMainComponentLibrary::AddOnCloseVNEventListenerOnActor(AActor *Actor, UObject *Listener)
{
  UObject *Host = FindVNHost(Actor);
  if (!Host || !IsValid(Listener) || !Listener->GetClass()->ImplementsInterface(URpyCloseVNEventListener::StaticClass()))
    return false;
  const TScriptInterface<IRpyCloseVNEventListener> ListenerInterface(Listener);
  if (Host == Actor)
    IRpyMainActor::Execute_AddOnCloseVNEventListener(Host, ListenerInterface);
  else
    IRpyMainComponent::Execute_AddOnCloseVNEventListener(Host, ListenerInterface);
  return true;
}
