// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KishiDataAssetReimportHandler.h"
#include "Rpy/RpyScript.h"
#include "RpyScriptImporterFactory.generated.h"

/**
 */
UCLASS()
class KISHIVISUALNOVELIMPORTER_API URpyScriptImporterFactory : public UKishiDataAssetImporterFactory, public FKishiDataAssetReimportHandler
{
	GENERATED_UCLASS_BODY()
	// UFactory interface
	virtual FText GetToolTip() const override;
	virtual bool FactoryUpdateString(UClass *InClass, UObject *Object, FString &content) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	uint8 TabSize = 4;
};
