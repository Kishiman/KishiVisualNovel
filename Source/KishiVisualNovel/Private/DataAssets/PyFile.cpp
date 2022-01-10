#include "DataAssets/PyFile.h"

// Online IDE - Code Editor, Compiler, Interpreter

#include <iostream>
#include <regex>
#include <string>

using namespace std;

int example() {
  string lines[] = {
      "set x 1",
      "label start:",
      "\"Sylvie\" \"Hi there! how was class?\"",
      "\"I'll ask her...!\"",
      "define m = Character('Me', color=\"#c8c8ff\")",
      "scene bg meadow",
      "show sylvie green smile",
      "hide sylvie",
      "image logo = \"renpy logo.png\"",
      "image eileen happy = \"eileen_happy_blue_dress.png\"",
      "with fade",
      "show sylvie green  at right"
  };
  regex regs[] = {
      regex("^set\\s([a-z]+)\\s([0-9]+)$"),
      regex("^label\\s*([a-z]+)\\s*:$"),
      regex("^\"(.+)\" \"(.*)\"$"),
      regex("^\"(.*)\"$"),
      regex("^define ([A-Za-z0-9]+) = Character\\('([A-Za-z0-9]+)', color=\"#([0-9a-fA-F]{6})\"\\)$"),
      regex("^scene bg ([A-Za-z0-9]+)$"),
      regex("^show(( [A-Za-z0-9]+)+)  (?:at ([A-Za-z0-9]+))?"),
      regex("^hide ([A-Za-z0-9]+)$"),
      regex("^image(\\s[A-Za-z0-9]+)+ = \"([A-Za-z0-9\\_\\.\\s]+)\"$"),
      regex("^with (fade|dissolve|None)$"),
  };
  for (string &line : lines) {
    bool matched = false;
    for (auto &reg : regs) {
      std::smatch m;
      matched = std::regex_match(line, m, reg);
      if (matched) {
        for (auto &param : m) {
          cout << param;
          cout << "|";
        }
        cout << "\n";
        break;
      }
    }
  }
  cout << "Welcome to Online IDE!! Happy Coding :)";
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