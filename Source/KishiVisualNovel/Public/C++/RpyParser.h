#pragma once

#include "CoreMinimal.h"
#include <regex>
#include <string>

#include "DataAssets/RpyScript.h"

struct RpyParser
{
	int paramsNum;
	FString query;
	FString parserName;
	std::regex reg_query;
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
	static std::string reg_bool;
	static std::string reg_string;
	// rpy
	static std::string reg_keyword;
	static std::string reg_name;
	static std::string reg_image_name;
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
	static bool GetBool(FString param)
	{
		return param == "True";
	}
	static TArray<FString> GetArrayPath(FString param)
	{
		FString left, right;
		TArray<FString> array;
		TArray<FString> strings;
		static const TCHAR *delimiters[] = {
				TEXT(", "),
		};
		param.ParseIntoArray(strings, delimiters, 1, true);
		for (auto string : strings)
		{
			array.Add(string.Mid(1, string.Len() - 2));
		}
		return array;
	}

	static FString GetString(FString param)
	{
		return param.Mid(1, param.Len() - 2);
	}
	static TArray<FName> GetNames(FString param)
	{
		FString left, right;
		TArray<FName> names;
		TArray<FString> strings;
		static const TCHAR *delimiters[] =
				{
						TEXT(" "),
						TEXT("\n"),
						TEXT("\t"),
				};
		param.ParseIntoArray(strings, delimiters, 3, true);
		for (auto string : strings)
		{
			names.Add(FName(*string));
		}
		return names;
	}
};
std::string RpyParser::reg_integer = "(\\d*)";
std::string RpyParser::reg_float = "([+-]?(?:\\d*\\.)?\\d+)";
std::string RpyParser::reg_ufloat = "((?:\\d*\\.)?\\d+)";
std::string RpyParser::reg_ufloatUnit = "(0(?:\\.\\d+)?|1(?:\\.0+)?)";
std::string RpyParser::reg_string = "((?:'[^']*')|(?:\"[^\"]*\")|(?:`[^`]*`))";
// std::string RpyParser::reg_string = "((?:'(?:[^'\\\\]|\\\\.)*')|(?:\"(?:[^\"\\\\]|\\\\.)*\")|(?:`(?:[^`\\\\]|\\\\.)*`))";

std::string RpyParser::reg_bool = "(True|False|None)";

std::string RpyParser::reg_keyword = "(at|call|elif|else|expression|hide|if|image|init|jump|label|menu|onlayer|pass|python|return|scene|set|show|with|while)";
std::string RpyParser::reg_name = "([a-zA-Z_]\\w*)";
std::string RpyParser::reg_image_name = "((?:(?!(?:at |with ))\\w+)(?: (?!(?:at |with ))\\w+)*)";
std::string RpyParser::reg_path = "([/\\w\\.]+)";
//\[(|(?:"[\/\w\.]+"(?:, "[\/\w\.]+")*?))\]
std::string RpyParser::reg_array_path = "\\[(|(?:\"[/\\w\\.]+\"(?:, \"[/\\w\\.]+\")*?))\\]";
