#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/RpyMainActor.h"
#include "Interfaces/RpyStatefulActor.h"
#include "RpyCloseVNEventListenerBase.generated.h"

/** Blueprint-extensible listener actor with the standard actor save-state behavior. */
UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API ARpyCloseVNEventListenerBase : public AActor, public IRpyCloseVNEventListener
{
	GENERATED_BODY()

public:
	virtual void OnCloseVNEvent_Implementation() override;
	virtual TSubclassOf<URpyState> GetStateClass_Implementation() override;
	virtual URpyState* SaveToState_Implementation(UObject* Outer) override;
	virtual void LoadFromState_Implementation(const URpyState* State) override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy")
	void OnVNClosed();
};
