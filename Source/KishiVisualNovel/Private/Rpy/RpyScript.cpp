#include "Rpy/RpyScript.h"
#include "Rpy/RpyInstruction.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Misc/FileHelper.h"
#include "XmlFile.h"
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
  if (nameSegments.Num() == 0 || pathSegments.Num() == 0) return false;
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

bool URpyScript::ImportXml(FString text)
{
  scriptXml = MoveTemp(text);
  const bool bParsed = Parse();
  if (!bParsed || !Compile()) return false;
  LoadRpyData();
  return true;
}

namespace
{
  bool HasXmlAttribute(const FXmlNode* Node, const TCHAR* Name)
  {
    if (!Node) return false;
    for (const FXmlAttribute& Attribute : Node->GetAttributes())
      if (Attribute.GetTag() == Name) return true;
    return false;
  }

  ERPYTransitionType ParseTransitionType(const FString& Name)
  {
    if (Name.Equals(TEXT("blur"), ESearchCase::IgnoreCase)) return ERPYTransitionType::BLUR;
    if (Name.Equals(TEXT("dissolve"), ESearchCase::IgnoreCase)) return ERPYTransitionType::DISSOLVE;
    if (Name.Equals(TEXT("ease"), ESearchCase::IgnoreCase)) return ERPYTransitionType::EASE;
    if (Name.Equals(TEXT("fade"), ESearchCase::IgnoreCase)) return ERPYTransitionType::FADE;
    if (Name.Equals(TEXT("move"), ESearchCase::IgnoreCase)) return ERPYTransitionType::MOVE;
    if (Name.Equals(TEXT("shake"), ESearchCase::IgnoreCase)) return ERPYTransitionType::SHAKE;
    if (Name.Equals(TEXT("wipe"), ESearchCase::IgnoreCase)) return ERPYTransitionType::WIPE;
    if (Name.Equals(TEXT("zoomin"), ESearchCase::IgnoreCase) || Name.Equals(TEXT("zoom"), ESearchCase::IgnoreCase)) return ERPYTransitionType::ZOOM;
    return ERPYTransitionType::NONE;
  }

  FString DescribeXmlInstruction(const FXmlNode* Node)
  {
    FString Description = Node->GetTag();
    for (const FXmlAttribute& Attribute : Node->GetAttributes())
    {
      Description += FString::Printf(TEXT(" %s=\"%s\""), *Attribute.GetTag(), *Attribute.GetValue());
    }
    const FString Content = Node->GetContent().TrimStartAndEnd();
    if (!Content.IsEmpty()) Description += TEXT("  ") + Content;
    return Description;
  }

  struct RpyUnsupportedInstruction : public RpyInstruction
  {
    RpyUnsupportedInstruction(URpyScript* Script, FRpyLine* Line) : RpyInstruction(Script, Line) {}
    virtual bool Execute(URpySession*) override
    {
      UE_LOG(LogTemp, Error, TEXT("Unsupported migrated instruction at line %d: %s"), rpyLine.lineNumber, *rpyLine.line);
      return false;
    }
  };

  FString RequiredValue(const FXmlNode* Node, const TCHAR* FieldName)
  {
    return Node ? Node->GetAttribute(FieldName) : FString();
  }

  bool HasRequiredField(const FXmlNode* Node, const TCHAR* FieldName)
  {
    if (!Node) return false;
    if (FCString::Strcmp(FieldName, TEXT("text")) == 0 && Node->GetTag() == TEXT("say")) return true;
    if (FCString::Strcmp(FieldName, TEXT("text")) == 0 && Node->GetTag() == TEXT("choice")) return HasXmlAttribute(Node, TEXT("text"));
    return HasXmlAttribute(Node, FieldName);
  }

  FString RequiredFieldType(const FXmlNode* Node, const TCHAR* FieldName)
  {
    if (!Node) return FString();
    const FString Value = RequiredValue(Node, FieldName);
    if (Value == TEXT("true") || Value == TEXT("false")) return TEXT("bool");
    if (FCString::Atoi(*Value) != 0 && Value.IsNumeric()) return TEXT("integer");
    if (Value.IsNumeric() && Value.Contains(TEXT("."))) return TEXT("float");
    if (HasRequiredField(Node, TEXT("value"))) return TEXT("string");
    return FString();
  }

