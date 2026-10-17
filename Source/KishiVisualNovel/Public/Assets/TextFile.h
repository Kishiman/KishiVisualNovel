// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Assets/KishiDataAssetBase.h"
#include "TextFile.generated.h"

/**
 *
 */
UCLASS()
class KISHIVISUALNOVEL_API UTextFile : public UKishiDataAssetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FText text;
};
