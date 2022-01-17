#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PaperSprite.h"

#include "Interfaces/RpyInterpreter.h"

#include "KVNGameMode.generated.h"

UCLASS(Blueprintable)
class KISHIVISUALNOVEL_API AKVNGameMode : public AGameModeBase//, public IRpyInterpreter
{
    GENERATED_BODY()
public:
    AKVNGameMode(){};

    // Static CompileTime Names
    UPROPERTY(EditAnywhere)
    TMap<FName, TScriptInterface<IRpyInstruction>> Labels;
    UPROPERTY(EditAnywhere)
    TMap<FName, TScriptInterface<IKVNCharacter>> Characters;
    UPROPERTY(EditAnywhere)
    TSet<FName> Effects;
    UPROPERTY(EditAnywhere)
    TSet<FName> Scenes;
    UPROPERTY(EditAnywhere)
    TMap<FName, UPaperSprite *> Images;
    UPROPERTY(EditAnywhere)
    TMap<FName, USoundWave *> Musics;
    UPROPERTY(EditAnywhere)
    TMap<FName, USoundWave *> Sounds;
    // Dynamic Runtime Flags
    UPROPERTY(EditAnywhere)
    TMap<FName, bool> Bools;
    UPROPERTY(EditAnywhere)
    TMap<FName, FName> Names;
    UPROPERTY(EditAnywhere)
    TMap<FName, int> Ints;
    UPROPERTY(EditAnywhere)
    TMap<FName, float> Floats;

protected:
private:
};
#define VNGAMEMODE (AKVNGameMode *)GetWorld()->GetAuthGameMode()