  bool ValidateXmlInstruction(const FXmlNode* Node)
  {
    const FString Type = Node->GetTag();
    TArray<FString> Required;
    if (Type == TEXT("label")) Required = {TEXT("name")};
    else if (Type == TEXT("say")) Required = {TEXT("text")};
    else if (Type == TEXT("choice")) Required = {TEXT("text")};
    else if (Type == TEXT("scene") || Type == TEXT("show")) Required = {TEXT("asset")};
    else if (Type == TEXT("hide")) Required = {TEXT("tag")};
    else if (Type == TEXT("jump") || Type == TEXT("call")) Required = {TEXT("label")};
    else if (Type == TEXT("pause")) Required = {TEXT("duration")};
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
    else if (Type == TEXT("character")) Required = {TEXT("name"), TEXT("displayName")};
    else if (Type == TEXT("asset")) Required = {TEXT("kind"), TEXT("name"), TEXT("path")};
    else if (Type == TEXT("play") || Type == TEXT("queue")) Required = {TEXT("channel"), TEXT("asset")};
    else if (Type == TEXT("voice")) Required = {TEXT("asset")};
    else if (Type == TEXT("stop")) Required = {TEXT("channel")};
    else if (Type == TEXT("if")) Required = {TEXT("condition")};
    else if (Type == TEXT("raw")) Required = {TEXT("source")};
    else if (Type != TEXT("menu") && Type != TEXT("else") && Type != TEXT("return") && Type != TEXT("init") &&
             !FRpyInstructionRegistry::IsRegistered(FName(*Type)))
    {
      UE_LOG(LogTemp, Error, TEXT("Unknown XML instruction type '%s'"), *Type);
      return false;
    }
    for (const FString& FieldName : Required)
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

  void ReadProps(const FXmlNode* Node, FRpyInstructionProps& Props)
  {
    if (!Node) return;
    const FString Type = Node->GetTag();
    TSet<FString> Required;
    if (Type == TEXT("label")) Required.Add(TEXT("name"));
    else if (Type == TEXT("say")) Required.Add(TEXT("text"));
    else if (Type == TEXT("choice")) Required.Add(TEXT("text"));
    else if (Type == TEXT("scene") || Type == TEXT("show")) Required.Add(TEXT("asset"));
    else if (Type == TEXT("hide")) Required.Add(TEXT("tag"));
    else if (Type == TEXT("jump") || Type == TEXT("call")) Required.Add(TEXT("label"));
    else if (Type == TEXT("pause")) Required.Add(TEXT("duration"));
    else if (Type == TEXT("set")) { Required.Add(TEXT("name")); Required.Add(TEXT("value")); Required.Add(TEXT("expression")); }
    else if (Type == TEXT("play") || Type == TEXT("queue")) { Required.Add(TEXT("channel")); Required.Add(TEXT("asset")); }
    else if (Type == TEXT("voice")) Required.Add(TEXT("asset"));
    else if (Type == TEXT("stop")) Required.Add(TEXT("channel"));
    else if (Type == TEXT("if")) Required.Add(TEXT("condition"));
    else if (Type == TEXT("raw")) Required.Add(TEXT("source"));
    else if (Type == TEXT("character")) { Required.Add(TEXT("name")); Required.Add(TEXT("displayName")); }
    else if (Type == TEXT("asset")) { Required.Add(TEXT("kind")); Required.Add(TEXT("name")); Required.Add(TEXT("path")); }
    for (const FXmlAttribute& Attr : Node->GetAttributes())
    {
      if (Required.Contains(Attr.GetTag())) continue;
      FRpyInstructionProp& Prop = Props.Values.AddDefaulted_GetRef();
      Prop.Name = FName(*Attr.GetTag());
      Prop.Type = (Attr.GetValue() == TEXT("true") || Attr.GetValue() == TEXT("false")) ? TEXT("bool") : (Attr.GetValue().IsNumeric() ? (Attr.GetValue().Contains(TEXT(".")) ? TEXT("float") : TEXT("integer")) : TEXT("string"));
      Prop.Value = Attr.GetValue();
    }
    if (Type == TEXT("say") && HasXmlAttribute(Node, TEXT("character"))) Props.SetString(FName(TEXT("speaker")), Node->GetAttribute(TEXT("character")));
    if (Type == TEXT("say")) Props.SetString(FName(TEXT("text")), Node->GetContent());
  }

  void MergeProps(const FXmlNode* Node, FRpyInstructionProps& Props)
  {
    FRpyInstructionProps Source;
    ReadProps(Node, Source);
    for (const FRpyInstructionProp& Prop : Source.Values)
    {
      if (FRpyInstructionProp* Existing = Props.Values.FindByPredicate([&Prop](const FRpyInstructionProp& Value) { return Value.Name == Prop.Name; }))
        *Existing = Prop;
      else
        Props.Values.Add(Prop);
    }
  }

  int32 CountXmlInstructions(const FXmlNode* Parent)
  {
    int32 Count = 0;
    if (!Parent) return Count;
    for (const FXmlNode* Child : Parent->GetChildrenNodes())
    {
      if (Child)
      {
        ++Count;
        Count += CountXmlInstructions(Child);
      }
    }
    return Count;
  }

  RpyInstruction* MakeXmlInstruction(URpyScript* Script, const FXmlNode* Node, FRpyLine* Line)
  {
    FString Type = Node->GetTag();
    if (Type == TEXT("character")) Type = TEXT("defineCharacter");
    if (Type == TEXT("asset")) Type = TEXT("defineAsset");
    const FString A = RequiredValue(Node, TEXT("name"));
    const FString B = RequiredValue(Node, TEXT("value"));
    RpyInstruction* Result = nullptr;
    if (Type == TEXT("label"))
    {
      Result = new LabelInstruction(Script, Line);
      Script->labels.Add(FName(*A), Result);
    }
    else if (Type == TEXT("say"))
    {
      FString Speaker;
      FRpyInstructionProps Props;
      ReadProps(Node, Props);
      Speaker = RequiredValue(Node, TEXT("speaker"));
      if (Speaker.IsEmpty()) Props.GetString(FName(TEXT("speaker")), Speaker);
      Result = new SayInstruction(Script, Line, FName(*Speaker), Node->GetContent());
      Result->Props = Props;
    }
    else if (Type == TEXT("scene") || Type == TEXT("show") || Type == TEXT("hide"))
    {
      FRpySceneOptions Options;
      FRpyInstructionProps Props;
      ReadProps(Node, Props);
      Props.GetString(FName(TEXT("attributes")), Options.attribute);
      if (Type == TEXT("scene")) Result = new SceneInstruction(Script, Line, FName(*RequiredValue(Node, TEXT("asset"))), Options);
      else if (Type == TEXT("show")) Result = new ShowInstruction(Script, Line, FName(*RequiredValue(Node, TEXT("asset"))), Options);
      else Result = new HideInstruction(Script, Line, FName(*RequiredValue(Node, TEXT("tag"))), Options);
      FString TransitionName;
      if (Props.GetString(FName(TEXT("transition")), TransitionName))
      {
        const ERPYTransitionType Transition = ParseTransitionType(TransitionName);
        if (Transition != ERPYTransitionType::NONE) { Options.with = Transition; }
        else
        {
          const FName TransitionKey(*TransitionName);
          if (!Script->transitions.Contains(TransitionKey)) Script->AddDefaultTransition(TransitionKey, TransitionName);
          if (const FRpyTransition* AssetTransition = Script->transitions.Find(TransitionKey))
          {
            Options.with = AssetTransition->type;
            Options.transition = *AssetTransition;
          }
        }
      }
      Props.GetFloat(FName(TEXT("duration")), Options.transitionTime);
      Props.GetInteger(FName(TEXT("zOrder")), Options.zorder);
      FString Layer;
      if (Props.GetString(FName(TEXT("layer")), Layer)) Options.layer = FName(*Layer);
      FString Position;
      if (Props.GetString(FName(TEXT("position")), Position))
      {
        if (Position.StartsWith(TEXT("(")))
        {
          Options.positionType = EPositionType::VECTOR;
          TArray<FString> Coordinates;
          Position.Mid(1, Position.Len() - 2).ParseIntoArray(Coordinates, TEXT(","), true);
          if (Coordinates.Num() > 0) Options.position.X = FCString::Atof(*Coordinates[0]);
          if (Coordinates.Num() > 1) Options.position.Y = FCString::Atof(*Coordinates[1]);
          if (Coordinates.Num() > 2) Options.position.Z = FCString::Atof(*Coordinates[2]);
        }
        else { Options.positionType = EPositionType::NAME; Options.positionName = FName(*Position); }
      }
      Result->Props = PropsFromSceneOptions(Options);
    }
    else if (Type == TEXT("pause")) Result = new PauseInstruction(Script, Line, FCString::Atof(*RequiredValue(Node, TEXT("duration"))));
    else if (Type == TEXT("menu")) Result = new MenuInstruction(Script, Line);
    else if (Type == TEXT("choice")) Result = new ChoiceInstruction(Script, Line, RequiredValue(Node, TEXT("text")));
    else if (Type == TEXT("jump")) Result = new JumpInstruction(Script, Line, FName(*RequiredValue(Node, TEXT("label"))));
    else if (Type == TEXT("call")) Result = new CallInstruction(Script, Line, FName(*RequiredValue(Node, TEXT("label"))));
    else if (Type == TEXT("return")) Result = new ReturnInstruction(Script, Line);
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
      if (Props.GetFloat(FName(TEXT("volume")), Number)) AudioOptions.volume = Number;
      if (Props.GetFloat(FName(TEXT("fadeIn")), Number)) AudioOptions.fadeIn = Number;
      if (Props.GetFloat(FName(TEXT("fadeOut")), Number)) AudioOptions.fadeOut = Number;
      if (!Script->audios.Contains(AssetName) && !Script->AddDefaultAudio(AssetName, AssetPath)) return nullptr;
      if (Type == TEXT("queue")) Result = new QueueInstruction(Script, Line, Channel, AssetName, AudioOptions);
      else Result = new PlayInstruction(Script, Line, Channel, AssetName, AudioOptions);
      Result->Props = Props;
    }
    else if (Type == TEXT("stop")) Result = new StopInstruction(Script, Line, FName(*RequiredValue(Node, TEXT("channel"))), FRpyAudioOptions());
    else if (Type == TEXT("if"))
    {
      FString Condition = RequiredValue(Node, TEXT("condition"));
      const bool bReverse = Condition.StartsWith(TEXT("!"));
      if (bReverse) Condition.RightChopInline(1);
      Result = new IfBoolInstruction(Script, Line, FName(*Condition), bReverse);
    }
    else if (Type == TEXT("set"))
    {
      const FName Name(*A);
      const FString Value = B;
      const bool bHasExpression = HasRequiredField(Node, TEXT("expression"));
      if (bHasExpression) Result = new RpyUnsupportedInstruction(Script, Line);
      else if (RequiredFieldType(Node, TEXT("value")) == TEXT("bool"))
      {
        const bool bValue = Value == TEXT("true");
        Result = new AssignInstruction<bool>(Script, Line, Name, bValue);
      }
      else if (RequiredFieldType(Node, TEXT("value")) == TEXT("integer")) Result = new AssignInstruction<int>(Script, Line, Name, FCString::Atoi(*Value));
      else if (RequiredFieldType(Node, TEXT("value")) == TEXT("float")) Result = new AssignInstruction<float>(Script, Line, Name, FCString::Atof(*Value));
      else Result = new AssignInstruction<FString>(Script, Line, Name, Value);
      FRpyInstructionProps ScopeProps;
      ReadProps(Node, ScopeProps);
      FString Scope;
      if (ScopeProps.GetString(FName(TEXT("scope")), Scope) && Scope == TEXT("default"))
      {
        const FString ValueType = RequiredFieldType(Node, TEXT("value"));
        if (ValueType == TEXT("bool")) Script->compileData.setValue(Name, Value == TEXT("true"));
        else if (ValueType == TEXT("integer")) Script->compileData.setValue(Name, FCString::Atoi(*Value));
        else if (ValueType == TEXT("float")) Script->compileData.setValue(Name, FCString::Atof(*Value));
        else if (ValueType == TEXT("string")) Script->compileData.setValue(Name, Value);
      }
    }
    else if (Type == TEXT("else")) Result = new ElseInstruction(Script, Line);
    else if (Type == TEXT("init")) Result = new RpyInstruction(Script, Line);
    else if (Type == TEXT("defineCharacter"))
    {
      FRpyCharacter Character;
      Character.name = FName(*RequiredValue(Node, TEXT("displayName")));
      Character.image = FName(*RequiredValue(Node, TEXT("name")));
      FRpyInstructionProps CharacterProps;
      ReadProps(Node, CharacterProps);
      FString CharacterValue;
      if (CharacterProps.GetString(FName(TEXT("image")), CharacterValue)) Character.image = FName(*CharacterValue);
      if (CharacterProps.GetString(FName(TEXT("voice")), CharacterValue)) Character.voice = FName(*CharacterValue);
      if (CharacterProps.GetString(FName(TEXT("color")), CharacterValue)) Character.color = FColor::FromHex(CharacterValue);
      Result = new RpyInstruction(Script, Line);
      Script->characters.Add(Character.image, Character);
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
      if (Kind == TEXT("image")) Script->AddDefaultImage(AssetName, AssetPath);
      else if (Kind == TEXT("sound") || Kind == TEXT("music") || Kind == TEXT("audio")) Script->AddDefaultAudio(AssetName, AssetPath);
      else if (Kind == TEXT("transition")) Script->AddDefaultTransition(AssetName, AssetPath);
      Result = new RpyInstruction(Script, Line);
      Result->Props = Props;
    }
    else if (Type == TEXT("raw")) Result = new RpyUnsupportedInstruction(Script, Line);

    if (!Result && FRpyInstructionRegistry::IsRegistered(FName(*Type)))
    {
      TMap<FName, FRpyInstructionProp> RequiredFields;
      for (const FXmlAttribute& Attr : Node->GetAttributes())
      {
        FRpyInstructionProp Value;
        Value.Name = FName(*Attr.GetTag());
        Value.Type = TEXT("string");
        Value.Value = Attr.GetValue();
        RequiredFields.Add(Value.Name, Value);
      }
      FRpyInstructionProps CustomProps;
      ReadProps(Node, CustomProps);
      Result = FRpyInstructionRegistry::Create(FName(*Type), Script, Line, RequiredFields, CustomProps);
    }

    if (Result)
    {
      Result->InstructionId = FName(*FString::Printf(TEXT("Instruction_%d"), Script->instructions.Num()));
      MergeProps(Node, Result->Props);
    }
    return Result;
  }
}

