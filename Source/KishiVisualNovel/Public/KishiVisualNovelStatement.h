#pragma once

#include "CoreMinimal.h"
#include "Engine/Blueprint.h" 

#include "KishiVisualNovelStatement.generated.h"




class AKishiVisualNovelGameMode;
class UVisualNovelScript;


UENUM(BlueprintType)
enum class ERuntimeStatementState : uint8
{
    OnHold UMETA(DisplayName="On Hold"),
    Running UMETA(DisplayName="Running"),
    Finished UMETA(DisplayName="Finished"),
};
class UVisualNovelStatement;
UCLASS(Blueprintable)
class KISHIVISUALNOVEL_API UVisualNovelTransition : public UObject
{
    GENERATED_BODY()
public:
    UVisualNovelTransition(){};
    UVisualNovelTransition(UVisualNovelStatement* parent):Parent(parent){};
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script")
    UVisualNovelStatement* Parent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scene")
    FName Scene;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scene")
    FName SceneEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sprite")
    FName Image;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sprite")
    FName ImageEffect;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sprite")
    bool bHideImage=false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
    FName Music;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
    FName SoundEffect;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
    FName Voice;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Control")
    FName JumpLabel;
    // UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	// void OnEnter();
	//virtual void OnEnter_Implementation();

protected:
private:

};

UCLASS(Blueprintable)
class KISHIVISUALNOVEL_API UVisualNovelStatement: public UObject
{
    GENERATED_BODY()
public:
    UVisualNovelStatement(){};
    // UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	// void OnEnter();
	// //virtual void OnEnter_Implementation();

    // UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	// void OnExit();
	// //virtual void OnExit_Implementation();

    // UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	// bool Verify()const;
	//virtual bool Verify_Implementation()const;
    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	FText GetSpeakerDisplayName()const;
	virtual FText GetSpeakerDisplayName_Implementation()const {return DisplayName;};

    UPROPERTY(EditAnywhere)
    AKishiVisualNovelGameMode* GameMode;
    UPROPERTY(EditAnywhere)
    ERuntimeStatementState state;

    
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Context")
    // FName CharacterAlias;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
    FText DisplayName=FText::FromString("???");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
    FText DialogueLine;

    UPROPERTY(BlueprintReadOnly, Category="ScriptData")
    int index;
    
// //Dynamic Runtime Flags
//     UPROPERTY(EditAnywhere)
//     TArray<FName> BoolFlags;
//     UPROPERTY(EditAnywhere)
//     TArray<FName> NameFlags;
//     UPROPERTY(EditAnywhere)
//     TArray<FName> IntFlags;
//     UPROPERTY(EditAnywhere)
//     TArray<FName> FloatFlags;
protected:
private:
};

