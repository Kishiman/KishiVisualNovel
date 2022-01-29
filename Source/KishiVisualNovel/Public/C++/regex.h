#pragma once

#include <iostream>
#include <regex>
#include <string>
#include <stdexcept>

using namespace std;
//regex:
string rpyKeywords[] = { "at","call","elif","else","expression","hide","if","image","init","jump","label","menu","onlayer","pass","python","return","scene","set","show","with","while", };
//regex


class RegexLib {
public:
    //primitive
    static string reg_integer;
    //rpy
    static string reg_keyword;
    static string reg_name;
    static string reg_image_name;
    static string reg_string;
    //parser
    static string reg_label;
    static string reg_say;
    static string reg_define_char;
};
string RegexLib::reg_integer = "([a-zA-Z_]\\w*)";

string RegexLib::reg_keyword = "(at|call|elif|else|expression|hide|if|image|init|jump|label|menu|onlayer|pass|python|return|scene|set|show|with|while)";
string RegexLib::reg_name = "([a-zA-Z_]\\w*)";
string RegexLib::reg_image_name = "((?:\\w+)(?: \\w+)*)";
string RegexLib::reg_string = "((?:'[^']*')|(?:\"[^\"]*\")|(?:`[^`]*`))";

string RegexLib::reg_label = "^label " + RegexLib::reg_name + ":$";
string RegexLib::reg_say = "^(?:(?:(\\w+)|\"(\\w+)\") (?:(\\w+) )?(?:@ (\\w+) )?)?" + RegexLib::reg_string + "(?: with (\\w+))?$";
string RegexLib::reg_define_char = "^\\$ " + RegexLib::reg_name + " = Character\\('(\\w+)'\\)$";


int _main() {
    regex regs[] = {
        regex(RegexLib::reg_label),
        regex(RegexLib::reg_say),
        regex(RegexLib::reg_define_char),
        regex("^scene(( \\w+ \\w+)+)$"),
        regex("^show(( (?!at)\\w+)+)(?: at (\\w+))?$"),
        regex("^hide (\\w+)$"),
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
        regex("^image " + RegexLib::reg_image_name + " = \"([/\\w\\.\\s]+)\"$"),
    };
    string lines[] = {
        "label start:",
        "\"Sylvie\" \"Hi there! how was class?\"",
        "\"I'll ask her...!\"",
        "define m = Character('Me')",
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
        "image eileen = \"/KishiVisualNovel/Rpy/Images/eileen_happy\"",
        "e happy @ vhappy \"Bam!!\" with vpunch",
    };
    for (string& line : lines) {
        bool matched = false;
        for (auto& reg : regs) {
            std::smatch m;
            matched = std::regex_match(line, m, reg);
            if (matched) {
                int counter = -1;
                for (auto& param : m) {
                    ++counter;
                    if (counter) {
                        cout << param;
                        cout << "|";
                    }
                    else {
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