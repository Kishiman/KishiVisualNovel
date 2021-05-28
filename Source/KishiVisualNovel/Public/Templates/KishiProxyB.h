#pragma once

#include "CoreMinimal.h"

template <
    typename Target,
    typename Derived,
    typename Interface,
    >
// UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API TKishiProxyB : public UObject, public Interface
{
    // GENERATED_BODY()
public:
    TKishiProxy(){};
    TKishiProxy(Target *target) : target(target), derived(Cast<Derived>(target)) {}

    // UFUNCTION(BlueprintCallable)
    static TKishiProxy *FromBase(Target *target)
    {
        auto result = TKishiProxyB::pointerMap.Find(target);
        if (result)
            return *result;
        auto newObject = NewObject<TKishiProxyB>();
        newObject->target = target;
        newObject->derived = Cast<Derived>(target);
        TKishiProxyB::pointerMap.Add(target, newObject);
        return newObject;
    }
    // UFUNCTION(BlueprintPure)
    Target *ToBase() const
    {
    return this->target;
}
    //overrides
protected:
    Target *target = NULL;
    Derived *derived = NULL;
    static TMap<Target *, TKishiProxy *> pointerMap = TMap<Target *, TKishiProxy *>();
};
