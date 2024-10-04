#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include <regex>
#include <string>

#include "Rpy/RpyScript.h"

TMap<FString, FVector> const stringToFVector = {
		{"left", FVector(0, 0.5, 0)},
		{"right", FVector(1, 0.5, 0)},
		{"center", FVector(0.5, 0.5, 0)},
		{"top", FVector(0.5, 1.0, 0)},
		{"bottom", FVector(0.5, 0, 0)},
};
struct RpyParser
{
	int paramsNum;
	FString query;
	FString parserName;
	std::regex reg_query;

	static TArray<RpyParser *> parsers;
	RpyParser(int paramsNum, std::string query, FString parserName = "") : paramsNum(paramsNum), query(query.c_str()), parserName(parserName)
	{
		UE_LOG(LogTemp, Warning, TEXT("RpyParser.query[%s]:%s"), *this->parserName, *this->query);
		reg_query = query;
	};

	virtual ~RpyParser() = default;
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params) = 0;

	// primitive
	static std::string reg_integer;
	static std::string reg_float;
	static std::string reg_ufloat;
	static std::string reg_ufloatUnit;
	static std::string reg_vector;
	static std::string reg_bool;
	static std::string reg_string;
	static std::string reg_string_simple;
	// rpy
	static std::string reg_comment;
	static std::string reg_keyword;
	static std::string reg_var_name;
	static std::string reg_args_map;
	static std::string reg_rpy_options;
	static std::string reg_rpy_scene_options;

	static std::string reg_name;
	static std::string reg_multi_name;
	static std::string reg_path;
	static std::string reg_array_path;
	//
	static int GetInteger(FString param)
	{
		int32 value = FCString::Atoi(*param);
		return value;
	}
	static float GetFloat(FString param)
	{
		float value = FCString::Atof(*param);
		return value;
	}
	static FVector GetVector(FString param)
	{
		FVector vector;
		TArray<FString> strings;
		static const TCHAR *delimiters[] =
				{
						TEXT(","),
				};
		param.TrimStartAndEnd().ParseIntoArray(strings, delimiters, 1, true);
		if (strings.Num() > 0)
		{
			vector.X = GetFloat(strings[0]);
		}
		if (strings.Num() > 1)
		{
			vector.Y = GetFloat(strings[1]);
		}
		if (strings.Num() > 2)
		{
			vector.Z = GetFloat(strings[2]);
		}
		return vector;
	}
	static bool GetBool(FString param)
	{
		return param == "True";
	}
	static FString GetPath(FString param)
	{
		return param.TrimStartAndEnd().Mid(1, param.Len() - 2);
	}
	static TArray<FString> GetArrayPath(FString param)
	{
		TArray<FString> array;
		TArray<FString> strings;
		static const TCHAR *delimiters[] = {
				TEXT(","),
		};
		param.TrimStartAndEnd().ParseIntoArray(strings, delimiters, 1, true);
		for (auto string : strings)
		{
			array.Add(GetPath(string));
		}
		return array;
	}

	static FString GetString(FString param)
	{
		if (param == "")
			return param;
		if (param.StartsWith("\""))
			param = param.Replace(TEXT("\\\""), TEXT("\""));
		else if (param.StartsWith("'"))
			param = param.Replace(TEXT("\\'"), TEXT("'"));
		else if (param.StartsWith("`"))
			param = param.Replace(TEXT("\\`"), TEXT("`"));
		return param.Mid(1, param.Len() - 2);
	}
	static TArray<FName> GetNames(FString param)
	{
		TArray<FName> names;
		TArray<FString> strings;
		static const TCHAR *delimiters[] =
				{
						TEXT(" "),
						TEXT("\n"),
						TEXT("\t"),
				};
		param.TrimStartAndEnd().ParseIntoArray(strings, delimiters, 3, true);
		for (auto string : strings)
		{
			names.Add(FName(*string));
		}
		return names;
	}
	static TMap<FName, FString> GetArgs(FString param)
	{
		TMap<FName, FString> args;
		TArray<FString> strings;
		static const TCHAR *delimiters[] =
				{
						TEXT(","),
				};
		param.TrimStartAndEnd().ParseIntoArray(strings, delimiters, 1, true);
		for (auto string : strings)
		{
			TArray<FString> pair;
			string.TrimStartAndEnd().ParseIntoArray(pair, TEXT("="), true);
			if (pair.Num() == 2)
			{
				args.Add(FName(pair[0]), pair[1]);
			}
		}
		return args;
	}
	static TMap<FName, FString> GetRpyOptions(FString param)
	{
		TMap<FName, FString> options;
		TArray<FString> strings;
		static const TCHAR *delimiters[] =
				{
						TEXT(" "),
				};
		param.TrimStartAndEnd().ParseIntoArray(strings, delimiters, 1, true);
		if (strings.Num() % 2 != 0)
			return options;
		for (auto idx = 0; idx < strings.Num(); idx += 2)
		{
			options.Add(FName(strings[idx]), strings[idx + 1]);
		}
		return options;
	}
	static FRpySceneOptions GetRpySceneOptions(TArray<FString> params, int offset = 0)
	{
		FRpySceneOptions options;
		if ((params.Num() + offset) < 5)
		{
			return options;
		}
		FString with = params[0 + offset];
		FString transitionTime = params[1 + offset];
		FString positionEnum = params[2 + offset];
		FString positionVector = params[3 + offset];
		FString layer = params[4 + offset];
		if (!with.IsEmpty())
		{
			options.with = stringToERPYTransition[with];
			options.transitionTime = GetFloat(transitionTime);
		}
		if (!layer.IsEmpty())
		{
			options.layer = stringToELayer[layer];
		}
		if (!positionEnum.IsEmpty())
		{
			options.position = stringToFVector[positionEnum];
		}
		else if (!positionVector.IsEmpty())
		{
			auto position = GetVector(positionVector);
			options.position.Set(position.X, position.Y, position.Z);
		}
		return options;
	}
};

