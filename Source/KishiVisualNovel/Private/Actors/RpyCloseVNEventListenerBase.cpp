#include "Actors/RpyCloseVNEventListenerBase.h"

void ARpyCloseVNEventListenerBase::OnCloseVNEvent_Implementation()
{
	OnVNClosed();
}

TSubclassOf<URpyState> ARpyCloseVNEventListenerBase::GetStateClass_Implementation()
{
	return URpyStatefulActorState::StaticClass();
}

URpyState* ARpyCloseVNEventListenerBase::SaveToState_Implementation(UObject* Outer)
{
	URpyStatefulActorState* State = NewObject<URpyStatefulActorState>(Outer ? Outer : this);
	if (!State)
	{
		return nullptr;
	}
	TScriptInterface<IRpyStatefulActor> Target;
	Target.SetObject(this);
	Target.SetInterface(Cast<IRpyStatefulActor>(this));
	URpyStatefulActorLibrary::BaseSaveToState(Target, State);
	return State;
}

void ARpyCloseVNEventListenerBase::LoadFromState_Implementation(const URpyState* State)
{
	const URpyStatefulActorState* ActorState = Cast<URpyStatefulActorState>(State);
	if (!ActorState)
	{
		return;
	}
	TScriptInterface<IRpyStatefulActor> Target;
	Target.SetObject(this);
	Target.SetInterface(Cast<IRpyStatefulActor>(this));
	URpyStatefulActorLibrary::BaseLoadFromState(Target, ActorState);
}
