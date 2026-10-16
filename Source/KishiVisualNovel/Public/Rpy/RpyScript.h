// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KishiDataAsset.h"
#include "Structs/DynamicObject.h"
#include "Rpy/RpyScriptTypes.h"
#include "Rpy/RpySceneTypes.h"
#include "Rpy/RpyManagerStateTypes.h"
#include "Rpy/RpyScriptSerialization.h"

#include "RpyScript.generated.h"

class IRpyScriptInterpreter;
class URpyInstruction;

UCLASS()
class KISHIVISUALNOVEL_API URpyScript : public UKishiDataAsset
{
	GENERATED_BODY()
public:
	URpyScript();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Novel|Script")
	FString scriptXml;
	UPROPERTY(EditAnywhere)
	TMap<FName, FRpyImage> images;
	UPROPERTY(EditAnywhere)
	TMap<FName, FRpyAudio> audios;
	UPROPERTY(EditAnywhere)
	TMap<FName, FRpyTransition> transitions;

	UPROPERTY(EditAnywhere)
	TMap<FName, FRpyCharacter> characters;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual Novel|Script")
	FDynamicObject compileData;

	/** Keys generated from XML declarations and implicit asset references. */
	UPROPERTY()
	TArray<FName> xmlGeneratedImageKeys;
	UPROPERTY()
	TArray<FName> xmlGeneratedAudioKeys;
	UPROPERTY()
	TArray<FName> xmlGeneratedTransitionKeys;
	UPROPERTY()
	TArray<FName> xmlGeneratedCharacterKeys;

	// URpyInstruction *current = nullptr;
	UPROPERTY(Transient)
	URpyInstruction *init = nullptr;
	UPROPERTY(Transient)
	TArray<URpyInstruction *> instructions;
	// TArray<URpyInstruction *> callStack;

	UPROPERTY(Transient)
	TMap<FName, URpyInstruction *> labels;
	UPROPERTY(Transient)
	TMap<FName, URpyInstruction *> instructionsById;

	UFUNCTION(BlueprintPure)
	TArray<FName> GetLabels() const;
	UFUNCTION(BlueprintPure)
	int GetInstructionsLength() const;

	virtual void PostLoad() override;
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;
	virtual void Serialize(FStructuredArchiveRecord Record) override;
	bool AddDefaultTransition(FName name, FString path);
	bool AddDefaultImage(FName name, FString path);
	FRpyImage *FindOrAddDefaultImage(FName name, FString attributes);
	bool AddDefaultAudio(FName name, FString path);

	bool ImportXml(FString text);
	bool Parse();
	bool Compile();
	void LoadRpyData();

	static FRpyInstructionSerialization SerializeInstruction(URpyInstruction *instruction);
	static URpyInstruction *DeserializeInstruction(const FRpyInstructionSerialization &data);

#if WITH_EDITORONLY_DATA
	bool IsAssetUnderProjectContent();
	bool IsAssetUnderPluginContent();
#endif

private:
};