bool URpyScript::Parse()
{
  for (auto instruction : instructions)
  {
    delete instruction;
  }
  instructions.Empty();
  labels.Empty();
  instructionsById.Empty();
  if (!scriptXml.IsEmpty())
  {
    FXmlFile XmlFile;
    if (!XmlFile.LoadFile(scriptXml, EConstructMethod::ConstructFromBuffer))
    {
      UE_LOG(LogTemp, Error, TEXT("Invalid visual novel XML: %s"), *XmlFile.GetLastError());
      return false;
    }
    const FXmlNode* Root = XmlFile.GetRootNode();
    if (!Root || Root->GetTag() != TEXT("visualNovel") || Root->GetAttribute(TEXT("version")) != TEXT("1"))
    {
      UE_LOG(LogTemp, Error, TEXT("Expected <visualNovel version=\"1\"> root"));
      return false;
    }
    const int32 InstructionCount = CountXmlInstructions(Root);
    instructions.Reserve(InstructionCount);
    TFunction<bool(const FXmlNode*, uint8)> ReadInstructionChildren;
    ReadInstructionChildren = [&](const FXmlNode* Parent, uint8 Depth) -> bool
    {
      bool bSuccess = true;
      for (const FXmlNode* Node : Parent->GetChildrenNodes())
      {
        if (!Node) continue;
        if (!ValidateXmlInstruction(Node))
        {
          bSuccess = false;
          continue;
        }
        FRpyLine Line;
        Line.tabs = Depth;
        FRpyInstructionProps SourceProps;
        ReadProps(Node, SourceProps);
        Line.lineNumber = instructions.Num() + 1;
        Line.line = DescribeXmlInstruction(Node);
        FRpyInstructionProps DiagnosticProps;
        ReadProps(Node, DiagnosticProps);
        DiagnosticProps.GetString(FName(TEXT("legacySource")), Line.line);
        RpyInstruction* Instruction = MakeXmlInstruction(this, Node, &Line);
        if (!Instruction)
        {
          UE_LOG(LogTemp, Error, TEXT("Unsupported XML instruction '%s' at source line %d"), *Node->GetTag(), Line.lineNumber);
          bSuccess = false;
          continue;
        }
        instructions.Add(Instruction);
        instructionsById.Add(Instruction->InstructionId, Instruction);
        if (!ReadInstructionChildren(Node, Depth + 1)) bSuccess = false;
      }
      return bSuccess;
    };
    return ReadInstructionChildren(Root, 0);
  }
  return false;
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
    int currentTabs = current->rpyLine.tabs;
    if (last)
    {
      int lastTabs = last->rpyLine.tabs;
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
        UE_LOG(LogTemp, Error, TEXT("unvalid tabs transition at line :%d"), current->rpyLine.lineNumber);
        return false;
      }
    }
    else if (currentTabs == 0)
    {
      stack.Add(current);
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("unvalid tabs transition at line :%d"), current->rpyLine.lineNumber);
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
      UE_LOG(LogTemp, Error, TEXT("failed to compile line %d : %s"), instruction->rpyLine.lineNumber, (*instruction->rpyLine.line));
      return false;
    }
  }
  return true;
}

FRpyInstructionSerialization URpyScript::SerializeInstruction(RpyInstruction *instruction)
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
RpyInstruction *URpyScript::DeserializeInstruction(const FRpyInstructionSerialization &data)
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
    if (RpyInstruction** ById = Script->instructionsById.Find(data.instructionId)) return *ById;
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
