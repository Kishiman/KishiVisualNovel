
#include "C++/RpyParsers.h"
#include "C++/RpyInstructions.h"

#include <iostream>
#include <regex>
#include <string>
#include <stdexcept>

using namespace std;
FString keywords[] = {
    "at",
    "call",
    "hide",
    "if",
    "image",
    "init",
    "jump",
    "menu",
    "onlayer",
    "python",
    "return",
    "scene",
    "set",
    "show",
    "with",
    "while",
};

int example()
{
    string lines[] = {
        //"#some comment",
        "label start:",
        "\"Sylvie\" \"Hi there! how was class?\"",
        "\"I'll ask her...!\"",
        "define m = Character('Me', color=\"#c8c8ff\")",
        "define s = Character(_(\"Sylvie\"), color=\"#c8ffc8\")",
        "scene bg meadow",
        "show sylvie green smile",
        "hide sylvie",
        "image logo = \"renpy logo.png\"",
        "image eileen happy = \"eileen_happy_blue_dress.png\"",
        "play music \"audio/illurock.ogg\"",
        "play music \"audio/illurock.ogg\" fadeout 1.0 fadein 1.0",
        "pause 3.0",
        "return",
        "\"It's a videogame.\":",
        "jump game",
        "menu:",
        "default book = False",
        "$ book = True",
        "$ book = 1",
        "$ book = 1.0",
        "$ book = \"Hi\"",
        "if book:",
        "else:",
        "with fade",
        "\"{b}Good Ending{/b}.\"",
        "show sylvie green at right",
    };
    regex regs[] = {
        // regex("^#(.*)$"),
        regex("^label (\\w+):$"),
        regex("^(?:\"(.+)\" )?\"(.*)\"$"),
        regex("^define (\\w+) = Character\\(('\\w+'|_\\(\"\\w+\"\\))(?:, "
              "color=\"#([0-f]{6})\")?(?:, who_color=\"#([0-f]{6})\")?\\)$"),
        regex("^scene(( \\w+ \\w+)+)$"),
        regex("^show(( (?!at)\\w+)+)(?: at (\\w+))?$"),
        regex("^hide (\\w+)$"),
        regex("^image(\\s\\w+)+ = \"([\\w\\.\\s]+)\"$"),
        regex("^(play|stop|queue) (music|sound) \"(.*)\"(?: fadeout "
              "([0-9\\.]+))?(?: fadein ([0-9\\.]+))?$"),
        regex("^pause(?: ([0-9\\.]+))?$"),
        regex("^return$"),
        regex("^with (fade|dissolve|None)$"),
        regex("^\"(.*)\":$"),
        regex("^jump (\\w+)$"),
        regex("^menu:$"),
        regex("^default (\\w+) = (False|True)$"),
        regex("^\\$ (\\w+) = ((True|False)|([0-9]+)|([0-9\\.]+)|(\".*\"))$"),
        regex("^if (\\w+):$"),
        regex("^(else if|elif) (\\w+):$"),
        regex("^else:$"),
    };
    for (string &line : lines)
    {
        bool matched = false;
        for (auto &reg : regs)
        {
            std::smatch m;
            matched = std::regex_match(line, m, reg);
            if (matched)
            {
                int counter = -1;
                for (auto &param : m)
                {
                    ++counter;
                    if (counter)
                    {
                        cout << param;
                        cout << "|";
                    }
                    else
                    {
                        cout << param << "\n=>|";
                    }
                }
                cout << "\n";
                break;
            }
        }
        if (!matched)
            cout << "!!!not matched :" << line << "\n\n";
    }
    return 0;
};
//"label start:"
LabelParser::LabelParser()
{
    this->query = "^label (\\w+):$";
};
RpyInstruction *LabelParser::GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
{
    RpyInstruction *label = new BlankInstruction(script, rpyLine);
    script->labels.Add(FName(*params[0]), label);
    return label;
};
//"\"Sylvie\" \"Hi there! how was class?\""
SayParser::SayParser()
{
    this->query = "^(?:\"(.+)\" )?\"(.*)\"$";
};
FName lastSayName = FName("$lastSayName");
RpyInstruction *SayParser::GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
{
    FName name = FName(*params[0]);
    if (params[0].Len() > 0)
    {
        script->compileData.names.Add(lastSayName, name);
    }
    else if (script->compileData.names.Contains(lastSayName))
    {
        name = script->compileData.names[lastSayName];
    }
    else
    {
        return nullptr;
    }
    return new SayInstruction(script, rpyLine, name, params[1]);
};

//"scene bg meadow"
SceneParser::SceneParser()
{
    this->query = "^(?:\"(.+)\" )?\"(.*)\"$";
};
RpyInstruction *SceneParser::GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
{
    TMap<FName, FString> _params;
    FString key;
    FString value;
    FString target = params[0];
    int from = 1;
    int to = target.Len() - 1;
    while (to > -1)
    {
        to = target.Find(" ", ESearchCase::IgnoreCase, ESearchDir::FromStart, from);
        key = target.Mid(from, to - from);
        from = to + 1;
        to = target.Find(" ", ESearchCase::IgnoreCase, ESearchDir::FromStart, from);
        if (to > -1)
        {
            value = target.Mid(from, to - from);
            from = to + 1;
            _params.Add(FName(*key), value);
        }
        else
        {
            value = target.Mid(from, target.Len() - from);
            _params.Add(FName(*key), value);
        }
    };
    return new SceneInstruction(script, rpyLine, _params);
};
