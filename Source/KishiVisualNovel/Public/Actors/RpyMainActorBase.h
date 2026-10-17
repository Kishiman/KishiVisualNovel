#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/RpyMainActor.h"
#include "Interfaces/RpyStatefulActor.h"
#include "Rpy/RpySession.h"
#include "RpyMainActorBase.generated.h"

class URpyScript;
class UUserWidget;

/** Native starting point for Blueprint VN actors. */
UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API ARpyMainActorBase : public AActor, public IRpyMainActor
{
	GENERATED_BODY()

public:
	ARpyMainActorBase();

	virtual bool IsActive_Implementation() const override;
	virtual URpySession* GetCurrentSession_Implementation() const override;
	virtual void StartRpyScript_Implementation(URpyScript* Script, FName const& Label) override;
	virtual void CloseVN_Implementation() override;
	virtual void AddOnCloseVNEventListener_Implementation(const TScriptInterface<IRpyCloseVNEventListener>& Listener) override;
	virtual TSubclassOf<URpyState> GetStateClass_Implementation() override;
	virtual URpyState* SaveToState_Implementation(UObject* Outer) override;
	virtual void LoadFromState_Implementation(const URpyState* State) override;

	UFUNCTION(BlueprintPure, Category="Rpy")
	UUserWidget* GetVNPlayer() const { return VNPlayer; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rpy")
	TSubclassOf<UUserWidget> VNPlayerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rpy")
	TSubclassOf<URpySession> SessionClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rpy")
	TScriptInterface<IRpyScriptInterpreter> Interpreter;

	UPROPERTY(BlueprintReadOnly, Category="Rpy")
	URpyScript* ActiveScript = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Rpy")
	URpySession* Session = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Rpy")
	UUserWidget* VNPlayer = nullptr;

	UPROPERTY()
	TArray<TScriptInterface<IRpyCloseVNEventListener>> CloseListeners;

	UFUNCTION(BlueprintImplementableEvent, Category="Rpy")
	void OnVNClosed();
};
