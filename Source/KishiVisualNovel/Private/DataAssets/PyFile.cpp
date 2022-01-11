#include "DataAssets/PyFile.h"

// Online IDE - Code Editor, Compiler, Interpreter

#include <iostream>
#include <regex>
#include <string>

using namespace std;

int example() {
  string lines[] = {
      "label start:",
      "\"Sylvie\" \"Hi there! how was class?\"",
      "\"I'll ask her...!\"",
      "define m = Character('Me', color=\"#c8c8ff\")",
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
      "if book:",
      "else:",
      "with fade",
      "show sylvie green at right",
  };
  regex regs[] = {
      regex("^label ([a-z]+):$"),
      regex("^\"(.+)\" \"(.*)\"$"),
      regex("^\"(.*)\"$"),
      regex("^define ([A-Za-z0-9]+) = Character\\('([A-Za-z0-9]+)', color=\"#([0-9a-fA-F]{6})\"\\)$"),
      regex("^scene bg ([A-Za-z0-9]+)$"),
      regex("^show(( (?!at)[A-Za-z0-9]+)+)(?: at ([A-Za-z0-9]+))?$"),
      regex("^hide ([A-Za-z0-9]+)$"),
      regex("^image(\\s[A-Za-z0-9]+)+ = \"([A-Za-z0-9\\_\\.\\s]+)\"$"),
      regex("^(play|stop|queue) (music|sound) \"(.*)\"(?: fadeout ([0-9\\.]+))?(?: fadein ([0-9\\.]+))?$"),
      regex("^pause(?: ([0-9\\.]+))?$"),
      regex("^return$"),
      regex("^with (fade|dissolve|None)$"),
      regex("^\"(.*)\":$"),
      regex("^jump ([a-z]+)$"),
      regex("^menu:$"),
      regex("^default ([a-z]+) = (False|True)$"),
      regex("^\\$ ([a-z]+) = (False|True)$"),
      regex("^\\if ([a-z]+):$"),
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
            cout << param << "\n=>:";
          }
        }
        cout << "\n";
        break;
      }
    }
    if (!matched)
      cout << "!not matched :" << line << "\n";
  }
  return 0;
}

TArray<FPyLine> UPyFile::PYLinesFromString(FString text, uint8 TabSize) {
  TArray<FString> lines;
  text.ParseIntoArrayLines(lines, true);
  TArray<FPyLine> PyLines;
  PyLines.Reserve(lines.Num());
  for (int idx = 0; idx < lines.Num(); ++idx) {
    FPyLine pyLine;
    // uint8 tabs=0;
    while (lines[idx][pyLine.tabs] == ' ' || lines[idx][pyLine.tabs] == '\t') {
      ++pyLine.tabs;
    }
    if (lines[idx][pyLine.tabs] == '#')
      continue;
    pyLine.LineNumber = idx;
    pyLine.line = lines[idx].RightChop(pyLine.tabs);
    pyLine.tabs = (pyLine.tabs + 1) / TabSize;
    PyLines.Add(pyLine);
  }
  return PyLines;
}