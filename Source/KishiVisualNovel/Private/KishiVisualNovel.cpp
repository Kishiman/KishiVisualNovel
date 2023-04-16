// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "KishiVisualNovel.h"
#include "CoreMinimal.h"
#include "Utils/StringUtils.h"

#define LOCTEXT_NAMESPACE "FVisualNovelModule"

void FVisualNovelModule::StartupModule()
{
  FString TaggedString = TEXT("Hi <green>hello<red>world</>!</>");
  FString FlattenedString = UStringUtils::FlattenTaggedString(TaggedString);

  UE_LOG(LogTemp, Warning, TEXT("Start Test"));
  UE_LOG(LogTemp, Log, TEXT("Input: %s"), *TaggedString);
  UE_LOG(LogTemp, Log, TEXT("Output: %s"), *FlattenedString);
  FTaggedString taggedString;
  UStringUtils::StringToTaggedString(FlattenedString, taggedString);
  UE_LOG(LogTemp, Log, TEXT("Length: %d"), taggedString.Length);

  for (int32 idx = 1; idx < taggedString.Length; ++idx)
  {
    UE_LOG(LogTemp, Log, TEXT("SubString[%d]: %s"), idx, *(UStringUtils::SubString(taggedString, 0, idx)));
  }
  // This code will execute after your module is loaded into memory; the exact
  // timing is specified in the .uplugin file per-module
}

void FVisualNovelModule::ShutdownModule()
{
  // This function may be called during shutdown to clean up your module.  For
  // modules that support dynamic reloading, we call this function before
  // unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVisualNovelModule, KishiVisualNovel)