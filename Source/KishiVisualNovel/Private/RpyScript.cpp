#include "Rpy/RpyScript.h"
#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstruction.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Misc/FileHelper.h"

// Online IDE - Code Editor, parser, Interpreter

#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <string>

using namespace std;

FRpyImage FRpyImage::MakeLayeredImage(FName name, FString path)
{
  FRpyImage rpyLayeredImage;
  rpyLayeredImage.type = ERpyImageType::ELAYERED_SPRITE;
  rpyLayeredImage.name = name;
  rpyLayeredImage.path = path;
  return rpyLayeredImage;
}

bool URpyScript::AddDefaultTransition(FName name, FString path)
{
  TArray<FString> searchPaths;
  FString foundPath;
  ULevelSequence *levelSequence = nullptr;
  FString searchParam = path.Replace(TEXT(" "), TEXT("_"));
  searchPaths.Add("/Game/" + searchParam);
  searchPaths.Add("/Game/Transitions/" + searchParam);
  searchPaths.Add("/KishiVisualNovel/" + searchParam);
  searchPaths.Add("/KishiVisualNovel/Transitions/" + searchParam);

  for (auto &searchPath : searchPaths)
  {
    if (FPackageName::DoesPackageExist(searchPath))
    {
      levelSequence = Cast<ULevelSequence>(StaticLoadObject(ULevelSequence::StaticClass(), NULL, *searchPath));
      if (levelSequence)
      {
        foundPath = searchPath;
        break;
      }
    }
  }
  if (foundPath == "")
  {
    UE_LOG(LogTemp, Error, TEXT("Could Not Found Default Transition : %s"), (*searchParam));
    return false;
  }
  if (levelSequence)
  {
    FRpyTransition rpyTransition;
    rpyTransition.type = ERPYTransitionType::LEVEL_SEQUENCE;
    rpyTransition.name = name;
    rpyTransition.levelSequence = levelSequence;
    rpyTransition.path = foundPath;
    this->transitions.Add(rpyTransition.name, rpyTransition);
    return true;
  }
  UE_LOG(LogTemp, Error, TEXT("Unvalid Default Transition : %s"), (*searchParam));
  return false;
}

