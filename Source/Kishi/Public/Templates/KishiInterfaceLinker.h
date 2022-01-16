#pragma once

#include "CoreMinimal.h"
#include "KishiLinker.h"

template <class InterfaceType>
class TKishiInterfaceLinker : public TKishiLinker<UObject, UObject>, virtual public InterfaceType
{
	typedef TKishiLinker<UObject> Super;

public:
	TKishiInterfaceLinker(const TKishiInterfaceLinker &Other) : TKishiLinker<UObject, UObject>(Other){};
	template <class OtherInterfaceType>
	TKishiInterfaceLinker(const TKishiInterfaceLinker<OtherInterfaceType> &Other)
	{
		Target = (Other && Cast<InterfaceType>(Other)) ? Other.Target : NULL;
	};
	void operator=(TYPE_OF_NULLPTR)
	{
		Target=NULL;
	};

	void operator=(const TKishiInterfaceLinker &Other)
	{
		Target = (Other) ? Other.Target : NULL;
	};
	template <class OtherInterfaceType>
	void operator=(const TKishiInterfaceLinker<OtherInterfaceType> &Other)
	{
		Target = (Other && Cast<InterfaceType>(Other)) ? Other.Target : NULL;
	};
	template <class OtherInterface>
	FORCEINLINE bool operator==(const TKishiInterfaceLinker<OtherInterface> &Other) const
	{
		auto I = Cast<InterfaceType>(Other);
		return I && operator==(Other.Target);
	};
	template <class OtherInterface>
	FORCEINLINE bool operator!=(const TKishiInterfaceLinker<OtherInterface> &Other) const
	{
		!operator==(Other);
	};
	virtual operator bool() const override
	{
		return (*((Super *)(this))) && Cast<InterfaceType>(Target);
	};
};