#include "DataAssets/RpyScript.h"
#include "C++/RpyInstructions.h"
#include "C++/RpyParsers.h"
#include "Interfaces/RpyInterpreter.h"

// Online IDE - Code Editor, parser, Interpreter

#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>

using namespace std;
void URpyScript::PostLoad()
{
  Super::PostLoad();
  UE_LOG(LogTemp, Warning, TEXT("PostLoad, rpyLines:%d"), rpyLines.Num());
  Parse();
  Compile();
};
void URpyScript::PostInitProperties()
{
  Super::PostInitProperties();
  UE_LOG(LogTemp, Warning, TEXT("PostInitProperties, rpyLines:%d"),
         rpyLines.Num());
  Parse();
  Compile();
};
void URpyScript::Serialize(FStructuredArchiveRecord Record)
{
  Super::Serialize(Record);
  // this->PostLoad();
};

URpyScript::URpyScript() : UKishiDataAsset()
{
  Parse();
  Compile();
};
TArray<FName> URpyScript::GetLabels() const
{
  TArray<FName> out;
  labels.GetKeys(out);
  return out;
};
int URpyScript::GetInstructionsLength() const { return instructions.Num(); };

bool URpyScript::RunInstruction(
    const TScriptInterface<IRpyInterpreter> &interpreter, int index)
{
  this->current = this->instructions[index];
  if (this->current)
    return this->current->Execute(interpreter);
  return false;
}

bool URpyScript::StartLabel(
    const TScriptInterface<IRpyInterpreter> &interpreter, FName label)
{
  if (!this->labels.Contains(label))
    return false;
  this->current = this->labels[label];
  if (!this->current)
    return true;
  while (this->current && IRpyInterpreter::Execute_AutoRunNext(interpreter.GetObject()))
  {
    bool allGood = this->current->Execute(interpreter);
    if (!allGood)
      return false;
    this->current = this->current->GetNext(interpreter);
  }
  return true;
};
bool URpyScript::RunNext(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  this->current = this->current->GetNext(interpreter);
  if (this->current)
    return this->current->Execute(interpreter);
  return false;
};
bool URpyScript::ImportRpyLines(FString text, uint8 TabSize)
{
  TArray<FString> lines;
  text.ParseIntoArrayLines(lines, true);
  rpyLines.Empty();
  rpyLines.Reserve(lines.Num());
  for (int idx = 0; idx < lines.Num(); ++idx)
  {
    FRpyLine rpyLine;
    // uint8 tabs=0;
    while (rpyLine.tabs < lines[idx].Len() && (lines[idx][rpyLine.tabs] == ' ' || lines[idx][rpyLine.tabs] == '\t'))
    {
      ++rpyLine.tabs;
    }
    if (rpyLine.tabs == lines[idx].Len())
      continue;
    if (lines[idx][rpyLine.tabs] == '#')
      continue;
    rpyLine.LineNumber = idx;
    rpyLine.line = lines[idx].RightChop(rpyLine.tabs);
    rpyLine.tabs = (rpyLine.tabs + 1) / TabSize;
    rpyLines.Add(rpyLine);
  }
  return true;
};

bool URpyScript::Parse()
{
  TArray<RpyParser *> parsers;
  // TODO
  parsers.Add(new InitParser());
  parsers.Add(new LabelParser());
  parsers.Add(new DefineCharacterParser());
  parsers.Add(new SayParser());
  parsers.Add(new SayParser2());
  parsers.Add(new IfBoolParser());
  parsers.Add(new ImageParser());
  parsers.Add(new ShowParser());
  //
  for (auto instruction : instructions)
  {
    delete instruction;
  }
  instructions.Empty();
  for (auto &rpyLine : rpyLines)
  {
    bool matched = false;
    for (auto parser : parsers)
    {
      std::smatch m;
      string target = TCHAR_TO_UTF8(*rpyLine.line);
      string _query = TCHAR_TO_UTF8(*parser->query);
      std::regex query = regex(_query);
      matched = std::regex_match(target, m, query);
      if (matched)
      {
        UE_LOG(LogTemp, Display, TEXT("matched query:%s"), (*parser->query));
        TArray<FString> params;
        int counter = -1;
        for (auto &param : m)
        {
          ++counter;
          if (counter)
          {
            string s = param;
            FString fs = s.c_str();
            UE_LOG(LogTemp, Display, TEXT("param[%d]:%s"), counter, (*fs));
            params.Add(fs);
          }
        }
        RpyInstruction *instruction =
            parser->GetRpyInstruction(this, &rpyLine, params);
        if (!instruction)
        {
          matched = false;
          UE_LOG(LogTemp, Warning,
                 TEXT("Failed to GetRpyInstruction from line %d : %s"),
                 rpyLine.LineNumber, (*rpyLine.line));
          continue;
        }
        instructions.Add(instruction);
        break;
      }
    }
    if (!matched)
    {
      UE_LOG(LogTemp, Error, TEXT("Failed to match line %d : %s"),
             rpyLine.LineNumber, (*rpyLine.line));
      return false;
    }
  }
  for (auto parser : parsers)
  {
    delete parser;
  }
  return true;
};
bool URpyScript::Compile()
{
  TArray<RpyInstruction *> stack;
  for (int idx = 0; idx < instructions.Num(); ++idx)
  {
    auto num = stack.Num();
    RpyInstruction *last = num > 0 ? stack[num - 1] : nullptr;
    current = instructions[idx];
    int currentTabs = current->rpyLine->tabs;
    if (last)
    {
      int lastTabs = last->rpyLine->tabs;
      if (currentTabs == lastTabs + 1)
      {
        current->parent = last;
        stack.Add(current);
      }
      else if (currentTabs <= lastTabs)
      {
        int count = num - currentTabs;
        while (count--)
        {
          stack.Pop()->next = current;
        }
        current->parent = currentTabs > 0 ? stack[currentTabs - 1] : nullptr;
        stack.Add(current);
      }
      else
      {
        UE_LOG(LogTemp, Error, TEXT("unvalid tabs transition at line :%d"),
               current->rpyLine->LineNumber);
        return false;
      }
    }
    else if (currentTabs == 0)
    {
      stack.Add(current);
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("unvalid tabs transition at line :%d"),
             current->rpyLine->LineNumber);
      return false;
    }
    if (current->parent)
    {
      current->parent->children.Add(current);
    }
  }
  // TODO
  // for (int idx = 0; idx < instructions.Num() - 1; ++idx)
  // {
  // 	instructions[idx]->next = instructions[idx + 1];
  // }
  // for (auto instruction : instructions)
  // {
  // 	if (instruction->children.Num() > 0)
  // 	{
  // 		instruction->next = instruction->children[instruction->children.Num()
  // - 1]->next;
  // 	}
  // }
  current = nullptr;
  for (auto instruction : instructions)
  {
    if (!instruction->Compile())
    {
      UE_LOG(LogTemp, Error, TEXT("failed to compile line %d : %s"),
             instruction->rpyLine->LineNumber, (*instruction->rpyLine->line));
      return false;
    }
  }
  return true;
}
