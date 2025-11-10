#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Components/ActorComponent.h"
#include "Interfaces/RpyStatefulComponent.h"
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

    StateName = "RpyMainActorState";
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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API URpyComponent : public UActorComponent, public IRpyStatefulComponent
{
  GENERATED_BODY()

public:
  virtual TSubclassOf<URpyState> GetStateClass_Implementation() override
  {
    return URpyComponentState::StaticClass();
  }

  virtual URpyState *SaveToState_Implementation(UObject *Outer) override;
  virtual void LoadFromState_Implementation(const URpyState *State) override;

  URpyComponent();
  UPROPERTY(BlueprintAssignable)
  FOnScriptComplete OnScriptComplete;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  TSubclassOf<URpyScriptInterpreter> InterpreterClass;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  URpySession *CurrentSession;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  TScriptInterface<IRpyScriptInterpreter> CurrentInterpreter;

  // Start a VN script
  UFUNCTION(BlueprintCallable, Category = "Rpy")
  void StartScript(URpyScript *Script, FName const &Label);
  // Close the VN script
  UFUNCTION(BlueprintCallable, Category = "Rpy")
  void CloseScript();

protected:
  virtual void BeginPlay() override;

  // Ticks automatically
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};