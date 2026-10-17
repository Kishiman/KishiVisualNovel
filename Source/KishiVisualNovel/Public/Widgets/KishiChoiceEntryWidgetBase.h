#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Audio/AudioPlayer.h"
#include "Components/RichTextBlock.h"
#include "Interfaces/ScriptInterpreterManagers/RpyAudioManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyChoiceManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpySceneManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyShowManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyStatementManager.h"
#include "Utils/StringUtils.h"
class UButton;
class UTextBlock;

#include "KishiChoiceEntryWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKishiChoiceClicked, int32, Index);

UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API UKishiChoiceEntryWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = "Rpy|Choice")
	FKishiChoiceClicked OnChoiceClicked;
	UPROPERTY(BlueprintReadOnly, Category = "Rpy|Choice")
	int32 ChoiceIndex = INDEX_NONE;
	UPROPERTY(BlueprintReadOnly, Category = "Rpy|Choice")
	FString ChoiceText;
	UFUNCTION(BlueprintCallable, Category = "Rpy|Choice")
	void SetChoice(int32 Index, const FString &Text);

protected:
	virtual void NativeOnInitialized() override;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Choice")
	UButton *ChoiceButton = nullptr;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Choice")
	UTextBlock *ChoiceLabel = nullptr;
	UFUNCTION()
	void HandleClicked();
};
