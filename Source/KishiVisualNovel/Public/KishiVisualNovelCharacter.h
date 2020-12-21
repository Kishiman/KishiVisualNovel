#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "Sound/SoundWave.h"
#include "KishiDataAsset.h"

#include "KishiVisualNovelCharacter.generated.h"

UCLASS()
class KISHIVISUALNOVEL_API UVisualNovelCharacter : public UKishiDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,Category="ID")
    TArray<FName> Aliases;
    UPROPERTY(EditAnywhere,Category="Profile")
    FName FirstName;
    UPROPERTY(EditAnywhere,Category="Profile")
    FName LastName;
    UPROPERTY(EditAnywhere,Category="Profile")
    int Age;
    UPROPERTY(EditAnywhere,Category="Profile")
    ETextGender Gender;
    UPROPERTY(EditAnywhere,Category="Assets")
    TMap<FName,UPaperSprite*> Sprites;
    UPROPERTY(EditAnywhere,Category="Assets")
    TMap<FName,USoundWave*> Sounds;

protected:
private:
};