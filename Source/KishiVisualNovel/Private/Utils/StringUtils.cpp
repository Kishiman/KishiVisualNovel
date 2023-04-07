#include "Utils/StringUtils.h"
bool UStringUtils::Set(FTaggedString &Target, FString SourceString) {
  Target.SourceString = SourceString;
  int32 StartPos = 0;
  while (StartPos >= 0 && StartPos < Target.SourceString.Len()) {
    // Find the next opening tag
    int32 OpenPos = Target.SourceString.Find("<", ESearchCase::IgnoreCase,
                                             ESearchDir::FromStart, StartPos);
    if (OpenPos < 0) {
      break; // No more tags found
    }

    // Find the closing tag
    int32 ClosePos = Target.SourceString.Find(">", ESearchCase::IgnoreCase,
                                              ESearchDir::FromStart, OpenPos);
    if (ClosePos < 0) {
      break; // Invalid tag, ignore it
    }

    // Extract the tag name and position information
    FString TagName =
        Target.SourceString.Mid(OpenPos + 1, ClosePos - OpenPos - 1);
    if (TagName[0] == '/') {
      auto &lastTag = Target.Tags[Target.Tags.Num() - 1];
      lastTag.end = ClosePos + 1;
    } else {
      Target.Tags.Add(Tag{TagName, OpenPos, ClosePos + 1, 0, 0});
    }
    StartPos = ClosePos + 1;
  }
  int32 skippedCharLen = 0;

  for (auto &tag : Target.Tags) {
    tag.taggedStart = tag.start - skippedCharLen;
    skippedCharLen += 2 * tag.tagName.Len() + 5;
    tag.taggedEnd = tag.end - skippedCharLen;
  }

  Target.Length = Target.SourceString.Len() - skippedCharLen;
  return true;
}

FString UStringUtils::Get(const FTaggedString &Target) {
  return Target.SourceString;
}

int32 UStringUtils::Length(const FTaggedString &Target) {
  return Target.Length;
}

FString UStringUtils::SubString(const FTaggedString &Target, int32 StartIndex,
                                int32 Length = -1) {
  int32 EndIndex = Length > -1 ? StartIndex + Length : Target.Length;
  int32 StartPos = StartIndex;
  int32 EndPos = -1;
  FString prefix = "";
  FString sufix = "";
  for (auto tag : Target.Tags) {
    if (StartIndex < tag.taggedStart) {
      StartPos = StartIndex + (tag.start - tag.taggedStart);
      break;
    } else if (StartIndex < tag.taggedEnd) {
      StartPos = StartIndex + (tag.start - tag.taggedStart);
      StartPos += tag.tagName.Len() + 2;
      prefix = FString::Printf(TEXT("<%s>"), *tag.tagName);
      break;
    }
  }
  for (auto tag : Target.Tags) {
    if (EndIndex < tag.taggedStart) {
      EndPos = EndIndex + (tag.start - tag.taggedStart);
      break;
    } else if (EndIndex < tag.taggedEnd) {
      EndPos = EndIndex + (tag.start - tag.taggedStart);
      EndPos += tag.tagName.Len() + 2;
      sufix = FString::Printf(TEXT("</%s>"), *tag.tagName);
      break;
    }
  }
  if (EndPos < 0) {
    EndPos = EndIndex + (Target.SourceString.Len() - Target.Length);
  }

  return prefix + Target.SourceString.Mid(StartPos, EndPos - StartPos) + sufix;
}

FString UStringUtils::FlattenTaggedString(const FString &TaggedString) {
  FString ResultString;
  TArray<FString> OpenTags;

  for (int32 i = 0; i < TaggedString.Len(); i++) {
    if (TaggedString[i] == '<') {
      // Check for closing tag
      if (TaggedString[i + 1] == '/') {
        FString CloseTagName;
        int32 CloseTagIndex = TaggedString.Find(">", ESearchCase::IgnoreCase,
                                                ESearchDir::FromStart, i + 2);
        if (CloseTagIndex != -1) {
          CloseTagName = TaggedString.Mid(i + 2, CloseTagIndex - i - 2);

          // Find matching open tag and remove it from the list
          for (int32 j = OpenTags.Num() - 1; j >= 0; j--) {
            if (OpenTags[j].Equals(CloseTagName, ESearchCase::IgnoreCase)) {
              ResultString += FString::Printf(TEXT("</%s>"), *OpenTags[j]);
              OpenTags.RemoveAt(j);
              if (OpenTags.Num() > 0)
                ResultString += FString::Printf(
                    TEXT("<%s>"), *(OpenTags[OpenTags.Num() - 1]));
              break;
            }
          }

          i = CloseTagIndex;
        }
      }
      // Check for opening tag
      else {
        FString OpenTagName;
        int32 OpenTagIndex = TaggedString.Find(">", ESearchCase::IgnoreCase,
                                               ESearchDir::FromStart, i + 1);
        if (OpenTagIndex != -1) {
          if (OpenTags.Num() > 0)
            ResultString +=
                FString::Printf(TEXT("</%s>"), *(OpenTags[OpenTags.Num() - 1]));
          OpenTagName = TaggedString.Mid(i + 1, OpenTagIndex - i - 1);
          OpenTags.Add(OpenTagName);
          ResultString += FString::Printf(TEXT("<%s>"), *OpenTagName);

          i = OpenTagIndex;
        }
      }
    } else {
      ResultString.AppendChar(TaggedString[i]);
    }
  }

  // Close any remaining open tags
  for (int32 i = OpenTags.Num() - 1; i >= 0; i--) {
    ResultString += FString::Printf(TEXT("</%s>"), *OpenTags[i]);
  }

  return ResultString;
}