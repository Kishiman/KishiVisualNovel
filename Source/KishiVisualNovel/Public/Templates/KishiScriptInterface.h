
#pragma once

#include "CoreMinimal.h"
#include "Templates/KishiInterfaceLinker.h"

//Comment for git test
template <class InterfaceType>
struct TKishiScriptInterface : public TScriptInterface<InterfaceType>
{

	TKishiScriptInterface() {}
	TKishiScriptInterface(TYPE_OF_NULLPTR)
		: TScriptInterface<InterfaceType>() {}

	template <class UObjectType>
	TKishiScriptInterface(UObjectType *SourceObject)
		: TScriptInterface<InterfaceType>(SourceObject) {}

	TKishiScriptInterface(const TScriptInterface<InterfaceType> &Other)
		: TScriptInterface<InterfaceType>(Other) {}

	template <class OtherInterfaceType>
	TKishiScriptInterface(const TScriptInterface<OtherInterfaceType> &Other)
		: TScriptInterface<InterfaceType>(Other.GetObject()) {}

	FORCEINLINE bool operator==(const UObject *Other) const
	{
		return this->GetObject() == Other;
	}

	FORCEINLINE bool operator!=(const UObject *Other) const
	{
		return !operator==(Other);
	}

	FORCEINLINE bool operator==(const TScriptInterface<InterfaceType> &Other) const
	{
		return operator==(Other.GetObject());
	}
	FORCEINLINE bool operator!=(const TScriptInterface<InterfaceType> &Other) const
	{
		return operator!=(Other.GetObject());
	}

	template <class OtherInterfaceType>
	FORCEINLINE bool operator==(const TScriptInterface<OtherInterfaceType> &Other) const
	{
		return operator==(Other.GetObject());
	}

	template <class OtherInterfaceType>
	FORCEINLINE bool operator!=(const TScriptInterface<OtherInterfaceType> &Other) const
	{
		return operator!=(Other.GetObject());
	}

	FORCEINLINE operator UObject *() const
	{
		return this->GetObject();
	}
	FORCEINLINE operator TScriptInterface<InterfaceType>() const
	{
		return (*this);
	}
	template <class OtherInterfaceType>
	FORCEINLINE operator TScriptInterface<OtherInterfaceType>() const
	{
		return TScriptInterface<OtherInterfaceType>(this->GetObject());
	}

	FORCEINLINE explicit operator bool() const
	{
		return this->GetInterface() != NULL || this->GetObject() != NULL;
	}
};