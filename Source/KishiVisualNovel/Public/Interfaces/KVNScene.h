#pragma once

#include "CoreMinimal.h"
#include "KishiVisualNovelData.h"

#include "KVNScene.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API UKVNSceneHandler : public UInterface
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IKVNSceneHandler
{
    GENERATED_BODY()
public:
    
    //Get the Name of the current Scene
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    FName GetCurrentScene()const;
    virtual FName GetCurrentScene_Implementation()const=0;
    //Check if a scene exists
    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    bool HasScene(FName SceneName,const FVisualNovelParameterCollection& SceneParams)const;
    virtual bool HasScene_Implementation(FName SceneName,const FVisualNovelParameterCollection& SceneParams)const=0;
    //Set a Scene to Load
    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    void SetScene(FName SceneName,const FVisualNovelParameterCollection& SceneParams);
    virtual void SetScene_Implementation(FName SceneName,const FVisualNovelParameterCollection& SceneParams)=0; 

    /*Events*/
    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    void BindOnSceneLoadedEvent(const FVNEvent& Event);
    virtual void BindOnSceneLoadedEvent_Implementation(const FVNEvent& Event)=0;
    
};