std::string reg_float_nc = "(?:[+-]?(?:\\d*\\.)?\\d+)";

std::string RpyParser::reg_bool = "(True|False|None)";
std::string RpyParser::reg_integer = "(\\d*)";
std::string RpyParser::reg_float = "([+-]?(?:\\d*\\.)?\\d+)";
std::string RpyParser::reg_ufloat = "((?:\\d*\\.)?\\d+)";
std::string RpyParser::reg_ufloatUnit = "(0(?:\\.\\d+)?|1(?:\\.0+)?)";
std::string RpyParser::reg_vector = "\\((\\s*" + reg_float_nc + "\\s*(?:,\\s*" + reg_float_nc + "\\s*)*)\\)";

/*
match a string containing a series of comma-separated key-value pairs, where each key is a word and each value is a string enclosed in double quotes
		, color="#c8ffc8", voice="ok"
INCLUDE ", " LEFT
*/
std::string RpyParser::reg_args_map = "((?:, \\w+=\".*\")+)";

/*
((?: (?:at |with )\w+)*)
INCLUDE space left
*/
std::string RpyParser::reg_rpy_options = "((?: (?:at|with|onlayer) \\w+)*)";

// 'master' | 'transient' | 'underlay' | 'overlay'
std::string reg_layer_nc = "master|transient|underlay|overlay";
// 'left' | 'right' | 'center' | 'top' | 'bottom' | Vector2D;
std::string reg_position_enum_nc = "left|right|center|top|bottom";
// | 'dissolve'
// | 'fade'
// | 'wipeleft'
// | 'wiperight'
// | 'wipeup'
// | 'wipedown'
// | 'zoom'
// | 'blur'
std::string reg_transition_enum_nc = "dissolve|fade|wipeleft|wiperight|wipeup|wipedown|zoom|blur";

// 2
std::string reg_position = "(?:(" + reg_position_enum_nc + ")|" + RpyParser::reg_vector + ")";

std::string RpyParser::reg_rpy_scene_options = "(?:(?:\\s*with\\s*(" + reg_transition_enum_nc + ")\\s*(" + reg_float_nc + ")?)?(?:\\s*at\\s*" + reg_position + ")?(?:\\s*onlayer\\s*(" + reg_layer_nc + "))?)";

/*
(?:\"(?:[^\"\\\\]|\\\\.)+\")
(?:'(?:[^'\\\\]|\\\\.)+')
(?:`(?:[^`\\\\]|\\\\.)+`)
((?:'[^']*')|(?:"[^"]*")|(?:`[^`]*`))
matches a string that is enclosed in either single quotes, double quotes, or backticks, while also capturing the string itself. For example, the following strings would match this regular expression:
		'Hello, world!'
		"Hello, world!"
		`Hello, world!`
*/
std::string RpyParser::reg_string_simple = "((?:'[^']*')|(?:\"[^\"]*\")|(?:`[^`]*`))";

std::string RpyParser::reg_string = "((?:\"(?:[^\"\\\\]|\\\\.)+\")|(?:'(?:[^'\\\\]|\\\\.)+')|(?:`(?:[^`\\\\]|\\\\.)+`))";

std::string RpyParser::reg_comment = "(?:\\s*#.*)?$";

std::string RpyParser::reg_keyword = "(at|call|elif|else|expression|hide|if|image|init|jump|label|menu|onlayer|pass|play|python|queue|return|scene|set|show|with|while)";
std::string RpyParser::reg_var_name = "((?:(?!(?:at|call|elif|else|expression|hide|if|image|init|jump|label|menu|onlayer|pass|play|python|queue|return|scene|set|show|with|while))\\w+))";
/*
([a-zA-Z_]\w*)
matches any valid identifier that starts with a letter or underscore, followed by any number of letters, digits, or underscores. For example, the following identifiers would match this regular expression:

		variable_name
		ClassName
		_private_member
*/
std::string RpyParser::reg_name = "([a-zA-Z_]\\w*)";

/*
((?:(?!(?:at |with ))\w+)(?:\s+(?:(?!(?:at |with ))\w+))*)
matches one or more words separated by a single space, where each word does not begin with the phrases "at" or "with"
*/
std::string RpyParser::reg_multi_name = "((?:(?!(?:at |with ))\\w+)(?:\\s+(?:(?!(?:at |with ))\\w+))*)";
/*
([/\w\.]+)
matches any file path or file name that contains only letters, digits, periods, underscores, and forward slashes. For example, the following file paths or file names would match this regular expression:

		/home/user/Documents/my_file.txt
		C:\Users\user\Desktop\my_file.docx
		script.js
*/
std::string RpyParser::reg_path = "(\"[/\\w\\.-]+\")";
/*
\[(|(?:"[\/\w\.]+"(?:, "[\/\w\.]+")*?))\]
matches a string that is enclosed in square brackets, where the string may be empty or may contain one or more comma-separated quoted strings. For example, the following strings would match this regular expression:

		[]
		[ "file.txt" ]
		[ "file.txt", "dir/file2.txt" ]
*/
std::string RpyParser::reg_array_path = "\\[(|(?:\"[/\\w\\.-]+\"(?:, \"[/\\w\\.-]+\")*?))\\]";
