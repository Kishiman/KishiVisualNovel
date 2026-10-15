#pragma once

#include "Lib/ArrayLib.h"
#include "Rpy/RpyInstruction.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Interfaces/ScriptInterpreterManagers/RpyAudioManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyChoiceManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpySceneManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyShowManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyStatementManager.h"
#include "Rpy/RpySession.h"
#include "Utils/StringUtils.h"

struct IfInstruction;
struct ElseInstruction;
struct MenuInstruction;
struct ChoiceInstruction;

static FRpyImage *ResolveImageForCompile(URpyScript *Script, FName Name, const FRpySceneOptions &Options)
{
  if (!Script || Name == FName(TEXT("_")))
    return nullptr;
  FName AssetKey = Name;
  if (!Options.attribute.IsEmpty())
    AssetKey = FName(*(Name.ToString() + TEXT(" ") + Options.attribute));
  if (FRpyImage *Image = Script->images.Find(Name))
    return Image;
  if (AssetKey != Name)
  {
    if (FRpyImage *Image = Script->images.Find(AssetKey))
      return Image;
  }
  return Script->FindOrAddDefaultImage(Name, Options.attribute);
}

static FRpyInstructionProps PropsFromSceneOptions(const FRpySceneOptions &Options)
{
  FRpyInstructionProps Props;
  Props.SetInteger(FName(TEXT("transitionType")), static_cast<int32>(Options.with));
  Props.SetInteger(FName(TEXT("transitionDirection")), static_cast<int32>(Options.direction));
  Props.SetInteger(FName(TEXT("transitionAssetType")), static_cast<int32>(Options.transition.type));
  Props.SetInteger(FName(TEXT("positionType")), static_cast<int32>(Options.positionType));
  Props.SetString(FName(TEXT("transitionName")), Options.transition.name.ToString());
  Props.SetString(FName(TEXT("transitionPath")), Options.transition.path);
  Props.SetString(FName(TEXT("layer")), Options.layer.ToString());
  Props.SetString(FName(TEXT("positionName")), Options.positionName.ToString());
  Props.SetString(FName(TEXT("attributes")), Options.attribute);
  Props.SetVector(FName(TEXT("position")), Options.position);
  Props.SetFloat(FName(TEXT("duration")), Options.transitionTime);
  Props.SetInteger(FName(TEXT("zOrder")), Options.zorder);
  return Props;
}

static FRpySceneOptions SceneOptionsFromProps(const FRpyInstructionProps &Props, const URpyScript *Script)
{
  FRpySceneOptions Options;
  int32 Number = 0;
  Props.GetInteger(FName(TEXT("transitionType")), Number);
  Options.with = static_cast<ERPYTransitionType>(Number);
  Props.GetInteger(FName(TEXT("transitionDirection")), Number);
  Options.direction = static_cast<ERPYTransitionDirection>(Number);
  Number = 0;
  Props.GetInteger(FName(TEXT("transitionAssetType")), Number);
  Options.transition.type = static_cast<ERPYTransitionType>(Number);
  Props.GetInteger(FName(TEXT("positionType")), Number);
  Options.positionType = static_cast<EPositionType>(Number);
  FString Value;
  if (Props.GetString(FName(TEXT("transitionName")), Value))
    Options.transition.name = FName(*Value);
  if (Props.GetString(FName(TEXT("transitionPath")), Value))
    Options.transition.path = Value;
  if (Script)
  {
    if (const FRpyTransition *Transition = Script->transitions.Find(Options.transition.name))
      Options.transition = *Transition;
  }
  if (Props.GetString(FName(TEXT("layer")), Value))
    Options.layer = FName(*Value);
  if (Props.GetString(FName(TEXT("positionName")), Value))
    Options.positionName = FName(*Value);
  Props.GetString(FName(TEXT("attributes")), Options.attribute);
  if (!Props.GetVector(FName(TEXT("position")), Options.position) && Props.GetString(FName(TEXT("position")), Value))
  {
    Value.ReplaceInline(TEXT("("), TEXT(""));
    Value.ReplaceInline(TEXT(")"), TEXT(""));
    TArray<FString> Parts;
    Value.ParseIntoArray(Parts, TEXT(","), true);
    if (Parts.Num() > 0)
      Options.position.X = FCString::Atof(*Parts[0]);
    if (Parts.Num() > 1)
      Options.position.Y = FCString::Atof(*Parts[1]);
    if (Parts.Num() > 2)
      Options.position.Z = FCString::Atof(*Parts[2]);
  }
  Props.GetFloat(FName(TEXT("duration")), Options.transitionTime);
  if (Options.with != ERPYTransitionType::NONE && Options.transitionTime <= 0.0f)
  {
    Options.transitionTime = 1.0f;
  }
  Props.GetInteger(FName(TEXT("zOrder")), Options.zorder);
  return Options;
}

static FRpyInstructionProps PropsFromAudioOptions(const FRpyAudioOptions &Options)
{
  FRpyInstructionProps Props;
  Props.SetFloat(FName(TEXT("volume")), Options.volume);
  Props.SetFloat(FName(TEXT("fadeIn")), Options.fadeIn);
  Props.SetFloat(FName(TEXT("fadeOut")), Options.fadeOut);
  Props.SetInteger(FName(TEXT("loop")), static_cast<int32>(Options.loop));
  Props.SetInteger(FName(TEXT("tight")), static_cast<int32>(Options.tight));
  Props.SetBool(FName(TEXT("synchroStart")), Options.synchroStart);
  Props.SetBool(FName(TEXT("ifChanged")), Options.if_changed);
  return Props;
}

static FRpyAudioOptions AudioOptionsFromProps(const FRpyInstructionProps &Props)
{
  FRpyAudioOptions Options;
  Props.GetFloat(FName(TEXT("volume")), Options.volume);
  Props.GetFloat(FName(TEXT("fadeIn")), Options.fadeIn);
  Props.GetFloat(FName(TEXT("fadeOut")), Options.fadeOut);
  int32 Number = 0;
  bool bFlag = false;
  if (Props.GetBool(FName(TEXT("loop")), bFlag))
    Options.loop = bFlag;
  else
  {
    Props.GetInteger(FName(TEXT("loop")), Number);
    Options.loop = Number != 0;
  }
  if (Props.GetBool(FName(TEXT("tight")), bFlag))
    Options.tight = bFlag;
  else
  {
    Props.GetInteger(FName(TEXT("tight")), Number);
    Options.tight = Number != 0;
  }
  Props.GetBool(FName(TEXT("synchroStart")), Options.synchroStart);
  if (!Props.GetBool(FName(TEXT("ifChanged")), Options.if_changed))
    Props.GetBool(FName(TEXT("if_changed")), Options.if_changed);
  return Options;
}
