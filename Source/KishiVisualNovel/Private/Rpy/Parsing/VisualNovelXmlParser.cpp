#include "Rpy/Parsing/VisualNovelXmlParser.h"

namespace
{
	FString EscapeRichTextLiteral(const FString& Text)
	{
		FString Result;
		Result.Reserve(Text.Len());
		for (TCHAR Character : Text)
		{
			switch (Character)
			{
			case TEXT('&'): Result += TEXT("&amp;"); break;
			case TEXT('<'): Result += TEXT("&lt;"); break;
			case TEXT('>'): Result += TEXT("&gt;"); break;
			case TEXT('"'): Result += TEXT("&quot;"); break;
			default: Result.AppendChar(Character); break;
			}
		}
		return Result;
	}
}

FString FVisualNovelXmlNode::GetAttribute(const TCHAR* Name) const
{
	for (const FVisualNovelXmlAttribute& Attribute : Attributes)
		if (Attribute.Name == Name) return Attribute.Value;
	return FString();
}

bool FVisualNovelXmlNode::HasAttribute(const TCHAR* Name) const
{
	for (const FVisualNovelXmlAttribute& Attribute : Attributes)
		if (Attribute.Name == Name) return true;
	return false;
}

FString FVisualNovelXmlNode::GetTextContent() const
{
	FString Result;
	for (const FVisualNovelXmlPart& Part : OrderedContent)
		if (!Part.Child) Result += Part.Text;
	return Result;
}

FString FVisualNovelXmlNode::GetRichTextContent() const
{
	FString Result;
	for (const FVisualNovelXmlPart& Part : OrderedContent)
	{
		if (!Part.Child)
		{
			Result += EscapeRichTextLiteral(Part.Text);
			continue;
		}
		Result += FString::Printf(TEXT("<%s>"), *Part.Child->Tag);
		Result += Part.Child->GetRichTextContent();
		Result += FString::Printf(TEXT("</%s>"), *Part.Child->Tag);
	}
	return Result;
}

bool FVisualNovelXmlParser::Parse(const FString& InSource, TUniquePtr<FVisualNovelXmlNode>& OutRoot, FString& OutError)
{
	Source = &InSource;
	Position = InSource.Len() > 0 && InSource[0] == 0xFEFF ? 1 : 0;
	Error.Empty();
	OutRoot.Reset();
	SkipMisc();
	if (!ParseElement(OutRoot))
	{
		OutError = Error;
		return false;
	}
	SkipMisc();
	if (Position != Source->Len())
	{
		Fail(TEXT("Unexpected content after the document root."));
		OutError = Error;
		return false;
	}
	return true;
}

void FVisualNovelXmlParser::SkipWhitespace()
{
	while (Position < Source->Len() && FChar::IsWhitespace((*Source)[Position])) ++Position;
}

