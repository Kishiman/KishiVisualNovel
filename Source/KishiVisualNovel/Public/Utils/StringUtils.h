#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StringUtils.generated.h"

struct Tag
{
  FString tagName;
  int32 start;
  int32 end;
  int32 taggedStart;
  int32 taggedEnd;
};

USTRUCT(BlueprintType)
struct FTaggedString
{
  GENERATED_BODY()

public:
  FString SourceString;
  TArray<Tag> Tags;
  int32 Length;
};

UCLASS()
class KISHIVISUALNOVEL_API UStringUtils : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintPure)
  static FString Get(const FTaggedString &Target);

  UFUNCTION(BlueprintCallable)
  static bool Set(FTaggedString &Target, FString SourceString);

  UFUNCTION(BlueprintPure)
  static int32 Length(const FTaggedString &Target);

  UFUNCTION(BlueprintPure)
  static FString SubString(const FTaggedString &Target, int32 StartIndex, int32 Length);
  UFUNCTION(BlueprintPure)
  static FString FlattenTaggedString(const FString &TaggedString);

protected:
private:
};