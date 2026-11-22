#pragma once

#include "CoreMinimal.h"

struct FVisualNovelXmlAttribute
{
	FString Name;
	FString Value;
};

struct FVisualNovelXmlNode;

struct FVisualNovelXmlPart
{
	FString Text;
	FVisualNovelXmlNode* Child = nullptr;
};

struct FVisualNovelXmlNode
{
	FString Tag;
	TArray<FVisualNovelXmlAttribute> Attributes;
	TArray<TUniquePtr<FVisualNovelXmlNode>> Children;
	TArray<FVisualNovelXmlPart> OrderedContent;

	const FString& GetTag() const { return Tag; }
	FString GetAttribute(const TCHAR* Name) const;
	bool HasAttribute(const TCHAR* Name) const;
	FString GetTextContent() const;
	FString GetRichTextContent() const;
};

class FVisualNovelXmlParser
{
public:
	bool Parse(const FString& Source, TUniquePtr<FVisualNovelXmlNode>& OutRoot, FString& OutError);

private:
	void SkipWhitespace();
	void SkipMisc();
	bool ParseElement(TUniquePtr<FVisualNovelXmlNode>& OutNode);
	bool ParseName(FString& OutName);
	bool ParseAttributeValue(FString& OutValue);
	bool ParseText(FString& OutText);
	bool DecodeEntities(const FString& Encoded, FString& Decoded);
	bool Fail(const FString& Message);

	const FString* Source = nullptr;
	int32 Position = 0;
	FString Error;
};
