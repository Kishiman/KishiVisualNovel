// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "KishiDataAsset.h"
#include "KishiDataAssetImporterFactory.generated.h"
/**
 * 
 */
UCLASS()
class KISHIVISUALNOVELIMPORTER_API UKishiDataAssetImporterFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	UKishiDataAssetImporterFactory(const FObjectInitializer& ObjectInitializer,TSubclassOf<UObject> T,char const* ext,char const* desc,bool bnew=true,bool bedit=true,bool btext=true);
	// UFactory interface
	virtual FText GetToolTip() const override;
	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual UObject* FactoryCreateText(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn) override;
	virtual UObject* FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn) override;
	virtual bool FactoryUpdateString(UClass* InClass,UObject* Object, FString& content){
		return false;
		};
	virtual bool FactoryUpdateBinary(UClass* InClass,UObject* Object,const uint8* data,uint8 size){
		return false;
	};
	virtual UObject* FactoryCreateString(UClass* InClass, FString content){return nullptr;};
	// virtual UObject* FactoryCreateBin(UClass* InClass,const uint8* data,uint8 size){return nullptr;};
protected:
	bool bNew=true;
	FString extension;
	FString description;
};
