#include "DataAssets/RpyScript.h"

// Online IDE - Code Editor, Compiler, Interpreter

#include <iostream>
#include <regex>
#include <string>

using namespace std;

int example() {
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
  for (string &line : lines) {
    bool matched = false;
    for (auto &reg : regs) {
      std::smatch m;
      matched = std::regex_match(line, m, reg);
      if (matched) {
        int counter = -1;
        for (auto &param : m) {
          ++counter;
          if (counter) {
            cout << param;
            cout << "|";
          } else {
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
}

TArray<FRpyLine> URpyScript::PYLinesFromString(FString text, uint8 TabSize) {
  TArray<FString> lines;
  text.ParseIntoArrayLines(lines, true);
  TArray<FRpyLine> PyLines;
  PyLines.Reserve(lines.Num());
  for (int idx = 0; idx < lines.Num(); ++idx) {
    FRpyLine rpyLine;
    // uint8 tabs=0;
    while (lines[idx][rpyLine.tabs] == ' ' || lines[idx][rpyLine.tabs] == '\t') {
      ++rpyLine.tabs;
    }
    if (lines[idx][rpyLine.tabs] == '#')
      continue;
    rpyLine.LineNumber = idx;
    rpyLine.line = lines[idx].RightChop(rpyLine.tabs);
    rpyLine.tabs = (rpyLine.tabs + 1) / TabSize;
    PyLines.Add(rpyLine);
  }
  return PyLines;
}