#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

#include "RpyStateful.generated.h"

class URpyState;
USTRUCT(BlueprintType)
struct FRpySerializedState
{
  GENERATED_BODY()

  // Which class this state represents
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TSubclassOf<URpyState> StateClass;

  // Raw serialized data of that UObject
  UPROPERTY()
  TArray<uint8> SerializedData;
};

UCLASS(BlueprintType)
class URpyState : public UObject
{
  GENERATED_BODY()
public:
  // Printed name for the state
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FName StateName;
  // debug
  UFUNCTION(BlueprintCallable)
  virtual void PrintDebug() const
  {
    UE_LOG(LogTemp, Warning, TEXT("State: %s"), *StateName.ToString());
  }

  static void SerializeStateObject(URpyState *State, FRpySerializedState &OutStruct)
  {
    if (!State)
      return;

    OutStruct.StateClass = State->GetClass();

    FMemoryWriter Writer(OutStruct.SerializedData, true);
    FObjectAndNameAsStringProxyArchive Ar(Writer, false);
    Ar.ArIsSaveGame = false;
    State->Serialize(Ar);
  }

  static URpyState *DeserializeStateObject(const FRpySerializedState &InStruct, UObject *Outer)
  {
    if (!InStruct.StateClass)
      return nullptr;

    URpyState *NewState = NewObject<URpyState>(Outer, InStruct.StateClass);

    FMemoryReader Reader(InStruct.SerializedData, true);
    FObjectAndNameAsStringProxyArchive Ar(Reader, true);
    Ar.ArIsSaveGame = false;
    NewState->Serialize(Ar);

    return NewState;
  }
};

UINTERFACE(Blueprintable)

class KISHIVISUALNOVEL_API URpyStateful : public UInterface
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyStateful
{
  GENERATED_BODY()

public:
  // get State Class
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
  TSubclassOf<URpyState> GetStateClass();
  virtual TSubclassOf<URpyState> GetStateClass_Implementation() = 0;

  // Save state to State
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
  URpyState *SaveToState(UObject *Outer);
  virtual URpyState *SaveToState_Implementation(UObject *Outer) = 0;

  // Load state from State
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
  void LoadFromState(const URpyState *State);
  virtual void LoadFromState_Implementation(const URpyState *State) = 0;
};
