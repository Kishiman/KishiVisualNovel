#pragma once

#include "CoreMinimal.h"
#include "Libraries/Tree/BaseTree.h"

#include "Components/PanelWidget.h"

#include "WidgetProxyBaseTree.generated.h"

UCLASS(BlueprintType)
class KISHI_API UWidgetProxyBaseTree : public UObject, public IBaseTree
{
    GENERATED_BODY()
public:
    UWidgetProxyBaseTree(){};
    UWidgetProxyBaseTree(UWidget *widget);

    UFUNCTION(BlueprintCallable)
    static UWidgetProxyBaseTree *FromWidget(UWidget *widget);
    UFUNCTION(BlueprintPure)
    UWidget *ToWidget() const;

    virtual TScriptInterface<IBaseTree> GetParentTree_Implementation() const override;
    virtual TArray<TScriptInterface<IBaseTree>> GetDirectChildren_Implementation() const override;
    virtual uint8 GetDirectChildrenSize_Implementation() const override;

protected:
    UWidget *widget = NULL;
    UPanelWidget *panelWidget = NULL;
    static TMap<UWidget *, UWidgetProxyBaseTree *> pointerMap;
};
