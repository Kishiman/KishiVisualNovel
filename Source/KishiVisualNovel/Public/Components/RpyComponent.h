#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Components/ActorComponent.h"
#include "Interfaces/RpyStatefulComponent.h"
#include "Interfaces/RpyMainComponent.h"
#include "Interfaces/RpySaveGame.h"
#include "Interfaces/RpyStateful.h"
#include "Interfaces/RpyScriptInterpreter.h"

#include "Rpy/RpyScript.h"
#include "Rpy/RpySession.h"

#include "RpyComponent.generated.h"

UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API URpyComponentState : public URpyStatefulComponentState
{
  GENERATED_BODY()
public:
  void PostInitProperties() override
  {
    Super::PostInitProperties();

    StateName = "RpyComponentState";
  }
  virtual void PrintDebug() const override
  {
    Super::PrintDebug();
    if (rpySessionState)
    {
      UE_LOG(LogTemp, Warning, TEXT("RpySessionState:"));
      rpySessionState->PrintDebug();
    }
    else
    {
      UE_LOG(LogTemp, Warning, TEXT("RpySessionState: nullptr"));
    }
  }
  virtual void Serialize(FArchive &Ar) override
  {
    Super::Serialize(Ar);

    if (Ar.IsSaving())
    {
      bool bHasSession = (rpySessionState != nullptr);
      Ar << bHasSession;

      if (bHasSession)
      {
        // Serialize the class and its data manually
        FString SessionClassPath = rpySessionState->GetClass()->GetPathName();
        Ar << SessionClassPath;

        TArray<uint8> Bytes;
        FMemoryWriter MemoryWriter(Bytes, true);
        FObjectAndNameAsStringProxyArchive ProxyAr(MemoryWriter, true);
        ProxyAr.ArIsSaveGame = Ar.ArIsSaveGame;
        rpySessionState->Serialize(ProxyAr);
        Ar << Bytes;
      }
    }
    else if (Ar.IsLoading())
    {
      bool bHasSession = false;
      Ar << bHasSession;

      if (bHasSession)
      {
        FString SessionClassPath;
        Ar << SessionClassPath;
        UClass *SessionClass = LoadObject<UClass>(nullptr, *SessionClassPath);

        rpySessionState = NewObject<URpySessionState>(this, SessionClass);

        TArray<uint8> Bytes;
        Ar << Bytes;

        FMemoryReader MemoryReader(Bytes, true);
        FObjectAndNameAsStringProxyArchive ProxyAr(MemoryReader, true);
        ProxyAr.ArIsSaveGame = Ar.ArIsSaveGame;

        rpySessionState->Serialize(ProxyAr);
      }
    }
  }
  // rpy state
  UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
  URpySessionState *rpySessionState;
};

class UUserWidget;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRpyVNClosed);

/**
 * Hosts a VN on any actor. Component counterpart of ARpyMainActorBase: creates the VN player widget, runs the session, notifies close listeners,
 * and saves the session with the owner's component states. Start scripts on an actor through URpyMainComponentLibrary::StartRpyScriptOnActor
 * so callers work with both this component and IRpyMainActor actors.
 */
UCLASS(ClassGroup = (VisualNovel), meta = (BlueprintSpawnableComponent, DisplayName = "VN Main Component"), BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API URpyComponent : public UActorComponent, public IRpyMainComponent
{
  GENERATED_BODY()

public:
  URpyComponent();

  virtual bool IsVNActive_Implementation() const override;
  virtual URpySession *GetCurrentSession_Implementation() const override;
  virtual void StartRpyScript_Implementation(URpyScript *Script, FName const &Label) override;
  virtual void CloseVN_Implementation() override;
  virtual void AddOnCloseVNEventListener_Implementation(const TScriptInterface<IRpyCloseVNEventListener> &Listener) override;
  virtual TSubclassOf<URpyState> GetStateClass_Implementation() override { return URpyComponentState::StaticClass(); }
  virtual URpyState *SaveToState_Implementation(UObject *Outer) override;
  virtual void LoadFromState_Implementation(const URpyState *State) override;

  UFUNCTION(BlueprintPure, Category = "VN Main Component")
  UUserWidget *GetVNPlayer() const { return VNPlayer; }

  /** Optional. Broadcast when the running script reaches its end. */
  UPROPERTY(BlueprintAssignable, Category = "VN Main Component|Hooks")
  FOnScriptComplete OnScriptComplete;
  /** Optional. Broadcast after CloseVN, once the close listeners have been notified. */
  UPROPERTY(BlueprintAssignable, Category = "VN Main Component|Hooks")
  FOnRpyVNClosed OnVNClosed;

protected:
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  /** Creates the VN player when needed, hands the session to the interpreter and shows the player. */
  bool InitializeInterpreterForSession(URpySession *TargetSession);
  /** The owner's player controller when the owner is a possessed pawn, else the first local player controller. */
  APlayerController *FindPlayerController() const;
  UFUNCTION()
  void HandleSessionScriptComplete();

  /** Widget class that implements IRpyScriptInterpreter, such as VN2D_RPYPlayer. Leave unset to use Interpreter instead. */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VN Main Component")
  TSubclassOf<UUserWidget> VNPlayerClass;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VN Main Component")
  TSubclassOf<URpySession> SessionClass;
  /** Interpreter used when VNPlayerClass is unset or does not implement IRpyScriptInterpreter. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VN Main Component")
  TScriptInterface<IRpyScriptInterpreter> Interpreter;
  /** Close the VN when the script reaches its end. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VN Main Component")
  bool bCloseOnScriptComplete = true;
  /** Show the mouse cursor and switch to Game and UI input while the VN player is open, and back to Game input on close. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VN Main Component")
  bool bSetUIInputMode = true;

  UPROPERTY(BlueprintReadOnly, Category = "VN Main Component")
  URpyScript *ActiveScript = nullptr;
  UPROPERTY(BlueprintReadOnly, Category = "VN Main Component")
  URpySession *Session = nullptr;
  UPROPERTY(BlueprintReadOnly, Category = "VN Main Component")
  UUserWidget *VNPlayer = nullptr;
  UPROPERTY()
  TArray<TScriptInterface<IRpyCloseVNEventListener>> CloseListeners;
};

