#include "DataAssets/RpyScript.h"
#include "C++/RpyParsers.h"
#include "C++/RpyInstructions.h"

// Online IDE - Code Editor, Compiler, Interpreter

#include <iostream>
#include <regex>
#include <string>
#include <stdexcept>

using namespace std;

TArray<FName> URpyScript::GetLabels() const
{
  TArray<FName> out;
  labels.GetKeys(out);
  return out;
};

bool URpyScript::StartLabel(const TScriptInterface<IRpyInterpreter> &interpreter, FName label)
{
  this->current = this->labels[label];
  if (!this->current)
    return false;
  return this->current->Execute(interpreter);
};
bool URpyScript::RunNext(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  this->current = this->current->GetNext(interpreter);
  if (!this->current)
    return false;
  return this->current->Execute(interpreter);
};
void URpyScript::ImportRpyLines(FString text, uint8 TabSize)
{
  TArray<FString> lines;
  text.ParseIntoArrayLines(lines, true);
  rpyLines.Empty();
  rpyLines.Reserve(lines.Num());
  for (int idx = 0; idx < lines.Num(); ++idx)
  {
    FRpyLine rpyLine;
    // uint8 tabs=0;
    while (lines[idx][rpyLine.tabs] == ' ' || lines[idx][rpyLine.tabs] == '\t')
    {
      ++rpyLine.tabs;
    }
    if (lines[idx][rpyLine.tabs] == '#')
      continue;
    rpyLine.LineNumber = idx;
    rpyLine.line = lines[idx].RightChop(rpyLine.tabs);
    rpyLine.tabs = (rpyLine.tabs + 1) / TabSize;
    rpyLines.Add(rpyLine);
  }
};

void URpyScript::Parse()
{
  TArray<RpyParser *> compilers;
  // TODO
  compilers.Add(new SayParser());
  //
  for (auto instruction : instructions)
  {
    delete instruction;
  }
  instructions.Empty();
  for (auto &rpyLine : rpyLines)
  {
    bool matched = false;
    for (auto compiler : compilers)
    {
      std::smatch m;
      string target = TCHAR_TO_UTF8(*rpyLine.line);
      matched = std::regex_match(target, m, compiler->query);
      if (matched)
      {
        TArray<FString> params;
        int counter = -1;
        for (auto &param : m)
        {
          ++counter;
          if (counter)
          {
            string s = param;
            params.Add(s.c_str());
          }
        }
        RpyInstruction *instruction = compiler->GetRpyInstruction(this, &rpyLine, params);
        if (!instruction)
        {
          matched = false;
          continue;
        }
        break;
      }
    }
    if (!matched)
    {
      throw std::runtime_error(TCHAR_TO_UTF8(*rpyLine.line));
    }
  }
};
void URpyScript::Compile(){

}