void FVisualNovelXmlParser::SkipMisc()
{
	while (Position < Source->Len())
	{
		SkipWhitespace();
		if (Source->Mid(Position, 2) == TEXT("<?"))
		{
			const int32 End = Source->Find(TEXT("?>"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Position + 2);
			if (End == INDEX_NONE) { Fail(TEXT("Unterminated XML processing instruction.")); return; }
			Position = End + 2;
		}
		else if (Source->Mid(Position, 4) == TEXT("<!--"))
		{
			const int32 End = Source->Find(TEXT("-->"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Position + 4);
			if (End == INDEX_NONE) { Fail(TEXT("Unterminated XML comment.")); return; }
			Position = End + 3;
		}
		else break;
	}
}

bool FVisualNovelXmlParser::ParseElement(TUniquePtr<FVisualNovelXmlNode>& OutNode)
{
	if (Position >= Source->Len() || (*Source)[Position] != TEXT('<') || Source->Mid(Position, 2) == TEXT("</")) return Fail(TEXT("Expected an XML element."));
	++Position;
	OutNode = MakeUnique<FVisualNovelXmlNode>();
	if (!ParseName(OutNode->Tag)) return false;

	while (Position < Source->Len())
	{
		SkipWhitespace();
		if (Position >= Source->Len()) return Fail(TEXT("Unterminated start tag."));
		if (Source->Mid(Position, 2) == TEXT("/>")) { Position += 2; return true; }
		if ((*Source)[Position] == TEXT('>')) { ++Position; break; }

		FVisualNovelXmlAttribute Attribute;
		if (!ParseName(Attribute.Name)) return false;
		SkipWhitespace();
		if (Position >= Source->Len() || (*Source)[Position] != TEXT('=')) return Fail(TEXT("Expected '=' after an XML attribute name."));
		++Position;
		SkipWhitespace();
		if (!ParseAttributeValue(Attribute.Value)) return false;
		if (OutNode->HasAttribute(*Attribute.Name)) return Fail(FString::Printf(TEXT("Duplicate XML attribute '%s'."), *Attribute.Name));
		OutNode->Attributes.Add(MoveTemp(Attribute));
	}

	while (Position < Source->Len())
	{
		if (Source->Mid(Position, 2) == TEXT("</"))
		{
			Position += 2;
			FString ClosingName;
			if (!ParseName(ClosingName)) return false;
			SkipWhitespace();
			if (Position >= Source->Len() || (*Source)[Position] != TEXT('>')) return Fail(TEXT("Expected '>' after a closing tag."));
			++Position;
			if (ClosingName != OutNode->Tag) return Fail(FString::Printf(TEXT("Closing tag '%s' does not match '%s'."), *ClosingName, *OutNode->Tag));
			return true;
		}
		if (Source->Mid(Position, 4) == TEXT("<!--"))
		{
			const int32 End = Source->Find(TEXT("-->"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Position + 4);
			if (End == INDEX_NONE) return Fail(TEXT("Unterminated XML comment."));
			Position = End + 3;
			continue;
		}
		if (Source->Mid(Position, 9) == TEXT("<![CDATA["))
		{
			const int32 End = Source->Find(TEXT("]]>"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Position + 9);
			if (End == INDEX_NONE) return Fail(TEXT("Unterminated CDATA section."));
			FVisualNovelXmlPart& Part = OutNode->OrderedContent.AddDefaulted_GetRef();
			Part.Text = Source->Mid(Position + 9, End - Position - 9);
			Position = End + 3;
			continue;
		}
		if ((*Source)[Position] == TEXT('<'))
		{
			TUniquePtr<FVisualNovelXmlNode> Child;
			if (!ParseElement(Child)) return false;
			FVisualNovelXmlNode* ChildPtr = Child.Get();
			OutNode->Children.Add(MoveTemp(Child));
			FVisualNovelXmlPart& Part = OutNode->OrderedContent.AddDefaulted_GetRef();
			Part.Child = ChildPtr;
			continue;
		}
		FString Text;
		if (!ParseText(Text)) return false;
		FVisualNovelXmlPart& Part = OutNode->OrderedContent.AddDefaulted_GetRef();
		Part.Text = MoveTemp(Text);
	}
	return Fail(FString::Printf(TEXT("Element '%s' is missing its closing tag."), *OutNode->Tag));
}

bool FVisualNovelXmlParser::ParseName(FString& OutName)
{
	const int32 Start = Position;
	if (Position >= Source->Len()) return Fail(TEXT("Expected an XML name."));
	const TCHAR First = (*Source)[Position];
	if (!(FChar::IsAlpha(First) || First == TEXT('_') || First == TEXT(':'))) return Fail(TEXT("Invalid first character in XML name."));
	++Position;
	while (Position < Source->Len())
	{
		const TCHAR C = (*Source)[Position];
		if (!(FChar::IsAlnum(C) || C == TEXT('_') || C == TEXT(':') || C == TEXT('-') || C == TEXT('.'))) break;
		++Position;
	}
	OutName = Source->Mid(Start, Position - Start);
	return true;
}

bool FVisualNovelXmlParser::ParseAttributeValue(FString& OutValue)
{
	if (Position >= Source->Len() || ((*Source)[Position] != TEXT('\"') && (*Source)[Position] != TEXT('\''))) return Fail(TEXT("XML attribute values must be quoted."));
	const TCHAR Quote = (*Source)[Position++];
	const int32 Start = Position;
	while (Position < Source->Len() && (*Source)[Position] != Quote) ++Position;
	if (Position >= Source->Len()) return Fail(TEXT("Unterminated XML attribute value."));
	const FString Encoded = Source->Mid(Start, Position - Start);
	++Position;
	return DecodeEntities(Encoded, OutValue);
}

bool FVisualNovelXmlParser::ParseText(FString& OutText)
{
	const int32 Start = Position;
	while (Position < Source->Len() && (*Source)[Position] != TEXT('<')) ++Position;
	return DecodeEntities(Source->Mid(Start, Position - Start), OutText);
}

bool FVisualNovelXmlParser::DecodeEntities(const FString& Encoded, FString& Decoded)
{
	Decoded.Empty();
	for (int32 Index = 0; Index < Encoded.Len(); ++Index)
	{
		if (Encoded[Index] != TEXT('&')) { Decoded.AppendChar(Encoded[Index]); continue; }
		const int32 Semicolon = Encoded.Find(TEXT(";"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Index + 1);
		if (Semicolon == INDEX_NONE) return Fail(TEXT("Unterminated XML entity reference."));
		const FString Entity = Encoded.Mid(Index + 1, Semicolon - Index - 1);
		if (Entity == TEXT("lt")) Decoded.AppendChar(TEXT('<'));
		else if (Entity == TEXT("gt")) Decoded.AppendChar(TEXT('>'));
		else if (Entity == TEXT("amp")) Decoded.AppendChar(TEXT('&'));
		else if (Entity == TEXT("quot")) Decoded.AppendChar(TEXT('\"'));
		else if (Entity == TEXT("apos")) Decoded.AppendChar(TEXT('\''));
		else if (Entity.StartsWith(TEXT("#")))
		{
			const bool bHex = Entity.Len() > 2 && (Entity[1] == TEXT('x') || Entity[1] == TEXT('X'));
			const int32 DigitStart = bHex ? 2 : 1;
			uint32 CodePoint = 0;
			if (DigitStart >= Entity.Len()) return Fail(TEXT("Empty numeric XML entity."));
			for (int32 DigitIndex = DigitStart; DigitIndex < Entity.Len(); ++DigitIndex)
			{
				const TCHAR Digit = Entity[DigitIndex];
				uint32 Value;
				if (Digit >= TEXT('0') && Digit <= TEXT('9')) Value = Digit - TEXT('0');
				else if (bHex && Digit >= TEXT('a') && Digit <= TEXT('f')) Value = 10 + Digit - TEXT('a');
				else if (bHex && Digit >= TEXT('A') && Digit <= TEXT('F')) Value = 10 + Digit - TEXT('A');
				else return Fail(TEXT("Invalid numeric XML entity."));
				const uint32 Base = bHex ? 16 : 10;
				if (CodePoint > (0x10FFFFu - Value) / Base) return Fail(TEXT("XML entity is outside the Unicode range."));
				CodePoint = CodePoint * Base + Value;
			}
			if (CodePoint <= 0xFFFFu) Decoded.AppendChar(static_cast<TCHAR>(CodePoint));
			else
			{
				CodePoint -= 0x10000u;
				Decoded.AppendChar(static_cast<TCHAR>(0xD800u + (CodePoint >> 10)));
				Decoded.AppendChar(static_cast<TCHAR>(0xDC00u + (CodePoint & 0x3FFu)));
			}
		}
		else return Fail(FString::Printf(TEXT("Unknown XML entity '&%s;'."), *Entity));
		Index = Semicolon;
	}
	return true;
}

bool FVisualNovelXmlParser::Fail(const FString& Message)
{
	if (Error.IsEmpty()) Error = FString::Printf(TEXT("%s (character %d)"), *Message, Position + 1);
	return false;
}
