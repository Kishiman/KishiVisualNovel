#include "DataAssets/RpyScript.h"
#include "C++/RpyParsers.h"
#include "C++/RpyInstructions.h"
#include "Interfaces/RpyInterpreter.h"

// Online IDE - Code Editor, parser, Interpreter

#include <iostream>
#include <regex>
#include <string>
#include <stdexcept>

using namespace std;
void uRpyScript::PostLoad(){
  UE_LOG(LogTemp, Warning, TEXT("PostLoad, rpyLines:"%d),rpyLines.Num());
  Parse();
  Compile();
};
void uRpyScript::PostInitProperties(){
  UE_LOG(LogTemp, Warning, TEXT("PostInitProperties, rpyLines:"%d),rpyLines.Num());
  Parse();
  Compile();
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
int URpyScript::GetInstructionsLength() const
{
  return instructions.Num();
};

bool URpyScript::RunInstruction(const TScriptInterface<IRpyInterpreter> &interpreter, int index)
{
  this->current = this->instructions[index];
  if (this->current)
    return this->current->Execute(interpreter);
  return false;
}

bool URpyScript::StartLabel(const TScriptInterface<IRpyInterpreter> &interpreter, FName label)
{
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
  return true;
};

bool URpyScript::Parse()
{
  TArray<RpyParser *> parsers;
  // TODO
  parsers.Add(new SayParser());
  parsers.Add(new LabelParser());
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
      UE_LOG(LogTemp, Display, TEXT("try parse query:%s"), (*parser->query));
      std::smatch m;
      string target = TCHAR_TO_UTF8(*rpyLine.line);
      string _query = TCHAR_TO_UTF8(*parser->query);
      std::regex query = regex(_query);
      matched = std::regex_match(target, m, query);
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
        RpyInstruction *instruction = parser->GetRpyInstruction(this, &rpyLine, params);
        if (!instruction)
        {
          matched = false;
          UE_LOG(LogTemp, Warning, TEXT("Failed to GetRpyInstruction from line %d : %s"), rpyLine.LineNumber, (*rpyLine.line));
          continue;
        }
        instructions.Add(instruction);
        break;
      }
    }
    if (!matched)
    {
      UE_LOG(LogTemp, Error, TEXT("Failed to match line %d : %s"), rpyLine.LineNumber, (*rpyLine.line));
      return false;
    }
  }
  return true;
};
bool URpyScript::Compile()
{
  // TODO
  for (int idx = 0; idx < instructions.Num() - 1; ++idx)
  {
    instructions[idx]->next = instructions[idx + 1];
  }
  TArray<instruction*> stack;
  for (int idx = 0; idx < instructions.Num(); ++idx)
  {
    auto num=stack.Num();
    RpyInstruction* last=num>0?stack[num-1]:nullptr;
    current=instructions[idx];
    int currentTabs=current->rpyLine.tabs;
    if(last){
      int lastTabs=last->rpyLine.tabs;
      if(currentTabs==lastTabs+1){
        current->parent=last;
        stack.Add(current);
      }else if(currentTabs<=lastTabs){
          int count=num-currentTabs
          while(count--){
            stack.Pop();
          }
          current->parent=currentTabs>0?stack[currentTabs-1]:nullptr;
          stack.Add(current);
      }else{
        UE_LOG(LogTemp, Error, TEXT("unvalid tabs transition at line :%d"),current->rpyLine.LineNumber);
        return false;
      }
    }else if(currentTabs==0){
      stack.Add(current);
    }else{
        UE_LOG(LogTemp, Error, TEXT("unvalid tabs transition at line :%d"),current->rpyLine.LineNumber);
        return false;
    }
    if(current->parent){
      parent->children.Add(current);
    }
  }
  current = nullptr;
  for (auto instruction : instructions)
  {
    if(!instruction->Compile()){
        UE_LOG(LogTemp, Error, TEXT("failed to compile line %d : %s"),current->rpyLine.LineNumber,(*rpyLine.line));
        return false;
    }
  }
  return true;
}
