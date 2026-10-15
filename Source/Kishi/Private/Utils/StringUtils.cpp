#include "Utils/StringUtils.h"
#include <regex>

/**
 * @param Pred: item - target
 */
template <typename T, typename Predicate>
int32 BinarySearchLeft(const TArray<T> &Array, Predicate Pred)
{
  int32 Left = 0, Right = Array.Num() - 1;
  int32 BestIndex = -1; // Default to not found

  while (Left <= Right)
  {
    int32 Mid = (Left + Right) / 2;
    int32 Comparison = Pred(Array[Mid]); // Get difference

    if (Comparison == 0) // Exact match
    {
      return Mid;
    }
    else if (Comparison < 0) // Target is greater, move right
    {
      BestIndex = Mid;
      Left = Mid + 1;
    }
    else // Target is smaller, move left
    {
      Right = Mid - 1;
    }
  }

  return BestIndex;
}

template <typename T, typename Predicate>
int32 BinarySearchRight(const TArray<T> &Array, Predicate Pred)
{
  int32 Left = 0, Right = Array.Num() - 1;
  int32 BestIndex = -1; // Default to not found

  while (Left <= Right)
  {
    int32 Mid = (Left + Right) / 2;
    int32 Comparison = Pred(Array[Mid]); // Get difference

    if (Comparison == 0) // Exact match
    {
      return Mid;
    }
    else if (Comparison < 0) // Target is greater, move right
    {
      Left = Mid + 1;
    }
    else // Target is smaller, move left
    {
      BestIndex = Mid;
      Right = Mid - 1;
    }
  }

  return BestIndex;
}

