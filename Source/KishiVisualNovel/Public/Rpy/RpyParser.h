#pragma once

#include "CoreMinimal.h"
#include <regex>
#include <string>

#include "Rpy/RpyScript.h"

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
		if (param == "")
			return param;
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
std::string RpyParser::reg_bool = "(True|False|None)";
std::string RpyParser::reg_integer = "(\\d*)";
std::string RpyParser::reg_float = "([+-]?(?:\\d*\\.)?\\d+)";
std::string RpyParser::reg_ufloat = "((?:\\d*\\.)?\\d+)";
std::string RpyParser::reg_ufloatUnit = "(0(?:\\.\\d+)?|1(?:\\.0+)?)";

/*
((?:'[^']*')|(?:"[^"]*")|(?:`[^`]*`))
matches a string that is enclosed in either single quotes, double quotes, or backticks, while also capturing the string itself. For example, the following strings would match this regular expression:
		'Hello, world!'
		"Hello, world!"
		`Hello, world!`
*/
std::string RpyParser::reg_string = "((?:'[^']*')|(?:\"[^\"]*\")|(?:`[^`]*`))";

std::string RpyParser::reg_keyword = "(at|call|elif|else|expression|hide|if|image|init|jump|label|menu|onlayer|pass|python|return|scene|set|show|with|while)";
/*
([a-zA-Z_]\w*)
matches any valid identifier that starts with a letter or underscore, followed by any number of letters, digits, or underscores. For example, the following identifiers would match this regular expression:

		variable_name
		ClassName
		_private_member
*/
std::string RpyParser::reg_name = "([a-zA-Z_]\\w*)";

/*
((?:(?!(?:at |with ))\w+)(?: (?!(?:at |with ))\w+)*)
matches one or more words separated by a single space, where each word does not begin with the phrases "at" or "with"
*/
std::string RpyParser::reg_multi_name = "((?:(?!(?:at |with ))\\w+)(?: (?!(?:at |with ))\\w+)*)";
/*
([/\w\.]+)
matches any file path or file name that contains only letters, digits, periods, underscores, and forward slashes. For example, the following file paths or file names would match this regular expression:

		/home/user/Documents/my_file.txt
		C:\Users\user\Desktop\my_file.docx
		script.js
*/
std::string RpyParser::reg_path = "([/\\w\\.]+)";
/*
\[(|(?:"[\/\w\.]+"(?:, "[\/\w\.]+")*?))\]
matches a string that is enclosed in square brackets, where the string may be empty or may contain one or more comma-separated quoted strings. For example, the following strings would match this regular expression:

		[]
		[ "file.txt" ]
		[ "file.txt", "dir/file2.txt" ]
*/
std::string RpyParser::reg_array_path = "\\[(|(?:\"[/\\w\\.]+\"(?:, \"[/\\w\\.]+\")*?))\\]";
