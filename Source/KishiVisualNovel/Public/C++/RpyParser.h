#pragma once

#include "CoreMinimal.h"
#include <regex>
#include <string>

#include "DataAssets/RpyScript.h"

class RpyParser
{
public:
    int paramsNum;
    FString query;
    std::regex reg_query;
    RpyParser(int paramsNum, std::string query) :paramsNum(paramsNum), query(query.c_str()), reg_query(query) {};
    virtual ~RpyParser() = default;
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) = 0;

    //primitive
    static std::string reg_integer;
    static std::string reg_float;
    static std::string reg_bool;
    //rpy
    static std::string reg_keyword;
    static std::string reg_name;
    static std::string reg_image_name;
    static std::string reg_string;
    //parser
    static std::string reg_label;
    static std::string reg_define_char;
    //
    static int GetInteger(FString param) {
        int32 value = FCString::Atoi(*param);
        return value;
    }
    static float GetFloat(FString param) {
        float value = FCString::Atof(*param);
        return value;
    }
    static bool GetBool(FString param) {
        return param == "True";
    }

    static FString GetString(FString param) {
        return param.Mid(1, param.Len() - 2);
    }
    static TArray<FName> GetNames(FString param) {
        FString left, right;
        TArray<FName> names;
        TArray<FString> strings;
        static const TCHAR* spaces[] =
        {
            TEXT(" "),
            TEXT("\n"),
            TEXT("\t"),
        };
        param.ParseIntoArray(strings, spaces, 3, true);
        for (auto string : strings) {
            names.Add(FName(*string));
        }
        return names;
    }

};
std::string RpyParser::reg_integer = "(\\d*)";
std::string RpyParser::reg_float = "([+-]?(?:\\d*\\.)?\\d+)";
std::string RpyParser::reg_bool = "(True|False|None)";


std::string RpyParser::reg_keyword = "(at|call|elif|else|expression|hide|if|image|init|jump|label|menu|onlayer|pass|python|return|scene|set|show|with|while)";
std::string RpyParser::reg_name = "([a-zA-Z_]\\w*)";
std::string RpyParser::reg_image_name = "((?:(?!(?:at |with ))\\w+)(?: (?!(?:at |with ))\\w+)*)";
std::string RpyParser::reg_string = "((?:'[^']*')|(?:\"[^\"]*\")|(?:`[^`]*`))";

std::string RpyParser::reg_label = "^label " + RpyParser::reg_name + ":$";
std::string RpyParser::reg_define_char = "^\\$ " + RpyParser::reg_name + " = Character\\('(\\w+)'(?:, image='(\\w+)')?\\)$";