bool UStringUtils::StringToTaggedString(const FString &SourceString, FTaggedString &Target)
{
	Target.Tags.Empty();
	Target.visemes.Empty();
	Target.PlainString.Empty();
	Target.SourceString.Empty();
	Target.Length = 0;
	TArray<int32> OpenTags;
	for (int32 Position = 0; Position < SourceString.Len();)
  {
		if (SourceString[Position] != TEXT('<'))
		{
			Target.PlainString.AppendChar(SourceString[Position++]);
			continue;
		}

		const int32 Close = SourceString.Find(TEXT(">"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Position + 1);
		if (Close == INDEX_NONE)
		{
			Target.PlainString.AppendChar(SourceString[Position++]);
			continue;
		}
		const bool bShorthandClose = SourceString.Mid(Position, 3) == TEXT("</>");
		const bool bClosing = bShorthandClose || (Position + 1 < SourceString.Len() && SourceString[Position + 1] == TEXT('/'));
		const int32 NameStart = Position + (bClosing ? 2 : 1);
		const FString TagName = bShorthandClose && OpenTags.Num() > 0 ? Target.Tags[OpenTags.Last()].tagName : SourceString.Mid(NameStart, Close - NameStart);
		if ((!bShorthandClose && TagName.IsEmpty()) || TagName.Contains(TEXT(" ")) || TagName.Contains(TEXT("/")) || TagName.Contains(TEXT("<")))
		{
			Target.PlainString.AppendChar(SourceString[Position++]);
			continue;
		}

		if (bClosing)
		{
			if (OpenTags.Num() == 0 || Target.Tags[OpenTags.Last()].tagName != TagName) return false;
			Tag& OpenTag = Target.Tags[OpenTags.Pop()];
			OpenTag.end = Close + 1;
			OpenTag.taggedEnd = Target.PlainString.Len();
		}
		else
		{
			Tag NewTag{TagName, Position, 0, Target.PlainString.Len(), 0};
			OpenTags.Add(Target.Tags.Add(NewTag));
		}
		Position = Close + 1;
  }
	if (OpenTags.Num() > 0) return false;
	Target.SourceString = FlattenTaggedString(SourceString);

	std::string InputStr = TCHAR_TO_UTF8(*Target.PlainString);
  std::regex RegexPatterns[] = {
      std::regex("([aA][hH]|[iI][yY])"),                // A, I
      std::regex("([eE])"),                             // E
      std::regex("([oO])"),                             // O
      std::regex("([uU]|[wW][oO])"),                    // U
      std::regex("([fF]|[vV])"),                        // F, V
      std::regex("([lL]|[dD]|[tT]|[nN])"),              // L, D, T, N
      std::regex("([mM]|[bB]|[pP])"),                   // M, B, P
      std::regex("([wW]|[qQ]|[rR])"),                   // W, Q, R
      std::regex("([sS]|[zZ]|[cC][hH]|[sS][hH]|[jJ])"), // S, Z, CH, SH, J
      std::regex("([tT][hH])")                          // TH
  };

  EMouthViseme VisemeTypes[] = {
      EMouthViseme::AI,
      EMouthViseme::E,
      EMouthViseme::O,
      EMouthViseme::U,
      EMouthViseme::FV,
      EMouthViseme::LDTN,
      EMouthViseme::MBP,
      EMouthViseme::WQR,
      EMouthViseme::SZCHSHJ,
      EMouthViseme::TH};

  for (int i = 0; i < 10; i++)
  {
    std::sregex_iterator begin(InputStr.begin(), InputStr.end(), RegexPatterns[i]);
    std::sregex_iterator end;

    for (auto it = begin; it != end; ++it)
    {
      FMouthViseme Viseme;
      Viseme.type = VisemeTypes[i];
			Viseme.index = it->position();
			Viseme.taggedIndex = Viseme.index;

      Target.visemes.Add(Viseme);
    }
  }

  Target.visemes.Sort([](const FMouthViseme &A, const FMouthViseme &B)
                      { return A.index < B.index; });

	Target.Length = Target.PlainString.Len();
  return true;
}

FString UStringUtils::GetTaggedString(const FTaggedString &Target)
{
  return Target.SourceString;
}
TArray<FName> UStringUtils::SplitPathtoNames(const FString &Target)
{
  TArray<FName> names;
  TArray<FString> strings;
  static const TCHAR *delimiters[] =
      {
          TEXT("/"),
      };
  Target.ParseIntoArray(strings, delimiters, 1, true);
  for (auto &string : strings)
  {
    names.Add(FName(string));
  }
  return names;
}

void UStringUtils::HeadAndTails(const TArray<FName> &Target, FName &Head, TArray<FName> &Tails)
{
  if (Target.Num() == 0)
  {
    Head = FName();
    Tails = {};
  }
  Head = Target[0];
  Tails.Empty(Target.Num() - 1);
  Tails.Append(Target.GetData() + 1, Target.Num() - 1);
  return;
}

int32 UStringUtils::Length(const FTaggedString &Target)
{
  return Target.Length;
}

FString UStringUtils::SubString(const FTaggedString &Target, int32 StartIndex, int32 Length = -1)
{
	const int32 Start = FMath::Clamp(StartIndex, 0, Target.Length);
	const int32 End = Length < 0 ? Target.Length : FMath::Clamp(Start + Length, Start, Target.Length);
	if (Start >= End) return FString();

	FString Result;
	FString CurrentStyle;
	for (int32 CharIndex = Start; CharIndex < End; ++CharIndex)
	{
		FString NewStyle;
		for (const Tag& Current : Target.Tags)
		{
			if (Current.taggedStart <= CharIndex && Current.taggedEnd > CharIndex)
			{
				NewStyle = Current.tagName;
			}
		}
		if (NewStyle != CurrentStyle)
		{
			if (!CurrentStyle.IsEmpty()) Result += TEXT("</>");
			if (!NewStyle.IsEmpty()) Result += FString::Printf(TEXT("<%s>"), *NewStyle);
			CurrentStyle = NewStyle;
		}
		Result.AppendChar(Target.PlainString[CharIndex]);
	}
	if (!CurrentStyle.IsEmpty()) Result += TEXT("</>");
	return Result;
}

FMouthViseme UStringUtils::getVisemeBetween(const FTaggedString &Target, int32 startIndex, int32 endIndex)
{

  FMouthViseme lastViseme;
  bool found = false;

  int32 FoundIndex = BinarySearchLeft(Target.visemes, [endIndex](const FMouthViseme &Viseme)
                                      {
                                        return Viseme.taggedIndex - (endIndex - 1); // Negative if item is before SearchTarget
                                      });
  // no viseme on the left
  if (FoundIndex < 0)
  {
    return FMouthViseme{};
  }
  auto &viseme = Target.visemes[FoundIndex];

  // viseme on the left in range
  if (viseme.taggedIndex >= startIndex)
  {
    return viseme;
  }
  // viseme on the left out of range
  return FMouthViseme{}; // Return default if not found
}

FString UStringUtils::FlattenTaggedString(const FString &TaggedString)
{
	FString Result;
	TArray<FString> OpenTags;
	for (int32 Position = 0; Position < TaggedString.Len();)
	{
		if (TaggedString[Position] != TEXT('<'))
		{
			Result.AppendChar(TaggedString[Position++]);
			continue;
		}
		const int32 Close = TaggedString.Find(TEXT(">"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Position + 1);
		if (Close == INDEX_NONE) { Result.AppendChar(TaggedString[Position++]); continue; }
		const bool bShorthandClose = TaggedString.Mid(Position, 3) == TEXT("</>");
		const bool bClosing = bShorthandClose || (Position + 1 < TaggedString.Len() && TaggedString[Position + 1] == TEXT('/'));
		const int32 NameStart = Position + (bClosing ? 2 : 1);
		const FString TagName = bShorthandClose && OpenTags.Num() > 0 ? OpenTags.Last() : TaggedString.Mid(NameStart, Close - NameStart);
		if ((!bShorthandClose && TagName.IsEmpty()) || TagName.Contains(TEXT(" ")) || TagName.Contains(TEXT("/")) || TagName.Contains(TEXT("<")))
		{
			Result.AppendChar(TaggedString[Position++]);
			continue;
		}

		if (bClosing)
		{
			if (OpenTags.Num() == 0 || OpenTags.Last() != TagName)
			{
				// Keep malformed markup literal; StringToTaggedString reports it as invalid.
				Result += TaggedString.Mid(Position, Close - Position + 1);
			}
			else
			{
				Result += TEXT("</>");
				OpenTags.Pop();
				if (OpenTags.Num() > 0) Result += FString::Printf(TEXT("<%s>"), *OpenTags.Last());
			}
		}
		else
		{
			if (OpenTags.Num() > 0) Result += TEXT("</>");
			Result += FString::Printf(TEXT("<%s>"), *TagName);
			OpenTags.Add(TagName);
		}
		Position = Close + 1;
	}
	while (OpenTags.Num() > 0)
	{
		Result += TEXT("</>");
		OpenTags.Pop();
	}
	return Result;
}
