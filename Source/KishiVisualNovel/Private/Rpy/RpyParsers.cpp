#include "Rpy/RpyParser.h"
#include "Rpy/RpyParsers.h"

InitParser _InitParser;
DefineCharacterParser _DefineCharacterParser;
DefineMediaParser _DefineMediaParser;
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
    &_HideParser};