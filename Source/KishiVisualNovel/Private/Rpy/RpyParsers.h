// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <regex>

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"
#include <string>

#include "Utils/MapUtils.h"

#include "./Parsers/AudioParser.h"
#include "./Parsers/CallParser.h"
#include "./Parsers/CharacterSayParser.h"
#include "./Parsers/ChoiceParser.h"
#include "./Parsers/DefineParser.h"
#include "./Parsers/ElseParser.h"
#include "./Parsers/HideParser.h"
#include "./Parsers/IfBoolParser.h"
#include "./Parsers/InitParser.h"
#include "./Parsers/JumpParser.h"
#include "./Parsers/LabelParser.h"
#include "./Parsers/MenuParser.h"
#include "./Parsers/PauseParser.h"
#include "./Parsers/ReturnParser.h"
#include "./Parsers/SayParser.h"
#include "./Parsers/SceneParser.h"
#include "./Parsers/ShowParser.h"
#include "./Parsers/StopAudioParser.h"
#include "./Parsers/VoiceParser.h"

InitParser _InitParser;
DefineCharacterParser _DefineCharacterParser;
DefineMediaParser _DefineMediaParser;
DefineStringParser _DefineStringParser;
DefineBoolParser _DefineBoolParser;
StopAudioParser _StopAudioParser;
VoiceParser _VoiceParser;
AudioParser _AudioParser;
LabelParser _LabelParser;
JumpParser _JumpParser;
PauseParser _PauseParser;
ReturnParser _ReturnParser;
CallParser _CallParser;
MenuParser _MenuParser;
ChoiceParser _ChoiceParser;
SayParser _SayParser;
IfBoolParser _IfBoolParser;
ElseParser _ElseParser;
CharacterSayParser _CharacterSayParser;
ShowParser _ShowParser;
SceneParser _SceneParser;
HideParser _HideParser;
TArray<RpyParser *> RpyParser::parsers = {
		&_InitParser,
		&_DefineCharacterParser,
		&_DefineMediaParser,
		&_DefineStringParser,
		&_DefineBoolParser,
		&_StopAudioParser,
		&_VoiceParser,
		&_AudioParser,
		&_LabelParser,
		&_JumpParser,
		&_PauseParser,
		&_ReturnParser,
		&_CallParser,
		&_MenuParser,
		&_ChoiceParser,
		&_SayParser,
		&_IfBoolParser,
		&_ElseParser,
		&_CharacterSayParser,
		&_ShowParser,
		&_SceneParser,
		&_HideParser,
};