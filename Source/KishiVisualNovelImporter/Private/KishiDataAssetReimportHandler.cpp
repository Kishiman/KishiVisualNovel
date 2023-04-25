// Fill out your copyright notice in the Description page of Project Settings.


#include "KishiDataAssetReimportHandler.h"
#include "KishiVisualNovelImporterLog.h"
// #include "TextFile.h"
#include "HAL/FileManager.h"
#include "EditorFramework/AssetImportData.h"
//////////////////////////////////////////////////////////////////////////
// FKishiDataAssetReimportHandler

// FKishiDataAssetReimportHandler::FKishiDataAssetReimportHandler(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// {
// 	SupportedClass = UTextFile::StaticClass();
// 	bCreateNew = false;
// }

bool FKishiDataAssetReimportHandler::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	if (Obj->GetClass() == AsFactory->SupportedClass)
	{
		UKishiDataAsset* kishiDataAsset = (UKishiDataAsset*)(Obj);
		kishiDataAsset->AssetImportData->ExtractFilenames(OutFilenames);
		return true;
	}
	return false;
}

void FKishiDataAssetReimportHandler::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UKishiDataAsset* kishiDataAsset = (UKishiDataAsset*)(Obj);
	if (kishiDataAsset && ensure(NewReimportPaths.Num() == 1))
	{
		kishiDataAsset->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type FKishiDataAssetReimportHandler::Reimport(UObject* Obj)
{
	UKishiDataAsset* kishiDataAsset = (UKishiDataAsset*)(Obj);
	if (!kishiDataAsset)
	{
		return EReimportResult::Failed;
	}

	// Make sure file is valid and exists
	const FString Filename = kishiDataAsset->AssetImportData->GetFirstFilename();
	if (!Filename.Len() || IFileManager::Get().FileSize(*Filename) == INDEX_NONE)
	{
		return EReimportResult::Failed;
	}
	// Run the import again
	EReimportResult::Type Result = EReimportResult::Failed;
	bool OutCanceled = false;

	if (AsFactory->ImportObject(kishiDataAsset->GetClass(), kishiDataAsset->GetOuter(), *kishiDataAsset->GetName(), RF_Public | RF_Standalone, Filename, nullptr, OutCanceled) != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Imported successfully"));

		kishiDataAsset->AssetImportData->Update(Filename);

		// Try to find the outer package so we can dirty it up
		if (kishiDataAsset->GetOuter())
		{
			kishiDataAsset->GetOuter()->MarkPackageDirty();
		}
		else
		{
			kishiDataAsset->MarkPackageDirty();
		}
		Result = EReimportResult::Succeeded;
	}
	else
	{
		if (OutCanceled)
		{
			UE_LOG(LogTemp, Warning, TEXT("-- import canceled"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("-- import failed"));
		}

		Result = EReimportResult::Failed;
	}

	return Result;
}

int32 FKishiDataAssetReimportHandler::GetPriority() const
{
	return AsFactory->ImportPriority;
}

//////////////////////////////////////////////////////////////////////////

