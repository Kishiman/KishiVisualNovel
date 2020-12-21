// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "KishiDataAsset.h"
#include "DataAssets/PyFile.h"
#include "KishiVisualNovelStatement.h"

#include "KishiVisualNovelScript.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EVisualNovelScriptNodeType : uint8
{
	String UMETA(DisplayName="String"),
	Name ,
	AliasStatement,
	Variable,
	Label,
	DialogueLine,
	ShowStatement,
	SceneStatement,
	BackgroundStatement,
};

UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API UVisualNovelScriptNode : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	TArray<UVisualNovelScriptNode*> Children;
	UPROPERTY(BlueprintReadOnly)
	UVisualNovelScriptNode* Parent=NULL;
	UPROPERTY(BlueprintReadOnly)
	int Index=0;
	UPROPERTY(BlueprintReadOnly)
	FPyLine ScriptLine;
	UPROPERTY(BlueprintReadWrite)
	EVisualNovelScriptNodeType NodeType;
	UVisualNovelScriptNode(){};
	UVisualNovelScriptNode(UVisualNovelScriptNode* parent):Parent(parent){};

	UFUNCTION(BlueprintPure)
	bool IsLeaf()const {return Children.Num()==0;}
	UFUNCTION(BlueprintPure)
	bool IsRoot()const {return Parent==NULL;}
	UFUNCTION(BlueprintPure)
	FString ToString() const;
	UFUNCTION(BlueprintPure)
	TArray<int> GetPath() const;
	UFUNCTION(BlueprintPure)
	UVisualNovelScriptNode* Next() const;
	UFUNCTION(BlueprintCallable) 
	void AddChild(UVisualNovelScriptNode* child);
	
protected:
private:
};
class UVisualNovelScript;
UCLASS(Blueprintable)
class KISHIVISUALNOVEL_API UVisualNovelScriptCompiler : public UObject
{
	GENERATED_BODY()
public:
	UVisualNovelScriptCompiler();
	UPROPERTY(BlueprintReadWrite)
	FString Regex;
	UPROPERTY(BlueprintReadWrite)
	EVisualNovelScriptNodeType TargetNodeType;
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	bool MatchScriptNode(UVisualNovelScript* script,UVisualNovelScriptNode* node)const;
	virtual bool MatchScriptNode_Implementation(UVisualNovelScript* script,UVisualNovelScriptNode* node)const;
	UFUNCTION(BlueprintImplementableEvent)
	void Handle(UVisualNovelScript* script,UVisualNovelScriptNode* node);
	
protected:
private:
};


UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API UVisualNovelScript : public UKishiDataAsset
{
	GENERATED_BODY()
	public:
	UPROPERTY(EditAnywhere)
	UVisualNovelScriptNode* ScriptTree;
	TArray<UVisualNovelStatement*> statements;
	TMap<FName,UVisualNovelStatement*> Labels;
	void ScriptTreeFromPyFines(TArray<FPyLine>& PyLines);
};
