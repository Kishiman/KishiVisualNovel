// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EditorReimportHandler.h"
#include "KishiDataAssetImporterFactory.h"
// #include "KishiDataAssetReimportHandler.generated.h"

/**
 * 
 */
// UCLASS()
class KISHIVISUALNOVELIMPORTER_API FKishiDataAssetReimportHandler :public FReimportHandler
{
	// GENERATED_UCLASS_BODY()

	//~ Begin FReimportHandler Interface
public:
	FKishiDataAssetReimportHandler():FReimportHandler(){}
	FKishiDataAssetReimportHandler(UKishiDataAssetImporterFactory* factory):FReimportHandler(){AsFactory=factory;}
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	virtual int32 GetPriority() const override;
	protected:
	UKishiDataAssetImporterFactory* AsFactory;
	//~ End FReimportHandler Interface
};
