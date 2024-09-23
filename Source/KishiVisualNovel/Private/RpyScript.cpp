#include "Rpy/RpyScript.h"
#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstruction.h"
#include "Rpy/RpyInterpreter.h"
#include "Misc/FileHelper.h"

// Online IDE - Code Editor, parser, Interpreter

#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <string>

using namespace std;
FRpyImage FRpyImage::Make(FString name, FString path)
{
  FRpyImage rpyImage = {nullptr, FName(name), path};
  auto names = RpyParser::GetNames(name);
  rpyImage.tag = names[0];
  return rpyImage;
}
FRpyLayeredImage FRpyLayeredImage::Make(FName name, FString path)
{
  FRpyLayeredImage rpyLayeredImage = {nullptr, name, path};
  return rpyLayeredImage;
}

bool URpyScript::IsAssetUnderProjectContent()
{
  FString PackagePath = this->AssetImportData->GetPathName();

  // Check if the package path starts with "/Game/"
  return false;
}

bool URpyScript::IsAssetUnderPluginContent()
{
  FString PackagePath = this->AssetImportData->GetPathName();

  // Check if the package path starts with "/Plugins/"
  return PackagePath.StartsWith(TEXT("/Plugins/"));
}
bool URpyScript::AddDefaultImage(FString param, TArray<FName> names)
{
  TArray<FString> searchPaths;
  FString path;
  UPaperSprite *image = nullptr;
  ULayeredSprite *layeredImage = nullptr;
  FString searchParam = param.Replace(TEXT(" "), TEXT("_"));
  searchPaths.Add("/Game/" + searchParam + "_Sprite");
  searchPaths.Add("/Game/Images/" + searchParam + "_Sprite");
  searchPaths.Add("/KishiVisualNovel/" + searchParam + "_Sprite");
  searchPaths.Add("/KishiVisualNovel/Images/" + searchParam + "_Sprite");
  auto mainName = names[0];
  auto layeredImageSearch = mainName.ToString();
  searchPaths.Add("/Game/" + layeredImageSearch);
  searchPaths.Add("/Game/Images/" + layeredImageSearch);
  searchPaths.Add("/KishiVisualNovel/" + layeredImageSearch);
  searchPaths.Add("/KishiVisualNovel/Images/" + layeredImageSearch);
  UE_LOG(LogTemp, Error, TEXT("searching for image : %s(%s)"), (*searchParam), (*layeredImageSearch));

  for (auto &searchPath : searchPaths)
  {
    if (FPackageName::DoesPackageExist(searchPath))
    {
      image = Cast<UPaperSprite>(StaticLoadObject(UPaperSprite::StaticClass(), NULL, *searchPath));
      layeredImage = Cast<ULayeredSprite>(StaticLoadObject(ULayeredSprite::StaticClass(), NULL, *searchPath));
      if (image || layeredImage)
      {
        path = searchPath;
        break;
      }
    }
  }
  if (path == "")
  {
    UE_LOG(LogTemp, Error, TEXT("Could Not Found Default Image : %s"), (*searchParam));
    return false;
  }
  if (image)
  {
    FRpyImage rpyImage = FRpyImage::Make(param, path);
    rpyImage.image = image;
    this->images.Add(rpyImage.name, rpyImage);
    return true;
  }
  else if (layeredImage)
  {
    FRpyLayeredImage rpyLayeredImage = FRpyLayeredImage::Make(mainName, path);
    rpyLayeredImage.image = layeredImage;
    this->layeredImages.Add(rpyLayeredImage.name, rpyLayeredImage);
    return true;
  }
  UE_LOG(LogTemp, Error, TEXT("Unvalid Default Image : %s"), (*searchParam));
  return false;
}
bool URpyScript::AddDefaultAudio(FString param)
{
  TArray<FString> searchPaths;
  FString path;
  USoundWave *soundWave = nullptr;
  FString searchParam = param.Replace(TEXT(" "), TEXT("_"));
  searchPaths.Add("/Game/" + searchParam);
  searchPaths.Add("/Game/Audio/" + searchParam);
  searchPaths.Add("/KishiVisualNovel/" + searchParam);
  searchPaths.Add("/KishiVisualNovel/Audio/" + searchParam);
  for (auto &searchPath : searchPaths)
  {
    if (FPackageName::DoesPackageExist(searchPath))
    {
      soundWave = Cast<USoundWave>(StaticLoadObject(USoundWave::StaticClass(), NULL, *searchPath));
      if (soundWave)
      {
        path = searchPath;
        break;
      }
    }
  }
  if (path == "")
  {
    UE_LOG(LogTemp, Error, TEXT("Could Not Found Default Audio : %s"), (*searchParam));
    return false;
  }
  FRpyAudio rpyAudio = {soundWave, path};
  this->audios.Add(FName(param), rpyAudio);
  return true;
}
void URpyScript::LoadRpyData()
{
  FString basePath, right;
  FText err;
  this->AssetImportData->GetPathName().Split("/", &basePath, &right, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
  TArray<FName> keys;
  // load images
  images.GetKeys(keys);
  for (auto &key : keys)
  {
    FRpyImage &rpyImage = images[key];
    FString path = rpyImage.path;
    try
    {
      if (!FFileHelper::IsFilenameValidForSaving(path, err))
      {
        UE_LOG(LogTemp, Error, TEXT("error :%s"), (*err.ToString()));
        continue;
      }
      UPaperSprite *image = Cast<UPaperSprite>(StaticLoadObject(UPaperSprite::StaticClass(), NULL, *path));
      if (image)
      {
        rpyImage.image = image;
      }
      else
      {
        UE_LOG(LogTemp, Error, TEXT("RpyImage not found at path : %s"), (*path));
      }
    }
    catch (const std::exception &e)
    {
      UE_LOG(LogTemp, Error, TEXT("error at path : %s"), (*path));
      UE_LOG(LogTemp, Error, TEXT("Caught exception: %s"), e.what());
      continue;
    }
  };
  keys.Empty();
  // load layered images
  layeredImages.GetKeys(keys);
  for (auto &key : keys)
  {
    FRpyLayeredImage &rpyLayeredImage = layeredImages[key];
    FString path = rpyLayeredImage.path;
    try
    {
      if (!FFileHelper::IsFilenameValidForSaving(path, err))
      {
        UE_LOG(LogTemp, Error, TEXT("error :%s"), (*err.ToString()));
        continue;
      }
      ULayeredSprite *image = Cast<ULayeredSprite>(StaticLoadObject(ULayeredSprite::StaticClass(), NULL, *path));
      if (image)
      {
        rpyLayeredImage.image = image;
      }
      else
      {
        UE_LOG(LogTemp, Error, TEXT("RpyLayeredImage not found at path : %s"), (*path));
      }
    }
    catch (const std::exception &e)
    {
      UE_LOG(LogTemp, Error, TEXT("error at path : %s"), (*path));
      UE_LOG(LogTemp, Error, TEXT("Caught exception: %s"), e.what());
      continue;
    }
  };
  keys.Empty();
  // audios
  audios.GetKeys(keys);
  for (auto &key : keys)
  {
    FRpyAudio &rpyAudio = audios[key];
    FString path = rpyAudio.path;
    try
    {
      if (!FFileHelper::IsFilenameValidForSaving(path, err))
      {
        UE_LOG(LogTemp, Error, TEXT("error :%s"), (*err.ToString()));
        continue;
      }
      USoundWave *audio = Cast<USoundWave>(StaticLoadObject(USoundWave::StaticClass(), NULL, *path));
      if (audio)
      {
        rpyAudio.audio = audio;
      }
      else
      {
        UE_LOG(LogTemp, Error, TEXT("FRpyAudio not found at path : %s"), (*path));
      }
    }
    catch (const std::exception &e)
    {
      UE_LOG(LogTemp, Error, TEXT("error at path : %s"), (*path));
      UE_LOG(LogTemp, Error, TEXT("Caught exception: %s"), e.what());
      continue;
    }
  };
  keys.Empty();
}
void URpyScript::PostLoad()
{
  Super::PostLoad();
  UE_LOG(LogTemp, Warning, TEXT("PostLoad, rpyLines:%d"), rpyLines.Num());
  Parse();
  Compile();
  LoadRpyData();
};

void URpyScript::PostInitProperties()
{
  Super::PostInitProperties();
  UE_LOG(LogTemp, Warning, TEXT("PostInitProperties, rpyLines:%d"), rpyLines.Num());
  Parse();
  Compile();
  LoadRpyData();
};
void URpyScript::Serialize(FStructuredArchiveRecord Record)
{
  Super::Serialize(Record);
  // this->PostLoad();
};

URpyScript::URpyScript() : UKishiDataAsset()
{
  UE_LOG(LogTemp, Warning, TEXT("URpyScript(), rpyLines:%d"), rpyLines.Num());
  Parse();
  Compile();
  LoadRpyData();
};
TArray<FName> URpyScript::GetLabels() const
{
  TArray<FName> out;
  labels.GetKeys(out);
  return out;
};
int URpyScript::GetInstructionsLength() const { return instructions.Num(); };

bool URpyScript::ImportRpyLines(FString text, uint8 TabSize)
{
  TArray<FString> lines;
  text.ParseIntoArrayLines(lines, false);
  rpyLines.Empty();
  rpyLines.Reserve(lines.Num());
  for (int idx = 0; idx < lines.Num(); ++idx)
  {
    FRpyLine rpyLine;
    // uint8 tabs=0;
    while (rpyLine.tabs < lines[idx].Len() && (lines[idx][rpyLine.tabs] == ' ' || lines[idx][rpyLine.tabs] == '\t'))
    {
      ++rpyLine.tabs;
    }
    if (rpyLine.tabs == lines[idx].Len())
      continue;
    if (lines[idx][rpyLine.tabs] == '#')
      continue;
    rpyLine.lineNumber = idx + 1;
    rpyLine.line = lines[idx].RightChop(rpyLine.tabs).TrimStartAndEnd();
    rpyLine.tabs = (rpyLine.tabs + 1) / TabSize;
    rpyLines.Add(rpyLine);
  }
  return true;
};

bool URpyScript::Parse()
{
  UE_LOG(LogTemp, Warning, TEXT("RpyParser::parsers %d"), RpyParser::parsers.Num());
  bool success = true;
  //
  for (auto instruction : instructions)
  {
    delete instruction;
  }
  instructions.Empty();
  for (auto &rpyLine : rpyLines)
  {
    bool matched = false;
    for (auto parser : RpyParser::parsers)
    {
      std::smatch m;
      string target = TCHAR_TO_UTF8(*rpyLine.line);
      std::regex &query = parser->reg_query;
      matched = std::regex_match(target, m, query);
      if (matched)
      {
        UE_LOG(LogTemp, Display, TEXT("matched query[%s]:%s"), *parser->parserName, *parser->query);
        TArray<FString> params;
        int counter = -1;
        for (auto &param : m)
        {
          ++counter;
          if (counter)
          {
            string s = param;
            FString fs = s.c_str();
            UE_LOG(LogTemp, Display, TEXT("param[%d]:%s"), counter - 1, (*fs));
            params.Add(fs);
          }
        }
        if (params.Num() != parser->paramsNum)
        {
          matched = false;
          UE_LOG(LogTemp, Warning, TEXT("number of params(%d) not equal to parser (%d)"), params.Num(), parser->paramsNum);
          continue;
        }
        RpyInstruction *instruction = parser->GetRpyInstruction(this, &rpyLine, params);
        if (!instruction)
        {
          matched = false;
          UE_LOG(LogTemp, Warning, TEXT("Failed to GetRpyInstruction from %s at line %d : %s"), *parser->parserName, rpyLine.lineNumber, (*rpyLine.line));
          continue;
        }
        instructions.Add(instruction);
        break;
      }
    }
    if (!matched)
    {
      UE_LOG(LogTemp, Error, TEXT("Failed to match line %d : %s"), rpyLine.lineNumber, (*rpyLine.line));
      success = false;
    }
  }
  return success;
};
bool URpyScript::Compile()
{
  // TODO
  for (int idx = 0; idx < instructions.Num() - 1; ++idx)
  {
    instructions[idx]->next = instructions[idx + 1];
  }
  TArray<RpyInstruction *> stack;
  RpyInstruction *current;
  for (int idx = 0; idx < instructions.Num(); ++idx)
  {
    auto num = stack.Num();
    RpyInstruction *last = num > 0 ? stack[num - 1] : nullptr;
    current = instructions[idx];
    int currentTabs = current->rpyLine->tabs;
    if (last)
    {
      int lastTabs = last->rpyLine->tabs;
      if (currentTabs == lastTabs + 1)
      {
        current->parent = last;
        stack.Add(current);
      }
      else if (currentTabs <= lastTabs)
      {
        int count = num - currentTabs;
        while (count--)
        {
          stack.Pop()->next = current;
        }
        current->parent = currentTabs > 0 ? stack[currentTabs - 1] : nullptr;
        stack.Add(current);
      }
      else
      {
        UE_LOG(LogTemp, Error, TEXT("unvalid tabs transition at line :%d"), current->rpyLine->lineNumber);
        return false;
      }
    }
    else if (currentTabs == 0)
    {
      stack.Add(current);
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("unvalid tabs transition at line :%d"), current->rpyLine->lineNumber);
      return false;
    }
    if (current->parent)
    {
      current->parent->children.Add(current);
    }
  }
  current = nullptr;
  for (auto instruction : instructions)
  {
    if (!instruction->Compile())
    {
      UE_LOG(LogTemp, Error, TEXT("failed to compile line %d : %s"), instruction->rpyLine->lineNumber, (*instruction->rpyLine->line));
      return false;
    }
  }
  return true;
}
