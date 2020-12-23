#pragma once

#include "CoreMinimal.h"

template <typename Base, typename TargetType = Base>
struct TKishiLinker : public Base
{
public:
	TKishiLinker(){};
	~TKishiLinker(){};

	TKishiLinker(TYPE_OF_NULLPTR){};

	TKishiLinker(TargetType *SourceObject) : Target(SourceObject){};

	TKishiLinker(const TKishiLinker &Other)
	{
		Target = (Other) ? Other.Target : NULL;
	};

	FORCEINLINE void operator=(TYPE_OF_NULLPTR)
	{
		*this = TKishiLinker();
	};
	virtual operator bool() const
	{
		return Target != NULL;
	};

	virtual bool operator==(const TargetType *OtherTarget) const
	{
		return Target == OtherTarget;
	};
	FORCEINLINE bool operator!=(const TargetType *OtherTarget) const
	{
		return !operator==(OtherTarget);
	};
	
	virtual bool operator==(const TKishiLinker &OtherLinker) const
	{
		return Target == OtherLinker.Target;
	};
	FORCEINLINE bool operator!=(const TKishiLinker &OtherLinker) const
	{
		return !operator==(OtherLinker);
	};

	template <typename T>
	FORCEINLINE explicit operator TargetType *() const
	{
		return (*this) ? Target : NULL;
	};

protected:
	TargetType *Target;
};