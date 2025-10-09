#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Interfaces/Widgets/SaveSlotWidget.h"
#include "Rpy/BaseRpyGameInstance.h"

#include "SaveMenuWidget.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API USaveMenuWidget : public UInterface
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API ISaveMenuWidget
{
  GENERATED_BODY()
public:
};

UCLASS()
class KISHIVISUALNOVEL_API USaveMenuWidgetLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
protected:
private:
};