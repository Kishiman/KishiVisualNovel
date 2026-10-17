// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KishiDataAssetReimportHandler.h"
#include "Assets/LayeredSprite.h"
#include "PSDDataAssetImporterFactory.generated.h"

/**
 *
 */
UCLASS()
class KISHIVISUALNOVELIMPORTER_API UPSDDataAssetImporterFactory : public UKishiDataAssetBaseImporterFactory, public FKishiDataAssetReimportHandler
{
	GENERATED_UCLASS_BODY()
	// UFactory interface
	virtual FText GetToolTip() const override;
	virtual bool FactoryUpdateBinary(UClass *InClass, UObject *Object, const uint8 *data, uint8 size) override;
};