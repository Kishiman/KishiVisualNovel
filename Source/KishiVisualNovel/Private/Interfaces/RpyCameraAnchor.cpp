#include "Interfaces/RpyCameraAnchor.h"
#include "Utils/ActorUtils.h"

TScriptInterface<IRpyCameraAnchor> URpyCameraAnchorLibrary::FindAnchorByLayerName(UWorld *World, FName Layer)
{
    TArray<AActor *> AllActors = UActorUtils::GetAllActorsWithInterfaceAndTag(World, URpyCameraAnchor::StaticClass(), Layer);

    if (AllActors.Num() > 0)
    {
        return AllActors[0];
    }
    return nullptr;
}
