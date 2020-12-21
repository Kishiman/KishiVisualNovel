// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "Factories/Factory.h"
#include "KishiDataAssetReimportHandler.h"
#include "KishiVisualNovelScript.h"
#include "KishiVisualNovelScriptImporterFactory.generated.h"

/**
 * 
 */
UCLASS()
class KISHIVISUALNOVELIMPORTER_API UVisualNovelScriptImporterFactory : public UKishiDataAssetImporterFactory,public FKishiDataAssetReimportHandler
{
	GENERATED_UCLASS_BODY()
	// UFactory interface
	virtual FText GetToolTip() const override;
	virtual bool FactoryUpdateString(UClass* InClass,UObject* Object, FString& content)override;
	uint8 TabSize=4;
};
