#pragma once

#include "CoreMinimal.h"
#include "Components/RpyComponent.h"
#include "Utils/PlayerControllerUtils.h"
#include "RpyFirstPersonComponent.generated.h"

class APlayerController;
class UCursorOverlayWidget;
class UInputComponent;
class UInteractableComponent;
class UInteractingComponent;

/**
 * The VN side of the legacy RpyFirstPersonCharacter, for a pawn that already has its own movement and camera. Add this one component to the pawn.
 * It is a VN Main Component that also binds the interact action, uses the owner's Interacting component (or creates one), locks player input while
 * a VN runs, sets the cursor widget and can start a script when the player takes control.
 */
UCLASS(ClassGroup = (VisualNovel), meta = (BlueprintSpawnableComponent, DisplayName = "VN First Person Component"), BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API URpyFirstPersonComponent : public URpyComponent
{
	GENERATED_BODY()

public:
	URpyFirstPersonComponent();

	virtual void StartRpyScript_Implementation(URpyScript *Script, FName const &Label) override;
	virtual void CloseVN_Implementation() override;
	virtual void LoadFromState_Implementation(const URpyState *State) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "VN First Person Component")
	UInteractingComponent *GetInteractingComponent() const { return Interacting; }

	UFUNCTION(BlueprintPure, Category = "VN First Person Component")
	UCursorOverlayWidget *GetCursorWidget() const { return CursorWidget; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Script to run when the player first takes control of the owner. Leave unset to start scripts only through interactions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VN First Person Component")
	URpyScript *StartScript = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VN First Person Component")
	FName StartLabel = TEXT("start");
	/** Input actions that call interact on the focused interactable, each passing its own name. Add one per handler action, and map it in the project's input settings. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VN First Person Component")
	TArray<FName> InteractActionNames;
	/** Input mode while a VN runs. Use this instead of bSetUIInputMode, which does not stop the player moving. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VN First Person Component")
	EPlayerInputMode VNInputMode = EPlayerInputMode::UIOnly;
	/** Widget used as the default mouse cursor. Leave unset to keep the system cursor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VN First Person Component")
	TSubclassOf<UCursorOverlayWidget> CursorWidgetClass;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "VN First Person Component")
	UInteractingComponent *Interacting = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "VN First Person Component")
	UCursorOverlayWidget *CursorWidget = nullptr;

private:
	/** A pawn gets its input component and controller after BeginPlay when it is possessed later, and a new input component when possessed again. */
	void UpdatePlayerBindings();
	void HandleInteractPressed(FName ActionName);
	void CaptureVNInput();
	void ReleaseVNInput();

	UFUNCTION()
	void HandleInteractionEnded(UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent);

	UPROPERTY(Transient)
	UInputComponent *BoundInputComponent = nullptr;
	bool bPlayerReady = false;
	/** SetIgnoreMoveInput is a counter, so the VN must take and release it exactly once. */
	bool bVNInputCaptured = false;
};
