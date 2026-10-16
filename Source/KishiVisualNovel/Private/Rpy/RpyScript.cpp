#include "Rpy/RpyScript.h"
#include "Rpy/RpyInstruction.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Misc/FileHelper.h"
#include "Rpy/VisualNovelXmlParser.h"
#include "Rpy/RpyInstructions.h"

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
  TArray<FString> nameSegments;
  TArray<FString> pathSegments;
  name.ToString().ParseIntoArrayWS(nameSegments);
  path.ParseIntoArrayWS(pathSegments);
  if (nameSegments.Num() == 0 || pathSegments.Num() == 0)
    return false;
  auto mainName = FName(*nameSegments[0]);
  auto mainPath = FName(*pathSegments[0]);
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

FRpyImage *URpyScript::FindOrAddDefaultImage(FName name, FString attributes)
{
  FName AssetKey = name;
  if (!attributes.IsEmpty())
  {
    AssetKey = FName(*(name.ToString() + TEXT(" ") + attributes));
  }

  if (FRpyImage *Image = images.Find(name))
    return Image;
  if (AssetKey != name)
  {
    if (FRpyImage *Image = images.Find(AssetKey))
      return Image;
  }

  // Attributes describe the displayed state of layered images; they are not
  // part of the image asset path.
  FString AssetPath = name.ToString();
  AssetPath.ReplaceInline(TEXT(" "), TEXT("_"));
  if (!AddDefaultImage(name, AssetPath))
    return nullptr;

  if (FRpyImage *Image = images.Find(name))
    return Image;
  return AssetKey != name ? images.Find(AssetKey) : nullptr;
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
  if (this->AssetImportData)
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
  UE_LOG(LogTemp, Warning, TEXT("PostLoad, instruction count:%d"), instructions.Num());
  Parse();
  Compile();
  LoadRpyData();
};

