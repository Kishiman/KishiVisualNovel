#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/Interface.h"

#include "Interfaces/RpyMainActor.h"
#include "Interfaces/RpyStatefulComponent.h"

#include "RpyMainComponent.generated.h"

class URpyScript;
class URpySession;

/**
 * Component counterpart of IRpyMainActor: runs VN sessions from a component, so any actor (Actor, Pawn, Character) can host the VN.
 * It derives from IRpyStatefulComponent so the save game stores it with the other stateful components.
 * The activity check is named IsVNActive because UActorComponent::IsActive already exists and reports component activation.
 */
UINTERFACE(Blueprintable)
class KISHIVISUALNOVEL_API URpyMainComponent : public URpyStatefulComponent
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyMainComponent : public IRpyStatefulComponent
{
  GENERATED_BODY()

public:
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyMainComponent|Required", meta = (ToolTip = "Required: return true while a VN session runs on this component."))
  bool IsVNActive() const;
  virtual bool IsVNActive_Implementation() const
  {
    UE_LOG(LogTemp, Error, TEXT("Required implementation missing: IRpyMainComponent::IsVNActive"));
    return false;
  }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyMainComponent|Required", meta = (ToolTip = "Required: return the running VN session, or None."))
  URpySession *GetCurrentSession() const;
  virtual URpySession *GetCurrentSession_Implementation() const
  {
    UE_LOG(LogTemp, Error, TEXT("Required implementation missing: IRpyMainComponent::GetCurrentSession"));
    return nullptr;
  }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyMainComponent|Required", meta = (ToolTip = "Required: start Script at Label, closing any running session first."))
  void StartRpyScript(URpyScript *Script, FName const &Label);
  virtual void StartRpyScript_Implementation(URpyScript *Script, FName const &Label)
  {
    UE_LOG(LogTemp, Error, TEXT("Required implementation missing: IRpyMainComponent::StartRpyScript"));
  }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyMainComponent|Required", meta = (ToolTip = "Required: shut down the session and VN player, then notify the close listeners."))
  void CloseVN();
  virtual void CloseVN_Implementation()
  {
    UE_LOG(LogTemp, Error, TEXT("Required implementation missing: IRpyMainComponent::CloseVN"));
  }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyMainComponent|Required", meta = (ToolTip = "Required: call Listener's OnCloseVNEvent when the VN closes."))
  void AddOnCloseVNEventListener(const TScriptInterface<IRpyCloseVNEventListener> &Listener);
  virtual void AddOnCloseVNEventListener_Implementation(const TScriptInterface<IRpyCloseVNEventListener> &Listener)
  {
    UE_LOG(LogTemp, Error, TEXT("Required implementation missing: IRpyMainComponent::AddOnCloseVNEventListener"));
  }
};

/**
 * Starts and closes VNs on an actor without knowing whether the actor implements IRpyMainActor itself or carries an IRpyMainComponent.
 * The actor's own IRpyMainActor wins; otherwise the first IRpyMainComponent on the actor is used.
 */
UCLASS()
class KISHIVISUALNOVEL_API URpyMainComponentLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:
  /** Returns the first component of Actor that implements IRpyMainComponent, or None. */
  UFUNCTION(BlueprintPure, Category = "Rpy|Main")
  static TScriptInterface<IRpyMainComponent> FindMainComponent(AActor *Actor);

  /** Returns the object that hosts the VN for Actor: Actor itself when it implements IRpyMainActor, else its IRpyMainComponent, else None. */
  UFUNCTION(BlueprintPure, Category = "Rpy|Main")
  static UObject *FindVNHost(AActor *Actor);

  /** Starts Script at Label on Actor's VN host. Returns false and logs an error when Actor has no VN host. */
  UFUNCTION(BlueprintCallable, Category = "Rpy|Main")
  static bool StartRpyScriptOnActor(AActor *Actor, URpyScript *Script, FName Label);

  /** Closes the VN on Actor's VN host. */
  UFUNCTION(BlueprintCallable, Category = "Rpy|Main")
  static void CloseVNOnActor(AActor *Actor);

  /** Returns true while Actor's VN host runs a session. */
  UFUNCTION(BlueprintPure, Category = "Rpy|Main")
  static bool IsVNActiveOnActor(AActor *Actor);

  /** Returns the session running on Actor's VN host, or None. */
  UFUNCTION(BlueprintPure, Category = "Rpy|Main")
  static URpySession *GetCurrentSessionOnActor(AActor *Actor);

  /** Registers Listener with Actor's VN host. Returns false when Actor has no VN host or Listener does not implement IRpyCloseVNEventListener. */
  UFUNCTION(BlueprintCallable, Category = "Rpy|Main")
  static bool AddOnCloseVNEventListenerOnActor(AActor *Actor, UObject *Listener);
};
