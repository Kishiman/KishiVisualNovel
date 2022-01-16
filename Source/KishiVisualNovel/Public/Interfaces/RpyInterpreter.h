#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interfaces/RpyInstruction.h"
#include "Interfaces/KVNCharacter.h"

#include "RpyInterpreter.generated.h"


UINTERFACE(BlueprintType) class KISHIVISUALNOVEL_API URpyInterpreter : public UInterface
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyInterpreter
{
    GENERATED_BODY()
public:
};

UCLASS()
class KISHIVISUALNOVEL_API URpyInterpreterLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
protected:
private:
};
