#include "Utils/StringUtils.h"
#include <regex>

template <typename T, typename Predicate>
int32 BinarySearch(const TArray<T> &Array, Predicate Pred)
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

bool UStringUtils::StringToTaggedString(const FString &SourceString, FTaggedString &Target)
{
  Target.SourceString = FlattenTaggedString(SourceString);
  int32 StartPos = 0;
  while (StartPos >= 0 && StartPos < Target.SourceString.Len())
  {
    // Find the next opening tag
    int32 OpenPos = Target.SourceString.Find("<", ESearchCase::IgnoreCase, ESearchDir::FromStart, StartPos);
    if (OpenPos < 0)
    {
      break; // No more tags found
    }

    // Find the closing tag
    int32 ClosePos = Target.SourceString.Find(">", ESearchCase::IgnoreCase, ESearchDir::FromStart, OpenPos);
    if (ClosePos < 0)
    {
      break; // Invalid tag, ignore it
    }

    // Extract the tag name and position information
    FString TagName = Target.SourceString.Mid(OpenPos + 1, ClosePos - OpenPos - 1);
    if (Target.SourceString[OpenPos + 1] == '/' && Target.SourceString[OpenPos + 2] == '>')
    {
      auto &lastTag = Target.Tags.Last();
      lastTag.end = ClosePos + 1;
    }
    else
    {
      Target.Tags.Add(Tag{TagName, OpenPos, ClosePos + 1, 0, 0});
    }
    StartPos = ClosePos + 1;
  }
  int32 skippedCharLen = 0;

  for (auto &tag : Target.Tags)
  {
    tag.taggedStart = tag.start - skippedCharLen;
    skippedCharLen += tag.tagName.Len() + 5;
    tag.taggedEnd = tag.end - skippedCharLen;
  }

  std::string InputStr = TCHAR_TO_UTF8(*Target.SourceString);
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

      Target.visemes.Add(Viseme);
    }
  }

  Target.visemes.Sort([](const FMouthViseme &A, const FMouthViseme &B)
                      { return A.index < B.index; });

  TArray<FMouthViseme> FilteredVisemes;
  int32 AdjustedIndex = 0;
  int32 RawIndex = 0;

  // Process each viseme
  for (FMouthViseme &Viseme : Target.visemes)
  {
    // Check if the viseme is inside a tag
    bool bIsInsideTag = false;
    int indexOffset = 0;
    for (const Tag &T : Target.Tags)
    {
      if (Viseme.index < T.start)
      {
        // this viseme is before the newt tag, stop search
        break;
      }
      if (Viseme.index >= T.start && Viseme.index <= T.start + T.tagName.Len())
      {
        // this viseme is inside the tag brackets
        bIsInsideTag = true;
        break;
      }
      // offset by"<TagName>"
      indexOffset += T.tagName.Len() + 2;
      if (Viseme.index <= T.end)
      {
        // this viseme is between the tag
        break;
      }
      // offset </>
      indexOffset += 2;
    }
    if (bIsInsideTag)
    {
      continue; // filter out this viseme
    }

    // Adjust indices to account for skipped tag characters
    Viseme.taggedIndex = Viseme.index - indexOffset;

    FilteredVisemes.Add(Viseme);
  }

  // Store the filtered visemes back in the tagged string
  Target.visemes = FilteredVisemes;

  Target.Length = Target.SourceString.Len() - skippedCharLen;
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
  int32 EndIndex = Length > -1 ? StartIndex + Length : Target.Length;
  int32 StartPos = StartIndex;
  int32 EndPos = -1;
  FString prefix = "";
  FString sufix = "";
  for (auto tag : Target.Tags)
  {
    if (StartIndex < tag.taggedStart)
    {
      StartPos = StartIndex + (tag.start - tag.taggedStart);
      break;
    }
    else if (StartIndex < tag.taggedEnd)
    {
      StartPos = StartIndex + (tag.start - tag.taggedStart);
      StartPos += tag.tagName.Len() + 2;
      prefix = FString::Printf(TEXT("<%s>"), *tag.tagName);
      break;
    }
  }
  for (auto tag : Target.Tags)
  {
    if (EndIndex < tag.taggedStart)
    {
      EndPos = EndIndex + (tag.start - tag.taggedStart);
      break;
    }
    else if (EndIndex < tag.taggedEnd)
    {
      EndPos = EndIndex + (tag.start - tag.taggedStart);
      EndPos += tag.tagName.Len() + 2;
      sufix = FString::Printf(TEXT("</>"), *tag.tagName);
      break;
    }
  }
  if (EndPos < 0)
  {
    EndPos = EndIndex + (Target.SourceString.Len() - Target.Length);
  }

  return prefix + Target.SourceString.Mid(StartPos, EndPos - StartPos) + sufix;
}

FMouthViseme UStringUtils::getVisemeBetween(const FTaggedString &Target, int32 startIndex, int32 endIndex)
{

  FMouthViseme lastViseme;
  bool found = false;

  for (const FMouthViseme &viseme : Target.visemes)
  {
    if (viseme.taggedIndex >= startIndex && viseme.taggedIndex < endIndex)
    {
      lastViseme = viseme;
      found = true;
    }
    else if (viseme.taggedIndex >= endIndex)
      break;
  }

  return found ? lastViseme : FMouthViseme{}; // Return default if not found
}

FString UStringUtils::FlattenTaggedString(const FString &TaggedString)
{
  FString ResultString;
  TArray<FString> OpenTags;

  for (int32 i = 0; i < TaggedString.Len(); i++)
  {
    if (TaggedString[i] == '<')
    {
      // Check for closing tag
      if (i + 2 < TaggedString.Len() && TaggedString[i + 1] == '/' && TaggedString[i + 2] == '>')
      {
        ResultString += FString::Printf(TEXT("</>"));
        OpenTags.Pop();
        if (OpenTags.Num() > 0)
          ResultString += FString::Printf(TEXT("<%s>"), *(OpenTags.Last()));
        i = i + 2;
      }
      // Check for opening tag
      else
      {
        FString OpenTagName;
        int32 OpenTagIndex = TaggedString.Find(">", ESearchCase::IgnoreCase, ESearchDir::FromStart, i + 1);
        if (OpenTagIndex != -1)
        {
          if (OpenTags.Num() > 0)
            ResultString += FString::Printf(TEXT("</>"));
          OpenTagName = TaggedString.Mid(i + 1, OpenTagIndex - i - 1);
          OpenTags.Add(OpenTagName);
          ResultString += FString::Printf(TEXT("<%s>"), *OpenTagName);
          i = OpenTagIndex;
        }
      }
    }
    else
    {
      ResultString.AppendChar(TaggedString[i]);
    }
  }
  // Close any remaining open tags
  for (int32 i = OpenTags.Num() - 1; i >= 0; i--)
  {
    ResultString += FString::Printf(TEXT("</>"), *OpenTags[i]);
  }

  return ResultString;
}