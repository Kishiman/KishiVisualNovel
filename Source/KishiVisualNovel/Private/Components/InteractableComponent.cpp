#include "Components/InteractableComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/InteractingComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/Interactable.h"
#include "Kismet/GameplayStatics.h"

UInteractableComponent::UInteractableComponent()
{
  // Ticks only while focused, to move the prompt.
  PrimaryComponentTick.bCanEverTick = true;
  PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
  UpdatePromptPosition();
}

void UInteractableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  RemovePrompt();
  Super::EndPlay(EndPlayReason);
}

AActor *UInteractableComponent::GetInteractingActor() const
{
  return CurrentInteractingComponent ? CurrentInteractingComponent->GetOwner() : nullptr;
}

void UInteractableComponent::SetIsFocused(bool bFocused)
{
  bIsFocused = bFocused;
  SetComponentTickEnabled(bFocused);
  if (bFocused)
    ShowPrompt();
  else if (IsValid(InteractionWidget))
    InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
  OnFocusChanged.Broadcast(bFocused);
  for (UObject *Handler : UInteractableLibrary::FindInteractableHandlers(GetOwner()))
    IInteractable::Execute_OnInteractableFocusChanged(Handler, bFocused);
}

void UInteractableComponent::SetIsNearby(bool bNearby)
{
  bIsNearby = bNearby;
  OnNearbyChanged.Broadcast(bNearby);
  for (UObject *Handler : UInteractableLibrary::FindInteractableHandlers(GetOwner()))
    IInteractable::Execute_OnInteractableNearbyChanged(Handler, bNearby);
}

void UInteractableComponent::EndInteraction()
{
  auto interactingComponent = this->CurrentInteractingComponent;
  if (!interactingComponent)
    return;
  UObject *Handler = ActiveHandler;
  this->CurrentInteractingComponent = nullptr;
  ActiveHandler = nullptr;
  // Before the broadcast, so a handler can still move the state on, for example to Disabled.
  if (State == EInteractableState::Interacting)
    State = EInteractableState::Interactable;
  OnInteractionEnd.Broadcast(interactingComponent, this);
  if (IsValid(Handler))
    IInteractable::Execute_HandleInteractionEnd(Handler, interactingComponent, this);
}

bool UInteractableComponent::CanInteract(FName ActionName, const UInteractingComponent *InteractingComponent) const
{
  if (State != EInteractableState::Interactable || ActionName == NAME_None || InteractingComponent == nullptr)
    return false;
  return Actions.Num() == 0 || Actions.ContainsByPredicate([ActionName](const FInteractableActorAction &Action) { return Action.ActionName == ActionName; });
}

void UInteractableComponent::Interact(FName ActionName, UInteractingComponent *InteractingComponent)
{
  if (!CanInteract(ActionName, InteractingComponent))
    return;
  this->CurrentInteractingComponent = InteractingComponent;

  State = EInteractableState::Interacting;
  OnInteractionStart.Broadcast(ActionName, InteractingComponent, this);
  // A delegate listener may already have ended the interaction.
  if (this->CurrentInteractingComponent != InteractingComponent)
    return;
  ActiveHandler = FindHandlerForAction(ActionName);
  UE_LOG(LogTemp, Warning, TEXT("Interact '%s' on %s: handler %s."), *ActionName.ToString(), *GetNameSafe(GetOwner()), *GetNameSafe(ActiveHandler));
  if (ActiveHandler)
    IInteractable::Execute_HandleInteractionStart(ActiveHandler, ActionName, InteractingComponent, this);
}

void UInteractableComponent::RegisterAction(FName ActionName, const FText &Label, UObject *Handler)
{
  if (ActionName == NAME_None || !IsValid(Handler) || !Handler->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
  {
    UE_LOG(LogTemp, Error, TEXT("%s on %s: cannot register action '%s' with handler %s, which needs a name and IInteractable."), *GetName(), *GetNameSafe(GetOwner()), *ActionName.ToString(), *GetNameSafe(Handler));
    return;
  }
  if (const UObject *Existing = ActionHandlers.FindRef(ActionName))
  {
    UE_LOG(LogTemp, Error, TEXT("%s on %s: action '%s' is already handled by %s, so %s is ignored. Give each handler its own action name."), *GetName(), *GetNameSafe(GetOwner()), *ActionName.ToString(), *GetNameSafe(Existing), *GetNameSafe(Handler));
    return;
  }
  ActionHandlers.Add(ActionName, Handler);

  FInteractableActorAction *Action = Actions.FindByPredicate([ActionName](const FInteractableActorAction &Entry) { return Entry.ActionName == ActionName; });
  if (!Action)
  {
    Action = &Actions.AddDefaulted_GetRef();
    Action->ActionName = ActionName;
    Action->Label = FText::FromName(ActionName);
  }
  if (!Label.IsEmpty())
    Action->Label = Label;
  // The next focus builds a prompt that shows this action.
  RemovePrompt();
}

UObject *UInteractableComponent::FindHandlerForAction(FName ActionName) const
{
  if (UObject *Registered = ActionHandlers.FindRef(ActionName))
    return Registered;
  for (UObject *Handler : UInteractableLibrary::FindInteractableHandlers(GetOwner()))
    if (!ActionHandlers.FindKey(Handler))
      return Handler;
  return nullptr;
}

void UInteractableComponent::ShowPrompt()
{
  if (!InteractionWidgetClass)
    return;
  if (!IsValid(InteractionWidget))
  {
    APlayerController *PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    InteractionWidget = PlayerController ? CreateWidget<UUserWidget>(PlayerController, InteractionWidgetClass) : CreateWidget<UUserWidget>(GetWorld(), InteractionWidgetClass);
    if (!IsValid(InteractionWidget))
    {
      UE_LOG(LogTemp, Error, TEXT("%s on %s: could not create widget %s."), *GetName(), *GetNameSafe(GetOwner()), *GetNameSafe(InteractionWidgetClass));
      return;
    }
    InteractionWidget->AddToViewport();
    if (InteractionWidget->GetClass()->ImplementsInterface(UInteractionPromptWidget::StaticClass()))
      IInteractionPromptWidget::Execute_HandleInteractionPromptSetup(InteractionWidget, InteractableName, GetAvailableActions());
    else
      UE_LOG(LogTemp, Error, TEXT("%s on %s: widget %s does not implement IInteractionPromptWidget, so it cannot show the actions."), *GetName(), *GetNameSafe(GetOwner()), *GetNameSafe(InteractionWidgetClass));
  }
  InteractionWidget->SetVisibility(ESlateVisibility::Visible);
  UpdatePromptPosition();
}

void UInteractableComponent::RemovePrompt()
{
  if (IsValid(InteractionWidget))
    InteractionWidget->RemoveFromParent();
  InteractionWidget = nullptr;
}

void UInteractableComponent::UpdatePromptPosition()
{
  if (!IsValid(InteractionWidget))
    return;
  FVector2D ScreenPosition;
  if (UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(this, 0), GetComponentLocation(), ScreenPosition, false))
    InteractionWidget->SetPositionInViewport(ScreenPosition, true);
}
