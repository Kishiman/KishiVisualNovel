#include "Interfaces/LayeredSpriteHandler.h"
#include "LayeredSprite.h"

void ULayeredSpriteHandlerLibrary::GetLayersToDisplayByAttribute(const TScriptInterface<ILayeredSpriteHandler> &Target, FString Attribute, TArray<FName> &NamesToShow, TArray<FName> &NamesToHide)
{
    ULayeredSprite *sprite = ILayeredSpriteHandler::Execute_GetSprite(Target.GetObject());
    TArray<FName> Names;
    TArray<FName> InvertNames;
    sprite->FindNamesByAttribute(Attribute, Names, InvertNames);
    for (const auto &layer : sprite->layers)
    {
        bool displayed = ILayeredSpriteHandler::Execute_GetLayerDisplay(Target.GetObject(), layer.name);
        if (Names.Contains(layer.name) && !displayed)
        {
            NamesToShow.Add(layer.name);
        }
        else if (InvertNames.Contains(layer.name) && displayed)
        {
            NamesToHide.Add(layer.name);
        }
    }
    return;
}

void ULayeredSpriteHandlerLibrary::SetLayersDisplayByAttribute(const TScriptInterface<ILayeredSpriteHandler> &Target, FString Attribute, TArray<FName> &ShownNames, TArray<FName> &HidenNames)
{
    ULayeredSprite *sprite = ILayeredSpriteHandler::Execute_GetSprite(Target.GetObject());

    GetLayersToDisplayByAttribute(Target, Attribute, ShownNames, HidenNames);

    for (auto &name : ShownNames)
    {
        ILayeredSpriteHandler::Execute_SetLayerDisplay(Target.GetObject(), name, true);
    }

    for (auto &name : HidenNames)
    {
        ILayeredSpriteHandler::Execute_SetLayerDisplay(Target.GetObject(), name, false);
    }
}