bool URpyScript::AddDefaultImage(FName name, FString path)
{
  auto nameSegments = RpyParser::GetNames(name.ToString());
  auto pathSegments = RpyParser::GetNames(path);
  auto mainName = nameSegments[0];
  auto mainPath = pathSegments[0];
  TArray<FString> searchPaths;
  FString foundPath;
  UPaperSprite *sprite = nullptr;
  ULayeredSprite *layeredImage = nullptr;
  ULevelSequence *levelSequence = nullptr;
  UVideoMediaSprite *videoMediaSprite = nullptr;
  FString searchParam = path.Replace(TEXT(" "), TEXT("_"));
  auto layeredImageSearch = mainPath.ToString();
  searchPaths.Add("/Game/" + layeredImageSearch);
  searchPaths.Add("/Game/Images/" + layeredImageSearch);
  searchPaths.Add("/KishiVisualNovel/" + layeredImageSearch);
  searchPaths.Add("/KishiVisualNovel/Images/" + layeredImageSearch);

  searchPaths.Add("/Game/" + searchParam + "_Sprite");
  searchPaths.Add("/Game/Images/" + searchParam + "_Sprite");
  searchPaths.Add("/KishiVisualNovel/" + searchParam + "_Sprite");
  searchPaths.Add("/KishiVisualNovel/Images/" + searchParam + "_Sprite");

  for (auto &searchPath : searchPaths)
  {
    if (FPackageName::DoesPackageExist(searchPath))
    {
      sprite = Cast<UPaperSprite>(StaticLoadObject(UPaperSprite::StaticClass(), NULL, *searchPath));
      layeredImage = Cast<ULayeredSprite>(StaticLoadObject(ULayeredSprite::StaticClass(), NULL, *searchPath));
      levelSequence = Cast<ULevelSequence>(StaticLoadObject(ULevelSequence::StaticClass(), NULL, *searchPath));
      videoMediaSprite = Cast<UVideoMediaSprite>(StaticLoadObject(UVideoMediaSprite::StaticClass(), NULL, *searchPath));
      if (sprite || layeredImage || levelSequence || videoMediaSprite)
      {
        foundPath = searchPath;
        break;
      }
    }
  }
  if (foundPath == "")
  {
    UE_LOG(LogTemp, Error, TEXT("Could Not Found Default Image : %s"), (*searchParam));
    return false;
  }
  FRpyImage rpyImage;
  if (sprite)
  {
    rpyImage.type = ERpyImageType::ESPRITE;
    rpyImage.name = name;
    rpyImage.sprite = sprite;
    rpyImage.path = foundPath;
    rpyImage.tag = mainName;
    this->images.Add(rpyImage.name, rpyImage);
    return true;
  }
  else if (layeredImage)
  {
    rpyImage.type = ERpyImageType::ELAYERED_SPRITE;
    rpyImage.name = mainName;
    rpyImage.tag = mainName;
    rpyImage.layeredSprite = layeredImage;
    rpyImage.path = foundPath;
    this->images.Add(rpyImage.name, rpyImage);
    return true;
  }
  else if (levelSequence)
  {
    rpyImage.type = ERpyImageType::ELEVEL_SEQUENCE;
    rpyImage.name = mainName;
    rpyImage.tag = mainName;
    rpyImage.levelSequence = levelSequence;
    rpyImage.path = foundPath;
    this->images.Add(rpyImage.name, rpyImage);
    return true;
  }
  else if (videoMediaSprite)
  {
    rpyImage.type = ERpyImageType::EVIDEO_MEDIA_SPRITE;
    rpyImage.name = mainName;
    rpyImage.tag = mainName;
    rpyImage.videoMediaSprite = videoMediaSprite;
    rpyImage.path = foundPath;
    this->images.Add(rpyImage.name, rpyImage);
    return true;
  }
  UE_LOG(LogTemp, Error, TEXT("Unvalid Default Image : %s"), (*searchParam));
  return false;
}
bool URpyScript::AddDefaultAudio(FName name, FString path)
{
  TArray<FString> searchPaths;
  FString foundPath;
  USoundWave *soundWave = nullptr;
  FString searchParam = path.Replace(TEXT(" "), TEXT("_"));
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
        foundPath = searchPath;
        break;
      }
    }
  }
  if (foundPath == "")
  {
    UE_LOG(LogTemp, Error, TEXT("Could Not Found Default Audio : %s"), (*searchParam));
    return false;
  }
  FRpyAudio rpyAudio = {soundWave, foundPath};
  this->audios.Add(name, rpyAudio);
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
      switch (rpyImage.type)
      {
      case ERpyImageType::ESPRITE:
        rpyImage.sprite = Cast<UPaperSprite>(StaticLoadObject(UPaperSprite::StaticClass(), NULL, *path));
        break;
      case ERpyImageType::ELAYERED_SPRITE:
        rpyImage.layeredSprite = Cast<ULayeredSprite>(StaticLoadObject(ULayeredSprite::StaticClass(), NULL, *path));
        break;
      case ERpyImageType::ELEVEL_SEQUENCE:
        rpyImage.levelSequence = Cast<ULevelSequence>(StaticLoadObject(ULevelSequence::StaticClass(), NULL, *path));
        break;
      case ERpyImageType::EVIDEO_MEDIA_SPRITE:
        rpyImage.videoMediaSprite = Cast<UVideoMediaSprite>(StaticLoadObject(UVideoMediaSprite::StaticClass(), NULL, *path));
        if (!rpyImage.videoMediaSprite)
        {
          UE_LOG(LogTemp, Error, TEXT("FRpyImage not found at path : %s"), (*path));
        }
        break;
      default:
        break;
      }
      if (!rpyImage.sprite && !rpyImage.layeredSprite && !rpyImage.levelSequence && !rpyImage.videoMediaSprite)
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

#if WITH_EDITORONLY_DATA

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

#endif