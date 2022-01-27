
#include "C++/RpyParsers.h"
#include "C++/RpyInstructions.h"

#include <iostream>
#include <regex>
#include <string>
#include <stdexcept>

using namespace std;
FName lastSayName = FName("$lastSayName");
RpyInstruction* SayParser::GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
    bool literal = params[0].StartsWith("\"") && params[0].EndsWith("\"");
    if (literal)
        params[0] = params[0].Mid(1, params[0].Len() - 2);
    FName name = FName(*params[0]);
    if (params[0].Len() > 0) {
        if (!literal) {
            if (script->compileData.names.Contains(name)) {
                name = script->compileData.names[name];
            }
            else {
                return nullptr;
            }
        }
    }
    else if (script->compileData.names.Contains(lastSayName)) {
        name = script->compileData.names[lastSayName];
    }
    else {
        return nullptr;
    }
    script->compileData.names.Add(lastSayName, name);
    return new SayInstruction(script, rpyLine, name, params[1]);
};
