
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
        "#some comment",
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
        regex("^#(.*)$"),
        regex("^label (\\w+):$"),
        regex("^\"(.+)\" \"(.*)\"$"),
        regex("^\"(.*)\"$"),
        regex("^define (\\w+) = Character\\(('\\w+'|_\\(\"\\w+\"\\))(?:, "
              "color=\"#([0-f]{6})\")?(?:, who_color=\"#([0-f]{6})\")?\\)$"),
        regex("^scene bg (\\w+)$"),
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
SayParser::SayParser()
{
    this->query = std::regex("^\"(.+)\" \"(.*)\"$");
};
RpyInstruction *SayParser::GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
{
    return new SayInstruction(script, rpyLine, FName(*params[0]), params[1]);
};
