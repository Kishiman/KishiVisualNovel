#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h" 

#include "KishiVisualNovelCharacter.h"
#include "KishiVisualNovelStatement.h"

#include "KishiVisualNovelGameMode.generated.h"

UCLASS(Blueprintable)
class KISHIVISUALNOVEL_API AKishiVisualNovelGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AKishiVisualNovelGameMode(){};

//Static CompileTime Names
    UPROPERTY(EditAnywhere)
    TMap<FName,UVisualNovelStatement*> Labels;
    UPROPERTY(EditAnywhere)
    TMap<FName,UVisualNovelCharacter*> Characters;
    UPROPERTY(EditAnywhere)
    TSet<FName> Effects;
    UPROPERTY(EditAnywhere)
    TSet<FName> Scenes;
    UPROPERTY(EditAnywhere)
    TMap<FName,UPaperSprite*> Images;
    UPROPERTY(EditAnywhere)
    TMap<FName,USoundWave*> Musics;
    UPROPERTY(EditAnywhere)
    TMap<FName,USoundWave*> Sounds;
//Dynamic Runtime Flags
    UPROPERTY(EditAnywhere)
    TMap<FName,bool> Bools;
    UPROPERTY(EditAnywhere)
    TMap<FName,FName> Names;
    UPROPERTY(EditAnywhere)
    TMap<FName,int> Ints;
    UPROPERTY(EditAnywhere)
    TMap<FName,float> Floats;

protected:
private:
};
#define VNGAMEMODE (AKishiVisualNovelGameMode*)GetWorld()->GetAuthGameMode() 