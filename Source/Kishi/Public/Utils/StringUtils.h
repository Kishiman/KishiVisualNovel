#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StringUtils.generated.h"

UENUM(BlueprintType)
enum class EMouthViseme : uint8
{
  Neutral UMETA(DisplayName = "Neutral"),
  AI UMETA(DisplayName = "A, I"),
  E UMETA(DisplayName = "E"),
  O UMETA(DisplayName = "O"),
  U UMETA(DisplayName = "U"),
  FV UMETA(DisplayName = "F, V"),
  LDTN UMETA(DisplayName = "L, D, T, N"),
  MBP UMETA(DisplayName = "M, B, P"),
  WQR UMETA(DisplayName = "W, Q, R"),
  SZCHSHJ UMETA(DisplayName = "S, Z, CH, SH, J"),
  TH UMETA(DisplayName = "TH")
};
USTRUCT(BlueprintType)
struct FMouthViseme
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EMouthViseme type = EMouthViseme::Neutral;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 index;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 taggedIndex;
};

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
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString SourceString;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FMouthViseme> visemes;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Length;

  TArray<Tag> Tags;
};

UCLASS()
class KISHI_API UStringUtils : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintPure)
  static FString GetTaggedString(const FTaggedString &Target);

  UFUNCTION(BlueprintPure)
  static TArray<FName> SplitPathtoNames(const FString &Target);

  UFUNCTION(BlueprintPure)
  static void HeadAndTails(const TArray<FName> &Target, FName &Head, TArray<FName> &Tails);

  UFUNCTION(BlueprintCallable)
  static bool StringToTaggedString(const FString &SourceString, FTaggedString &Target);

  UFUNCTION(BlueprintPure)
  static int32 Length(const FTaggedString &Target);

  UFUNCTION(BlueprintPure)
  static FString SubString(const FTaggedString &Target, int32 StartIndex, int32 Length);

  UFUNCTION(BlueprintPure)
  static FMouthViseme getVisemeBetween(const FTaggedString &Target, int32 startIndex, int32 endIndex);

  UFUNCTION(BlueprintPure)
  static FString FlattenTaggedString(const FString &TaggedString);

protected:
private:
};