void URpyScript::PostInitProperties()
{
  Super::PostInitProperties();
  UE_LOG(LogTemp, Warning, TEXT("PostInitProperties, instruction count:%d"), instructions.Num());
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
  UE_LOG(LogTemp, Warning, TEXT("URpyScript(), instruction count:%d"), instructions.Num());
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

void URpyScript::BeginDestroy()
{
  instructions.Empty();
  labels.Empty();
  instructionsById.Empty();
  init = nullptr;
  Super::BeginDestroy();
}

bool URpyScript::ImportXml(FString text)
{
	URpyScript* StagingScript = NewObject<URpyScript>(this, NAME_None, RF_Transient);
	if (!StagingScript)
		return false;

	StagingScript->images = images;
	StagingScript->audios = audios;
	StagingScript->transitions = transitions;
	StagingScript->characters = characters;
	StagingScript->compileData = FDynamicObject();
	StagingScript->xmlGeneratedImageKeys = xmlGeneratedImageKeys;
	StagingScript->xmlGeneratedAudioKeys = xmlGeneratedAudioKeys;
	StagingScript->xmlGeneratedTransitionKeys = xmlGeneratedTransitionKeys;
	StagingScript->xmlGeneratedCharacterKeys = xmlGeneratedCharacterKeys;

	for (FName Key : StagingScript->xmlGeneratedImageKeys) StagingScript->images.Remove(Key);
	for (FName Key : StagingScript->xmlGeneratedAudioKeys) StagingScript->audios.Remove(Key);
	for (FName Key : StagingScript->xmlGeneratedTransitionKeys) StagingScript->transitions.Remove(Key);
	for (FName Key : StagingScript->xmlGeneratedCharacterKeys) StagingScript->characters.Remove(Key);
	StagingScript->xmlGeneratedImageKeys.Empty();
	StagingScript->xmlGeneratedAudioKeys.Empty();
	StagingScript->xmlGeneratedTransitionKeys.Empty();
	StagingScript->xmlGeneratedCharacterKeys.Empty();

	TSet<FName> ExistingImageKeys;
	TSet<FName> ExistingAudioKeys;
	TSet<FName> ExistingTransitionKeys;
	StagingScript->images.GetKeys(StagingScript->xmlGeneratedImageKeys);
	for (FName Key : StagingScript->xmlGeneratedImageKeys) ExistingImageKeys.Add(Key);
	StagingScript->audios.GetKeys(StagingScript->xmlGeneratedAudioKeys);
	for (FName Key : StagingScript->xmlGeneratedAudioKeys) ExistingAudioKeys.Add(Key);
	StagingScript->transitions.GetKeys(StagingScript->xmlGeneratedTransitionKeys);
	for (FName Key : StagingScript->xmlGeneratedTransitionKeys) ExistingTransitionKeys.Add(Key);
	StagingScript->xmlGeneratedImageKeys.Empty();
	StagingScript->xmlGeneratedAudioKeys.Empty();
	StagingScript->xmlGeneratedTransitionKeys.Empty();

	StagingScript->scriptXml = text;
	if (!StagingScript->Parse() || !StagingScript->Compile())
	{
		UE_LOG(LogTemp, Error, TEXT("XML import failed; preserving the previously imported script state."));
		StagingScript->instructions.Empty();
		return false;
	}
	StagingScript->LoadRpyData();

	TArray<FName> FinalKeys;
	StagingScript->images.GetKeys(FinalKeys);
	for (FName Key : FinalKeys) if (!ExistingImageKeys.Contains(Key)) StagingScript->xmlGeneratedImageKeys.AddUnique(Key);
	FinalKeys.Empty();
	StagingScript->audios.GetKeys(FinalKeys);
	for (FName Key : FinalKeys) if (!ExistingAudioKeys.Contains(Key)) StagingScript->xmlGeneratedAudioKeys.AddUnique(Key);
	FinalKeys.Empty();
	StagingScript->transitions.GetKeys(FinalKeys);
	for (FName Key : FinalKeys) if (!ExistingTransitionKeys.Contains(Key)) StagingScript->xmlGeneratedTransitionKeys.AddUnique(Key);

	scriptXml = MoveTemp(StagingScript->scriptXml);
	images = MoveTemp(StagingScript->images);
	audios = MoveTemp(StagingScript->audios);
	transitions = MoveTemp(StagingScript->transitions);
	characters = MoveTemp(StagingScript->characters);
	compileData = MoveTemp(StagingScript->compileData);
	instructions = MoveTemp(StagingScript->instructions);
	labels = MoveTemp(StagingScript->labels);
	instructionsById = MoveTemp(StagingScript->instructionsById);
	init = nullptr;
	xmlGeneratedImageKeys = MoveTemp(StagingScript->xmlGeneratedImageKeys);
	xmlGeneratedAudioKeys = MoveTemp(StagingScript->xmlGeneratedAudioKeys);
	xmlGeneratedTransitionKeys = MoveTemp(StagingScript->xmlGeneratedTransitionKeys);
	xmlGeneratedCharacterKeys = MoveTemp(StagingScript->xmlGeneratedCharacterKeys);
	for (URpyInstruction* Instruction : instructions)
		if (Instruction) Instruction->script = this;
	return true;
}

namespace
{
  FString InferXmlScalarType(const FString &Value)
  {
    if (Value == TEXT("true") || Value == TEXT("false"))
      return TEXT("bool");

    int32 Index = 0;
    if (Value.Len() > 0 && (Value[0] == TEXT('+') || Value[0] == TEXT('-')))
      ++Index;
    bool bHasDigits = false;
    while (Index < Value.Len() && FChar::IsDigit(Value[Index]))
    {
      bHasDigits = true;
      ++Index;
    }
    bool bIsFloat = false;
    if (Index < Value.Len() && Value[Index] == TEXT('.'))
    {
      bIsFloat = true;
      ++Index;
      while (Index < Value.Len() && FChar::IsDigit(Value[Index]))
      {
        bHasDigits = true;
        ++Index;
      }
    }
    if (Index < Value.Len() && (Value[Index] == TEXT('e') || Value[Index] == TEXT('E')))
    {
      bIsFloat = true;
      ++Index;
      if (Index < Value.Len() && (Value[Index] == TEXT('+') || Value[Index] == TEXT('-')))
        ++Index;
      const int32 ExponentStart = Index;
      while (Index < Value.Len() && FChar::IsDigit(Value[Index]))
        ++Index;
      if (Index == ExponentStart)
        return TEXT("string");
    }
    if (!bHasDigits || Index != Value.Len())
      return TEXT("string");
    return bIsFloat ? TEXT("float") : TEXT("integer");
  }

  bool HasXmlAttribute(const FVisualNovelXmlNode *Node, const TCHAR *Name)
  {
    return Node && Node->HasAttribute(Name);
  }

  ERPYTransitionType ParseTransitionType(const FString &Name, ERPYTransitionDirection &OutDirection)
  {
    OutDirection = ERPYTransitionDirection::NONE;
    if (Name.Equals(TEXT("blur"), ESearchCase::IgnoreCase))
      return ERPYTransitionType::BLUR;
    if (Name.Equals(TEXT("dissolve"), ESearchCase::IgnoreCase))
      return ERPYTransitionType::DISSOLVE;
    if (Name.Equals(TEXT("ease"), ESearchCase::IgnoreCase))
      return ERPYTransitionType::EASE;
    if (Name.Equals(TEXT("fade"), ESearchCase::IgnoreCase))
      return ERPYTransitionType::FADE;
    if (Name.Equals(TEXT("move"), ESearchCase::IgnoreCase))
      return ERPYTransitionType::MOVE;
    if (Name.Equals(TEXT("shake"), ESearchCase::IgnoreCase))
      return ERPYTransitionType::SHAKE;
    if (Name.Equals(TEXT("wipe"), ESearchCase::IgnoreCase))
      return ERPYTransitionType::WIPE;
    if (Name.Equals(TEXT("zoomin"), ESearchCase::IgnoreCase) || Name.Equals(TEXT("zoom"), ESearchCase::IgnoreCase))
      return ERPYTransitionType::ZOOM;

    const bool bEase = Name.StartsWith(TEXT("easein"), ESearchCase::IgnoreCase);
    const bool bMove = Name.StartsWith(TEXT("movein"), ESearchCase::IgnoreCase);
    if (bEase || bMove)
    {
      const FString DirectionName = Name.RightChop(6);
      if (DirectionName.Equals(TEXT("left"), ESearchCase::IgnoreCase))
        OutDirection = ERPYTransitionDirection::LEFT;
      else if (DirectionName.Equals(TEXT("right"), ESearchCase::IgnoreCase))
        OutDirection = ERPYTransitionDirection::RIGHT;
      else if (DirectionName.Equals(TEXT("top"), ESearchCase::IgnoreCase) || DirectionName.Equals(TEXT("up"), ESearchCase::IgnoreCase))
        OutDirection = ERPYTransitionDirection::UP;
      else if (DirectionName.Equals(TEXT("bottom"), ESearchCase::IgnoreCase) || DirectionName.Equals(TEXT("down"), ESearchCase::IgnoreCase))
        OutDirection = ERPYTransitionDirection::DOWN;
      if (OutDirection != ERPYTransitionDirection::NONE)
        return bEase ? ERPYTransitionType::EASE : ERPYTransitionType::MOVE;
    }
    return ERPYTransitionType::NONE;
  }

  bool ParseNamedPosition(const FString &Name, FVector &OutPosition)
  {
    OutPosition = FVector::ZeroVector;
    if (Name.Equals(TEXT("left"), ESearchCase::IgnoreCase))
      OutPosition.X = -0.4f;
    else if (Name.Equals(TEXT("right"), ESearchCase::IgnoreCase))
      OutPosition.X = 0.4f;
    else if (Name.Equals(TEXT("center"), ESearchCase::IgnoreCase) || Name.Equals(TEXT("truecenter"), ESearchCase::IgnoreCase))
      return true;
    else
      return false;
    return true;
  }

  bool ParsePositionValue(const FString &Value, FVector &OutPosition)
  {
    if (Value.StartsWith(TEXT("(")))
    {
      FString CoordinatesText = Value;
      CoordinatesText.RemoveFromStart(TEXT("("));
      CoordinatesText.RemoveFromEnd(TEXT(")"));
      TArray<FString> Coordinates;
      CoordinatesText.ParseIntoArray(Coordinates, TEXT(","), true);
      if (Coordinates.Num() < 2 || Coordinates.Num() > 3)
        return false;
      OutPosition = FVector::ZeroVector;
      OutPosition.X = FCString::Atof(*Coordinates[0]);
      OutPosition.Y = FCString::Atof(*Coordinates[1]);
      if (Coordinates.Num() > 2)
        OutPosition.Z = FCString::Atof(*Coordinates[2]);
      return true;
    }
    return ParseNamedPosition(Value, OutPosition);
  }

  FString RequiredValue(const FVisualNovelXmlNode *Node, const TCHAR *FieldName)
  {
    return Node ? Node->GetAttribute(FieldName) : FString();
  }

  bool HasRequiredField(const FVisualNovelXmlNode *Node, const TCHAR *FieldName)
  {
    if (!Node)
      return false;
    if (FCString::Strcmp(FieldName, TEXT("text")) == 0 && Node->GetTag() == TEXT("say"))
      return true;
    if (FCString::Strcmp(FieldName, TEXT("text")) == 0 && Node->GetTag() == TEXT("choice"))
      return HasXmlAttribute(Node, TEXT("text"));
    return HasXmlAttribute(Node, FieldName);
  }

  FString RequiredFieldType(const FVisualNovelXmlNode *Node, const TCHAR *FieldName)
  {
    if (!Node)
      return FString();
    const FString Value = RequiredValue(Node, FieldName);
    if (HasRequiredField(Node, TEXT("value")))
      return InferXmlScalarType(Value);
    return FString();
  }

  bool IsXmlScalarType(const FString& Value, const FString& ExpectedType)
  {
    if (ExpectedType == TEXT("string")) return true;
    if (ExpectedType == TEXT("bool")) return Value == TEXT("true") || Value == TEXT("false");
    if (ExpectedType == TEXT("integer")) return InferXmlScalarType(Value) == TEXT("integer");
    if (ExpectedType == TEXT("float")) return InferXmlScalarType(Value) == TEXT("float") || InferXmlScalarType(Value) == TEXT("integer");
    if (ExpectedType == TEXT("name") || ExpectedType == TEXT("asset")) return !Value.IsEmpty();
    if (ExpectedType == TEXT("vector"))
    {
      TArray<FString> Components;
      Value.ParseIntoArray(Components, TEXT(","), true);
      if (Components.Num() != 3) return false;
      for (FString& Component : Components)
      {
        Component.TrimStartAndEndInline();
        if (InferXmlScalarType(Component) == TEXT("string")) return false;
      }
      return true;
    }
    return false;
  }

  void ReadCustomProps(const FVisualNovelXmlNode* Node, const FRpyInstructionSchema& Schema, FRpyInstructionProps& Props)
  {
    for (const TPair<FName, FString>& Definition : Schema.OptionalProps)
    {
      const FString FieldName = Definition.Key.ToString();
      if (!Node->HasAttribute(*FieldName)) continue;
      FRpyInstructionProp& Prop = Props.Values.AddDefaulted_GetRef();
      Prop.Name = Definition.Key;
      Prop.Type = Definition.Value;
      Prop.Value = Node->GetAttribute(*FieldName);
    }
  }

  bool ValidateCustomInstruction(const FVisualNovelXmlNode* Node, const FRpyInstructionSchema& Schema)
  {
    for (const TPair<FName, FString>& Definition : Schema.RequiredFields)
    {
      const FString FieldName = Definition.Key.ToString();
      if (!Node->HasAttribute(*FieldName))
      {
        UE_LOG(LogTemp, Error, TEXT("Custom instruction '%s' is missing required field '%s'"), *Node->Tag, *FieldName);
        return false;
      }
      if (!IsXmlScalarType(Node->GetAttribute(*FieldName), Definition.Value))
      {
        UE_LOG(LogTemp, Error, TEXT("Custom instruction '%s' field '%s' is not a valid %s"), *Node->Tag, *FieldName, *Definition.Value);
        return false;
      }
    }
    for (const TPair<FName, FString>& Definition : Schema.OptionalProps)
    {
      const FString FieldName = Definition.Key.ToString();
      if (Node->HasAttribute(*FieldName) && !IsXmlScalarType(Node->GetAttribute(*FieldName), Definition.Value))
      {
        UE_LOG(LogTemp, Error, TEXT("Custom instruction '%s' prop '%s' is not a valid %s"), *Node->Tag, *FieldName, *Definition.Value);
        return false;
      }
    }
    for (const FVisualNovelXmlAttribute& Attribute : Node->Attributes)
    {
      const FName AttributeName(*Attribute.Name);
      if (!Schema.RequiredFields.Contains(AttributeName) && !Schema.OptionalProps.Contains(AttributeName))
      {
        UE_LOG(LogTemp, Error, TEXT("Custom instruction '%s' has undeclared field or prop '%s'"), *Node->Tag, *Attribute.Name);
        return false;
      }
    }
    return true;
  }

  bool ValidateXmlInstruction(const FVisualNovelXmlNode *Node)
  {
    const FString Type = Node->GetTag();
    TArray<FString> Required;
    if (Type == TEXT("label"))
      Required = {TEXT("name")};
    else if (Type == TEXT("say"))
      Required = {TEXT("text")};
    else if (Type == TEXT("choice"))
      Required = {TEXT("text")};
    else if (Type == TEXT("scene") || Type == TEXT("show"))
      Required = {TEXT("asset")};
    else if (Type == TEXT("hide"))
      Required = {TEXT("tag")};
    else if (Type == TEXT("jump") || Type == TEXT("call"))
      Required = {TEXT("label")};
    else if (Type == TEXT("pause"))
      Required = {TEXT("duration")};
    else if (Type == TEXT("set"))
    {
      if (!HasRequiredField(Node, TEXT("name")) ||
          (HasRequiredField(Node, TEXT("value")) == HasRequiredField(Node, TEXT("expression"))))
      {
        UE_LOG(LogTemp, Error, TEXT("<set> requires name and exactly one of value or expression"));
        return false;
      }
      return true;
    }
    else if (Type == TEXT("character"))
      Required = {TEXT("name"), TEXT("displayName")};
    else if (Type == TEXT("asset"))
      Required = {TEXT("kind"), TEXT("name"), TEXT("path")};
    else if (Type == TEXT("play") || Type == TEXT("queue"))
      Required = {TEXT("channel"), TEXT("asset")};
    else if (Type == TEXT("voice"))
      Required = {TEXT("asset")};
    else if (Type == TEXT("stop"))
      Required = {TEXT("channel")};
    else if (Type == TEXT("if"))
      Required = {TEXT("condition")};
    else if (Type == TEXT("raw"))
      Required = {TEXT("source")};
    else if (FRpyInstructionRegistry::IsRegistered(FName(*Type)))
    {
      const FRpyInstructionSchema* Schema = FRpyInstructionRegistry::FindSchema(FName(*Type));
      return Schema && ValidateCustomInstruction(Node, *Schema);
    }
    else if (Type != TEXT("menu") && Type != TEXT("else") && Type != TEXT("return") && Type != TEXT("init"))
    {
      UE_LOG(LogTemp, Error, TEXT("Unknown XML instruction type '%s'"), *Type);
      return false;
    }
    for (const FString &FieldName : Required)
    {
      const bool bHas = FieldName == TEXT("text") ? true : HasRequiredField(Node, *FieldName);
      if (!bHas)
      {
        UE_LOG(LogTemp, Error, TEXT("Instruction '%s' is missing required field '%s'"), *Type, *FieldName);
        return false;
      }
    }
    return true;
  }

  void ReadProps(const FVisualNovelXmlNode *Node, FRpyInstructionProps &Props)
  {
    if (!Node)
      return;
    const FString Type = Node->GetTag();
    TSet<FString> Required;
    if (Type == TEXT("label"))
      Required.Add(TEXT("name"));
    else if (Type == TEXT("say"))
      Required.Add(TEXT("text"));
    else if (Type == TEXT("choice"))
      Required.Add(TEXT("text"));
    else if (Type == TEXT("scene") || Type == TEXT("show"))
      Required.Add(TEXT("asset"));
    else if (Type == TEXT("hide"))
      Required.Add(TEXT("tag"));
    else if (Type == TEXT("jump") || Type == TEXT("call"))
      Required.Add(TEXT("label"));
    else if (Type == TEXT("pause"))
      Required.Add(TEXT("duration"));
    else if (Type == TEXT("set"))
    {
      Required.Add(TEXT("name"));
      Required.Add(TEXT("value"));
      Required.Add(TEXT("expression"));
    }
    else if (Type == TEXT("play") || Type == TEXT("queue"))
    {
      Required.Add(TEXT("channel"));
      Required.Add(TEXT("asset"));
    }
    else if (Type == TEXT("voice"))
      Required.Add(TEXT("asset"));
    else if (Type == TEXT("stop"))
      Required.Add(TEXT("channel"));
    else if (Type == TEXT("if"))
      Required.Add(TEXT("condition"));
    else if (Type == TEXT("raw"))
      Required.Add(TEXT("source"));
    else if (Type == TEXT("character"))
    {
      Required.Add(TEXT("name"));
      Required.Add(TEXT("displayName"));
    }
    else if (Type == TEXT("asset"))
    {
      Required.Add(TEXT("kind"));
      Required.Add(TEXT("name"));
      Required.Add(TEXT("path"));
    }
    for (const FVisualNovelXmlAttribute &Attr : Node->Attributes)
    {
      if (Required.Contains(Attr.Name))
        continue;
      if (Attr.Name == TEXT("position"))
      {
        FVector Position;
        if (ParsePositionValue(Attr.Value, Position))
        {
          Props.SetVector(FName(TEXT("position")), Position);
          continue;
        }
      }
      FRpyInstructionProp &Prop = Props.Values.AddDefaulted_GetRef();
      Prop.Name = FName(*Attr.Name);
      Prop.Type = InferXmlScalarType(Attr.Value);
      Prop.Value = Attr.Value;
    }
    if (Type == TEXT("say") && HasXmlAttribute(Node, TEXT("character")))
      Props.SetString(FName(TEXT("speaker")), Node->GetAttribute(TEXT("character")));
  }

  void MergeProps(const FVisualNovelXmlNode *Node, FRpyInstructionProps &Props)
  {
    FRpyInstructionProps Source;
    ReadProps(Node, Source);
    for (const FRpyInstructionProp &Prop : Source.Values)
    {
      if (FRpyInstructionProp *Existing = Props.Values.FindByPredicate([&Prop](const FRpyInstructionProp &Value)
                                                                       { return Value.Name == Prop.Name; }))
        *Existing = Prop;
      else
        Props.Values.Add(Prop);
    }
  }

  int32 CountXmlInstructions(const FVisualNovelXmlNode *Parent)
  {
    int32 Count = 0;
    if (!Parent)
      return Count;
    for (const TUniquePtr<FVisualNovelXmlNode> &ChildPtr : Parent->Children)
    {
      const FVisualNovelXmlNode *Child = ChildPtr.Get();
      if (Child)
      {
        ++Count;
        Count += CountXmlInstructions(Child);
      }
    }
    return Count;
  }

  URpyInstruction *MakeXmlInstruction(URpyScript *Script, const FVisualNovelXmlNode *Node)
  {
    FString Type = Node->GetTag();
    if (Type == TEXT("character"))
      Type = TEXT("defineCharacter");
    if (Type == TEXT("asset"))
      Type = TEXT("defineAsset");
    const FString A = RequiredValue(Node, TEXT("name"));
    const FString B = RequiredValue(Node, TEXT("value"));
    URpyInstruction *Result = nullptr;
    if (Type == TEXT("label"))
    {
      ULabelInstruction *Label = CreateRpyInstruction<ULabelInstruction>(Script);
      Label->name = FName(*A);
      Result = Label;
      Script->labels.Add(FName(*A), Result);
    }
    else if (Type == TEXT("say"))
    {
      FString Speaker;
      FRpyInstructionProps Props;
      ReadProps(Node, Props);
      Speaker = RequiredValue(Node, TEXT("speaker"));
      if (Speaker.IsEmpty())
        Props.GetString(FName(TEXT("speaker")), Speaker);
      FString Statement = Node->GetRichTextContent();
      USayInstruction *Say = CreateRpyInstruction<USayInstruction>(Script);
      Say->name = FName(*Speaker);
      Say->statement = MoveTemp(Statement);
      Result = Say;
      Result->Props = Props;
    }
    else if (Type == TEXT("scene") || Type == TEXT("show") || Type == TEXT("hide"))
    {
      FRpySceneOptions Options;
      FRpyInstructionProps Props;
      ReadProps(Node, Props);
      Props.GetString(FName(TEXT("attributes")), Options.attribute);
      FString TransitionName;
      if (Props.GetString(FName(TEXT("transition")), TransitionName))
      {
        ERPYTransitionDirection Direction;
        const ERPYTransitionType Transition = ParseTransitionType(TransitionName, Direction);
        if (Transition != ERPYTransitionType::NONE)
        {
          Options.with = Transition;
          Options.direction = Direction;
        }
        else
        {
          const FName TransitionKey(*TransitionName);
          if (!Script->transitions.Contains(TransitionKey))
            Script->AddDefaultTransition(TransitionKey, TransitionName);
          if (const FRpyTransition *AssetTransition = Script->transitions.Find(TransitionKey))
          {
            Options.with = AssetTransition->type;
            Options.transition = *AssetTransition;
          }
        }
      }
      Props.GetFloat(FName(TEXT("duration")), Options.transitionTime);
      Props.GetInteger(FName(TEXT("zOrder")), Options.zorder);
      FString Layer;
      if (Props.GetString(FName(TEXT("layer")), Layer))
        Options.layer = FName(*Layer);
      FString Position;
      if (Props.GetVector(FName(TEXT("position")), Options.position))
      {
        Options.positionType = EPositionType::VECTOR;
      }
      else if (Props.GetString(FName(TEXT("position")), Position))
      {
        if (Position.StartsWith(TEXT("(")))
        {
          Options.positionType = EPositionType::VECTOR;
          TArray<FString> Coordinates;
          Position.Mid(1, Position.Len() - 2).ParseIntoArray(Coordinates, TEXT(","), true);
          if (Coordinates.Num() > 0)
            Options.position.X = FCString::Atof(*Coordinates[0]);
          if (Coordinates.Num() > 1)
            Options.position.Y = FCString::Atof(*Coordinates[1]);
          if (Coordinates.Num() > 2)
            Options.position.Z = FCString::Atof(*Coordinates[2]);
        }
        else if (ParseNamedPosition(Position, Options.position))
        {
          Options.positionType = EPositionType::VECTOR;
        }
        else
        {
          Options.positionType = EPositionType::NAME;
          Options.positionName = FName(*Position);
        }
      }
      if (Type == TEXT("scene"))
      {
        USceneInstruction *Scene = CreateRpyInstruction<USceneInstruction>(Script);
        Scene->name = FName(*RequiredValue(Node, TEXT("asset")));
        Scene->Props = PropsFromSceneOptions(Options);
        Result = Scene;
      }
      else if (Type == TEXT("show"))
      {
        UShowInstruction *Show = CreateRpyInstruction<UShowInstruction>(Script);
        Show->name = FName(*RequiredValue(Node, TEXT("asset")));
        Show->Props = PropsFromSceneOptions(Options);
        Result = Show;
      }
      else
      {
        UHideInstruction *Hide = CreateRpyInstruction<UHideInstruction>(Script);
        Hide->tag = FName(*RequiredValue(Node, TEXT("tag")));
        Hide->Props = PropsFromSceneOptions(Options);
        Result = Hide;
      }
    }
    else if (Type == TEXT("pause"))
    {
      UPauseInstruction *Pause = CreateRpyInstruction<UPauseInstruction>(Script);
      Pause->timeInSec = FCString::Atof(*RequiredValue(Node, TEXT("duration")));
      Result = Pause;
    }
    else if (Type == TEXT("menu"))
      Result = CreateRpyInstruction<UMenuInstruction>(Script);
    else if (Type == TEXT("choice"))
    {
      UChoiceInstruction *Choice = CreateRpyInstruction<UChoiceInstruction>(Script);
      Choice->statement = RequiredValue(Node, TEXT("text"));
      Result = Choice;
    }
    else if (Type == TEXT("jump"))
    {
      UJumpInstruction *Jump = CreateRpyInstruction<UJumpInstruction>(Script);
      Jump->label = FName(*RequiredValue(Node, TEXT("label")));
      Result = Jump;
    }
    else if (Type == TEXT("call"))
    {
      UCallInstruction *Call = CreateRpyInstruction<UCallInstruction>(Script);
      Call->label = FName(*RequiredValue(Node, TEXT("label")));
      Result = Call;
    }
    else if (Type == TEXT("return"))
      Result = CreateRpyInstruction<UReturnInstruction>(Script);
    else if (Type == TEXT("play") || Type == TEXT("queue") || Type == TEXT("voice"))
    {
      const FString ChannelName = Type == TEXT("voice") ? TEXT("voice") : RequiredValue(Node, TEXT("channel"));
      const FName Channel(*ChannelName);
      const FString AssetPath = RequiredValue(Node, TEXT("asset"));
      const FName AssetName(*AssetPath);
      FRpyAudioOptions AudioOptions;
      FRpyInstructionProps Props;
      ReadProps(Node, Props);
      float Number = 0.0f;
      bool bFlag = false;

      if (Type == TEXT("voice"))
        AudioOptions.loop = false;
      if (Props.GetBool(FName(TEXT("loop")), bFlag))
        AudioOptions.loop = bFlag;

      if (Props.GetFloat(FName(TEXT("volume")), Number))
        AudioOptions.volume = Number;
      if (Props.GetFloat(FName(TEXT("fadeIn")), Number))
        AudioOptions.fadeIn = Number;
      if (Props.GetFloat(FName(TEXT("fadeOut")), Number))
        AudioOptions.fadeOut = Number;
      if (!Script->audios.Contains(AssetName) && !Script->AddDefaultAudio(AssetName, AssetPath))
        return nullptr;
      if (Type == TEXT("queue"))
      {
        UQueueInstruction *Queue = CreateRpyInstruction<UQueueInstruction>(Script);
        Queue->channel = Channel;
        Queue->name = AssetName;
        Queue->Props = PropsFromAudioOptions(AudioOptions);
        Result = Queue;
      }
      else
      {
        UPlayInstruction *Play = CreateRpyInstruction<UPlayInstruction>(Script);
        Play->channel = Channel;
        Play->name = AssetName;
        Play->Props = PropsFromAudioOptions(AudioOptions);
        Result = Play;
      }
      Result->Props = Props;
    }
    else if (Type == TEXT("stop"))
    {
      UStopInstruction *Stop = CreateRpyInstruction<UStopInstruction>(Script);
      Stop->channel = FName(*RequiredValue(Node, TEXT("channel")));
      Stop->Props = PropsFromAudioOptions(FRpyAudioOptions());
      Result = Stop;
    }
    else if (Type == TEXT("if"))
    {
      FString Condition = RequiredValue(Node, TEXT("condition"));
      const bool bReverse = Condition.StartsWith(TEXT("!"));
      if (bReverse)
        Condition.RightChopInline(1);
      UIfBoolInstruction *If = CreateRpyInstruction<UIfBoolInstruction>(Script);
      If->varName = FName(*Condition);
      If->Props.SetBool(FName(TEXT("reverse")), bReverse);
      Result = If;
    }
    else if (Type == TEXT("set"))
    {
      const FName Name(*A);
      const FString Value = B;
      const bool bHasExpression = HasRequiredField(Node, TEXT("expression"));
      if (bHasExpression)
        Result = CreateRpyInstruction<UUnsupportedInstruction>(Script);
      else if (RequiredFieldType(Node, TEXT("value")) == TEXT("bool"))
      {
        UAssignInstruction *Set = CreateRpyInstruction<UAssignInstruction>(Script);
        Set->varName = Name;
        Set->value = Value;
        Set->valueType = TEXT("bool");
        Result = Set;
      }
      else if (RequiredFieldType(Node, TEXT("value")) == TEXT("integer"))
      {
        UAssignInstruction *Set = CreateRpyInstruction<UAssignInstruction>(Script);
        Set->varName = Name;
        Set->value = Value;
        Set->valueType = TEXT("integer");
        Result = Set;
      }
      else if (RequiredFieldType(Node, TEXT("value")) == TEXT("float"))
      {
        UAssignInstruction *Set = CreateRpyInstruction<UAssignInstruction>(Script);
        Set->varName = Name;
        Set->value = Value;
        Set->valueType = TEXT("float");
        Result = Set;
      }
      else
      {
        UAssignInstruction *Set = CreateRpyInstruction<UAssignInstruction>(Script);
        Set->varName = Name;
        Set->value = Value;
        Set->valueType = TEXT("string");
        Result = Set;
      }
      FRpyInstructionProps ScopeProps;
      ReadProps(Node, ScopeProps);
      FString Scope;
      if (ScopeProps.GetString(FName(TEXT("scope")), Scope) && Scope == TEXT("default"))
      {
        const FString ValueType = RequiredFieldType(Node, TEXT("value"));
        if (ValueType == TEXT("bool"))
          Script->compileData.setValue(Name, Value == TEXT("true"));
        else if (ValueType == TEXT("integer"))
          Script->compileData.setValue(Name, FCString::Atoi(*Value));
        else if (ValueType == TEXT("float"))
          Script->compileData.setValue(Name, FCString::Atof(*Value));
        else if (ValueType == TEXT("string"))
          Script->compileData.setValue(Name, Value);
      }
    }
    else if (Type == TEXT("else"))
      Result = CreateRpyInstruction<UElseInstruction>(Script);
    else if (Type == TEXT("init"))
      Result = CreateRpyInstruction<URpyInstruction>(Script);
    else if (Type == TEXT("defineCharacter"))
    {
      FRpyCharacter Character;
      Character.name = FName(*RequiredValue(Node, TEXT("displayName")));
      Character.image = FName(*RequiredValue(Node, TEXT("name")));
      FRpyInstructionProps CharacterProps;
      ReadProps(Node, CharacterProps);
      FString CharacterValue;
      if (CharacterProps.GetString(FName(TEXT("image")), CharacterValue))
        Character.image = FName(*CharacterValue);
      if (CharacterProps.GetString(FName(TEXT("voice")), CharacterValue))
        Character.voice = FName(*CharacterValue);
      if (CharacterProps.GetString(FName(TEXT("color")), CharacterValue))
        Character.color = FColor::FromHex(CharacterValue);
      Result = CreateRpyInstruction<URpyInstruction>(Script);
      Script->characters.Add(Character.image, Character);
      Script->xmlGeneratedCharacterKeys.AddUnique(Character.image);
      Result->Props.SetString(FName(TEXT("displayName")), RequiredValue(Node, TEXT("displayName")));
    }
    else if (Type == TEXT("defineAsset"))
    {
      FRpyInstructionProps Props;
      ReadProps(Node, Props);
      FString Kind;
      Kind = RequiredValue(Node, TEXT("kind"));
      const FName AssetName(*RequiredValue(Node, TEXT("name")));
      const FString AssetPath = RequiredValue(Node, TEXT("path"));
      if (Kind == TEXT("image"))
      {
        Script->AddDefaultImage(AssetName, AssetPath);
        Script->xmlGeneratedImageKeys.AddUnique(AssetName);
      }
      else if (Kind == TEXT("sound") || Kind == TEXT("music") || Kind == TEXT("audio"))
      {
        Script->AddDefaultAudio(AssetName, AssetPath);
        Script->xmlGeneratedAudioKeys.AddUnique(AssetName);
      }
      else if (Kind == TEXT("transition"))
      {
        Script->AddDefaultTransition(AssetName, AssetPath);
        Script->xmlGeneratedTransitionKeys.AddUnique(AssetName);
      }
      Result = CreateRpyInstruction<URpyInstruction>(Script);
      Result->Props = Props;
    }
    else if (Type == TEXT("raw"))
      Result = CreateRpyInstruction<UUnsupportedInstruction>(Script);

    if (!Result && FRpyInstructionRegistry::IsRegistered(FName(*Type)))
    {
      TMap<FName, FRpyInstructionProp> RequiredFields;
      const FRpyInstructionSchema* Schema = FRpyInstructionRegistry::FindSchema(FName(*Type));
      if (!Schema)
        return nullptr;
      for (const TPair<FName, FString>& Definition : Schema->RequiredFields)
      {
        const FString FieldName = Definition.Key.ToString();
        FRpyInstructionProp Value;
        Value.Name = Definition.Key;
        Value.Type = Definition.Value;
        Value.Value = Node->GetAttribute(*FieldName);
        RequiredFields.Add(Value.Name, Value);
      }
      FRpyInstructionProps CustomProps;
      ReadCustomProps(Node, *Schema, CustomProps);
      Result = FRpyInstructionRegistry::Create(FName(*Type), Script, RequiredFields, CustomProps);
    }

    if (Result)
    {
      Result->InstructionType = FName(*Type);
      Result->InstructionId = FName(*FString::Printf(TEXT("Instruction_%d"), Script->instructions.Num()));
      if (!FRpyInstructionRegistry::IsRegistered(FName(*Type)))
        MergeProps(Node, Result->Props);
    }
    return Result;
  }
}

bool URpyScript::Parse()
{
  instructions.Empty();
  labels.Empty();
  instructionsById.Empty();
  if (!scriptXml.IsEmpty())
  {
    FVisualNovelXmlParser XmlParser;
    TUniquePtr<FVisualNovelXmlNode> Root;
    FString XmlError;
    if (!XmlParser.Parse(scriptXml, Root, XmlError))
    {
      UE_LOG(LogTemp, Error, TEXT("Invalid visual novel XML: %s"), *XmlError);
      return false;
    }
    if (!Root || Root->GetTag() != TEXT("visualNovel") || Root->GetAttribute(TEXT("version")) != TEXT("1"))
    {
      UE_LOG(LogTemp, Error, TEXT("Expected <visualNovel version=\"1\"> root"));
      return false;
    }
    const int32 InstructionCount = CountXmlInstructions(Root.Get());
    instructions.Reserve(InstructionCount);
    TFunction<bool(const FVisualNovelXmlNode *, URpyInstruction *)> ReadInstructionChildren;
    ReadInstructionChildren = [&](const FVisualNovelXmlNode *Parent, URpyInstruction *ParentInstruction) -> bool
    {
      bool bSuccess = true;
      for (const TUniquePtr<FVisualNovelXmlNode> &NodePtr : Parent->Children)
      {
        const FVisualNovelXmlNode *Node = NodePtr.Get();
        if (!Node)
          continue;
        if (!ValidateXmlInstruction(Node))
        {
          bSuccess = false;
          continue;
        }
        URpyInstruction *Instruction = MakeXmlInstruction(this, Node);
        if (!Instruction)
        {
          UE_LOG(LogTemp, Error, TEXT("Unsupported XML instruction '%s'"), *Node->GetTag());
          bSuccess = false;
          continue;
        }
        Instruction->parent = ParentInstruction;
        if (ParentInstruction)
          ParentInstruction->children.Add(Instruction);
        instructions.Add(Instruction);
        instructionsById.Add(Instruction->InstructionId, Instruction);
        if (Node->GetTag() != TEXT("say") && !ReadInstructionChildren(Node, Instruction))
          bSuccess = false;
      }
      return bSuccess;
    };
    return ReadInstructionChildren(Root.Get(), nullptr);
  }
  return false;
};
bool URpyScript::Compile()
{
  TArray<URpyInstruction *> RootInstructions;
  for (URpyInstruction *Instruction : instructions)
    if (Instruction && !Instruction->parent)
      RootInstructions.Add(Instruction);

  TFunction<void(const TArray<URpyInstruction *> &, URpyInstruction *)> LinkSequence;
  LinkSequence = [&LinkSequence](const TArray<URpyInstruction *> &Sequence, URpyInstruction *Continuation)
  {
    URpyInstruction *Next = Continuation;
    for (int32 Index = Sequence.Num() - 1; Index >= 0; --Index)
    {
      URpyInstruction *Instruction = Sequence[Index];
      if (!Instruction) continue;
      Instruction->next = Next;
      if (Instruction->children.Num() > 0)
        LinkSequence(Instruction->children, Next);
      Next = Instruction;
    }
  };
  LinkSequence(RootInstructions, nullptr);

  for (auto instruction : instructions)
  {
    if (!instruction->Compile())
    {
      UE_LOG(LogTemp, Error, TEXT("failed to compile instruction: %s"), *instruction->ToString());
      return false;
    }
  }
  return true;
}

FRpyInstructionSerialization URpyScript::SerializeInstruction(URpyInstruction *instruction)
{
  FRpyInstructionSerialization result;
  if (!instruction)
  {
    result.script = nullptr;
    result.index = -1;
    return result;
  }
  result.script = instruction->script;
  result.index = instruction->script->instructions.IndexOfByKey(instruction);
  result.instructionId = instruction->InstructionId;
  return result;
}
URpyInstruction *URpyScript::DeserializeInstruction(const FRpyInstructionSerialization &data)
{
  if (!data.script.IsValid())
  {
    // Load the script synchronously
    data.script.LoadSynchronous();
  }

  URpyScript *Script = data.script.Get();
  if (!Script)
    return nullptr;

  if (!data.instructionId.IsNone())
  {
    if (URpyInstruction **ById = Script->instructionsById.Find(data.instructionId))
      return *ById;
  }

  return Script->instructions.IsValidIndex(data.index)
             ? Script->instructions[data.index]
             : nullptr;
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
