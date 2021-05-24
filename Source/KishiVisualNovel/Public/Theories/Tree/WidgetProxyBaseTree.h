#pragma once

#include "CoreMinimal.h"
#include "Libraries/Tree/BaseTree.h"

#include "Components/PanelWidget.h"

#include "WidgetProxyBaseTree.generated.h"

UCLASS(Blueprintable)
class KISHIVISUALNOVEL_API UWidgetProxyBaseTree : public UObject, public IBaseTree
{
    GENERATED_BODY()
public:
    UWidgetProxyBaseTree(){};
    UWidgetProxyBaseTree(UWidget *widget) : widget(widget), panelWidget(Cast<UPanelWidget>(widget)) {}
    virtual TScriptInterface<IBaseTree> GetParentTree_Implementation() const override
    {
        UWidget *parent = this->widget->GetParent();
        UWidgetProxyBaseTree *proxy = NewObject<UWidgetProxyBaseTree>(parent);
        return proxy;
    }

    virtual TArray<TScriptInterface<IBaseTree>> GetDirectChildren_Implementation() const override
    {
        if (!this->panelWidget)
        {
            return TArray<TScriptInterface<IBaseTree>>();
        }
        TArray<UWidget *> children = this->panelWidget->GetAllChildren();
        TArray<TScriptInterface<IBaseTree>> proxies;
        for (size_t i = 0; i < children.Num(); i++)
        {
            proxies.Add(NewObject<UWidgetProxyBaseTree>(children[0]));
        }
        return proxies;
    }
    virtual uint8 GetDirectChildrenSize_Implementation() const override
    {
        if (!this->panelWidget)
        {
            return 0;
        }
        return this->panelWidget->GetChildrenCount()
    }

protected:
    UWidget *widget = NULL;
    UPanelWidget *panelWidget = NULL;
};
