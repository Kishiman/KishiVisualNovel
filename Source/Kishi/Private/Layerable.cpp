#include "Layerable.h"
#include "Misc/Paths.h" 

bool ULayerableLibrary::HasLayer(const TScriptInterface<ILayerable>& Target,const FString& Layer){
    auto layers=ILayerable::Execute_GetAllLayers(Target.GetObject());
    return layers.Contains(Layer);
};
TSet<FName> ULayerableLibrary::GetSelectiveLayerOptions(const TScriptInterface<ILayerable>& Target,const FString& Path){
    auto selectiveLayers=ILayerable::Execute_GetAllSelectiveLayers(Target.GetObject());
    TSet<FName> result;
    for (auto &&layer : selectiveLayers)
    {
        FString Prefix,option,ext;
        FPaths::Split(layer,Prefix,option,ext);
        if (Path==Prefix)
        {
            result.Add(FName(*option));
        }   
    }
    return result;
};
void ULayerableLibrary::SetSelectiveLayer( TScriptInterface<ILayerable>& Target,const FString& Layer){
    return;
};
void ULayerableLibrary::SetSelectiveLayerOption( TScriptInterface<ILayerable>& Target,const FString& Path, FName Option){
    return;
};