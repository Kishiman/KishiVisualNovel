#include "DataAssets/RpyScript.h"
#include "C++/RpyParsers.h"
#include "C++/RpyInstructions.h"
#include "Interfaces/RpyInterpreter.h"
#include "Misc/FileHelper.h" 

// Online IDE - Code Editor, parser, Interpreter

#include <iostream>
#include <regex>
#include <string>
#include <stdexcept>

using namespace std;
void URpyScript::PostLoad()
{
	Super::PostLoad();
	UE_LOG(LogTemp, Warning, TEXT("PostLoad, rpyLines:%d"), rpyLines.Num());
	Parse();
	Compile();
	FString basePath, right;
	this->AssetImportData->GetPathName().Split("/", &basePath, &right, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	TArray<FName> keys;
	//load images
	images.GetKeys(keys);
	for (auto& key : keys) {
		FRpyImage& rpyImage = images[key];
		FString path = basePath + "/Images/" + rpyImage.path;
		FText err;
		if (!FFileHelper::IsFilenameValidForSaving(path, err)) {
			UE_LOG(LogTemp, Error, TEXT("error :%s"), (*err.ToString()));
			continue;
		}
		UPaperSprite* image = Cast<UPaperSprite>(StaticLoadObject(UPaperSprite::StaticClass(), NULL, *path));
		if (image) {
			rpyImage.image = image;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("RpyImage not found at path : %s"), (*path));
		}
	};
	keys.Empty();
	//sounds
	images.GetKeys(keys);
	for (auto& key : keys) {
		FRpyAudio& rpyAudio = sounds[key];
		FString path = basePath + "/Audios/" + rpyAudio.path;
		FText err;
		if (!FFileHelper::IsFilenameValidForSaving(path, err)) {
			UE_LOG(LogTemp, Error, TEXT("error :%s"), (*err.ToString()));
			continue;
		}
		USoundWave* sound = Cast<USoundWave>(StaticLoadObject(USoundWave::StaticClass(), NULL, *path));
		if (sound) {
			rpyAudio.sound = sound;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("FRpyAudio not found at path : %s"), (*path));
		}	
	};
	keys.Empty();
};
void URpyScript::PostInitProperties()
{
	Super::PostInitProperties();
	UE_LOG(LogTemp, Warning, TEXT("PostInitProperties, rpyLines:%d"), rpyLines.Num());
	// Parse();
	// Compile();
};
void URpyScript::Serialize(FStructuredArchiveRecord Record)
{
	Super::Serialize(Record);
	// this->PostLoad();
};

URpyScript::URpyScript() : UKishiDataAsset()
{
	UE_LOG(LogTemp, Warning, TEXT("URpyScript(), rpyLines:%d"), rpyLines.Num());
	// Parse();
	// Compile();
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

// bool URpyScript::RunInstruction(const TScriptInterface<IRpyInterpreter>& interpreter, int index)
// {
// 	this->current = this->instructions[index];
// 	if (this->current)
// 		return this->current->Execute(interpreter);
// 	return false;
// }

// bool URpyScript::StartLabel(const TScriptInterface<IRpyInterpreter>& interpreter, FName label)
// {
// 	this->current = this->labels[label];
// 	if (!this->current)
// 		return true;
// 	while (this->current && IRpyInterpreter::Execute_AutoRunNext(interpreter.GetObject()))
// 	{
// 		bool allGood = this->current->Execute(interpreter);
// 		if (!allGood)
// 			return false;
// 		this->current = this->current->GetNext(interpreter);
// 	}
// 	return true;
// };
// bool URpyScript::RunNext(const TScriptInterface<IRpyInterpreter>& interpreter)
// {
// 	this->current = this->current->GetNext(interpreter);
// 	if (this->current)
// 		return this->current->Execute(interpreter);
// 	return false;
// };
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
	TArray<RpyParser*> parsers;
	// TODO
	parsers.Add(new InitParser());
	parsers.Add(new LabelParser());
	parsers.Add(new DefineCharacterParser());
	parsers.Add(new SayParser());
	parsers.Add(new NarratorSayParser());
	parsers.Add(new CharacterSayParser());
	parsers.Add(new ImageParser());
	parsers.Add(new ShowParser());
	parsers.Add(new HideParser());
	//
	for (auto instruction : instructions)
	{
		delete instruction;
	}
	instructions.Empty();
	for (auto& rpyLine : rpyLines)
	{
		bool matched = false;
		for (auto parser : parsers)
		{
			std::smatch m;
			string target = TCHAR_TO_UTF8(*rpyLine.line);
			std::regex& query = parser->reg_query;
			matched = std::regex_match(target, m, query);
			if (matched)
			{
				UE_LOG(LogTemp, Display, TEXT("matched query:%s"), (*parser->query));
				TArray<FString> params;
				int counter = -1;
				for (auto& param : m)
				{
					++counter;
					if (counter)
					{
						string s = param;
						FString fs = s.c_str();
						UE_LOG(LogTemp, Display, TEXT("param[%d]:%s"), counter - 1, (*fs));
						params.Add(fs);
					}
				}
				if (params.Num() != parser->paramsNum) {
					matched = false;
					UE_LOG(LogTemp, Warning, TEXT("number of params(%d) not equal to parser (%d)"), params.Num(), parser->paramsNum);
					continue;
				}

				RpyInstruction* instruction = parser->GetRpyInstruction(this, &rpyLine, params);
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
	for (auto parser : parsers) {
		delete parser;
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
	TArray<RpyInstruction*> stack;
	RpyInstruction* current;
	for (int idx = 0; idx < instructions.Num(); ++idx)
	{
		auto num = stack.Num();
		RpyInstruction* last = num > 0 ? stack[num - 1] : nullptr;
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
					stack.Pop();
				}
				current->parent = currentTabs > 0 ? stack[currentTabs - 1] : nullptr;
				stack.Add(current);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("unvalid tabs transition at line :%d"), current->rpyLine->LineNumber);
				return false;
			}
		}
		else if (currentTabs == 0)
		{
			stack.Add(current);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("unvalid tabs transition at line :%d"), current->rpyLine->LineNumber);
			return false;
		}
		if (current->parent)
		{
			current->parent->children.Add(current);
		}
	}
	current = nullptr;
	for (auto instruction : instructions)
	{
		if (!instruction->Compile())
		{
			UE_LOG(LogTemp, Error, TEXT("failed to compile line %d : %s"), current->rpyLine->LineNumber, (*current->rpyLine->line));
			return false;
		}
	}
	return true;
}
