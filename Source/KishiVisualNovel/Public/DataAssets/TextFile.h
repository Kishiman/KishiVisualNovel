// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KishiDataAsset.h"
#include "TextFile.generated.h"

/**
 * 
 */
UCLASS()
class KISHIVISUALNOVEL_API UTextFile : public UKishiDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
	FText	text;
};
