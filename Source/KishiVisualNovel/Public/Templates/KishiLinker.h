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

	virtual bool operator==(const TargetType *Other) const
	{
		return Target == Other;
	};
	FORCEINLINE bool operator!=(const TargetType *Other) const
	{
		return !operator==(Other.Target);
	};
	
	virtual bool operator==(const TKishiLinker &Other) const
	{
		return Target == Other.Target;
	};
	FORCEINLINE bool operator!=(const TKishiLinker &Other) const
	{
		return !operator==(Other);
	};

	template <typename T>
	FORCEINLINE explicit operator TargetType *() const
	{
		return (*this) ? Target : NULL;
	};

	// template <typename T>
	// FORCEINLINE explicit operator Base *() const
	// {
	// 	return (*this) ? this : NULL;
	// };

protected:
	TargetType *Target;
};