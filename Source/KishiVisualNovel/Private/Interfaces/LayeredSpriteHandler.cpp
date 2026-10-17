#include "Interfaces/LayeredSpriteHandler.h"
#include "Assets/LayeredSprite.h"

void ULayeredSpriteHandlerLibrary::GetLayersToDisplayByAttribute(const TScriptInterface<ILayeredSpriteHandler> &Target, FString Attribute, TArray<FName> &NamesToShow, TArray<FName> &NamesToHide)
{
	NamesToShow.Reset();
	NamesToHide.Reset();
	UObject* TargetObject = Target.GetObject();
	if (!IsValid(TargetObject) || !TargetObject->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()))
	{
		return;
	}

	ULayeredSprite* Sprite = ILayeredSpriteHandler::Execute_GetSprite(TargetObject);
	if (!IsValid(Sprite))
	{
		return;
	}

	TArray<FName> Names;
	TArray<FName> InvertNames;
	Sprite->FindNamesByAttribute(Attribute, Names, InvertNames);
	for (const FSpriteLayer& Layer : Sprite->layers)
    {
		const bool bDisplayed = ILayeredSpriteHandler::Execute_GetLayerDisplay(TargetObject, Layer.name);
		if (Names.Contains(Layer.name) && !bDisplayed)
        {
			NamesToShow.Add(Layer.name);
        }
		else if (InvertNames.Contains(Layer.name) && bDisplayed)
        {
			NamesToHide.Add(Layer.name);
        }
    }
}

void ULayeredSpriteHandlerLibrary::SetLayersDisplayByAttribute(const TScriptInterface<ILayeredSpriteHandler> &Target, FString Attribute, TArray<FName> &ShownNames, TArray<FName> &HidenNames)
{
    GetLayersToDisplayByAttribute(Target, Attribute, ShownNames, HidenNames);
	UObject* TargetObject = Target.GetObject();
	if (!IsValid(TargetObject) || !TargetObject->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()))
	{
		return;
	}

	for (const FName& Name : ShownNames)
    {
		ILayeredSpriteHandler::Execute_SetLayerDisplay(TargetObject, Name, true);
    }

	for (const FName& Name : HidenNames)
    {
		ILayeredSpriteHandler::Execute_SetLayerDisplay(TargetObject, Name, false);
    }
}

FName ULayeredSpriteHandlerLibrary::GetDisplayedLayerInGroup(const TScriptInterface<ILayeredSpriteHandler> &Target, FName GroupName)
{
	UObject* TargetObject = Target.GetObject();
	if (!IsValid(TargetObject) || !TargetObject->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()))
	{
		return NAME_None;
	}

	ULayeredSprite* Sprite = ILayeredSpriteHandler::Execute_GetSprite(TargetObject);
	if (!IsValid(Sprite))
	{
		return NAME_None;
	}

	for (const FSpriteLayer& Layer : Sprite->layers)
    {
		if (Layer.group == GroupName)
        {
			if (ILayeredSpriteHandler::Execute_GetLayerDisplay(TargetObject, Layer.name))
			{
				return Layer.name;
			}
        }
    }
	return NAME_None;
}
