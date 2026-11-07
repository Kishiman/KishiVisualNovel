#include "Interfaces/RpyCameraAnchor.h"
#include "EngineUtils.h"
#include "Utils/ActorUtils.h"

TScriptInterface<IRpyCameraAnchor> URpyCameraAnchorLibrary::FindAnchorByLayerName(UWorld *World, FName Layer)
{
    if (!World || Layer.IsNone())
        return nullptr;

    // Actor anchors are matched by actor tag, as before.
    TArray<AActor *> AllActors = UActorUtils::GetAllActorsWithInterfaceAndTag(World, URpyCameraAnchor::StaticClass(), Layer);
    if (AllActors.Num() > 0)
    {
        return AllActors[0];
    }

    // Component anchors are matched by their anchor layer.
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        for (UActorComponent *Component : It->GetComponents())
        {
            if (IsValid(Component) && Component->GetClass()->ImplementsInterface(URpyCameraAnchor::StaticClass()) &&
                IRpyCameraAnchor::Execute_GetAnchorLayer(Component) == Layer)
            {
                return Component;
            }
        }
    }
    return nullptr;
}
