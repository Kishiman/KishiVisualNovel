#pragma once
#include "Theories/Tree/WidgetProxyBaseTree.h"

TMap<UWidget *, UWidgetProxyBaseTree *> UWidgetProxyBaseTree::pointerMap = TMap<UWidget *, UWidgetProxyBaseTree *>();

UWidgetProxyBaseTree::UWidgetProxyBaseTree(UWidget *widget) : widget(widget), panelWidget(Cast<UPanelWidget>(widget)) {}
UWidgetProxyBaseTree *UWidgetProxyBaseTree::FromWidget(UWidget *widget)
{
    auto result = UWidgetProxyBaseTree::pointerMap.Find(widget);
    if (result)
        return *result;
    auto newObject = NewObject<UWidgetProxyBaseTree>();
    newObject->widget = widget;
    newObject->panelWidget = Cast<UPanelWidget>(widget);
    UWidgetProxyBaseTree::pointerMap.Add(widget, newObject);
    return newObject;
}

UWidget *UWidgetProxyBaseTree::ToWidget() const
{
    return this->widget;
}

TScriptInterface<IBaseTree> UWidgetProxyBaseTree::GetParentTree_Implementation() const
{
    UWidget *parent = this->widget->GetParent();
    UWidgetProxyBaseTree *proxy = FromWidget(parent);
    return proxy;
}

TArray<TScriptInterface<IBaseTree>> UWidgetProxyBaseTree::GetDirectChildren_Implementation() const
{
    if (!this->panelWidget)
    {
        return TArray<TScriptInterface<IBaseTree>>();
    }
    TArray<UWidget *> children = this->panelWidget->GetAllChildren();
    TArray<TScriptInterface<IBaseTree>> proxies;
    for (size_t i = 0; i < children.Num(); i++)
    {
        proxies.Add(FromWidget(children[0]));
    }
    return proxies;
}
uint8 UWidgetProxyBaseTree::GetDirectChildrenSize_Implementation() const
{
    if (!this->panelWidget)
    {
        return 0;
    }
    return this->panelWidget->GetChildrenCount();
}
