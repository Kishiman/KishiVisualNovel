#include "DataAssets/PyFile.h"

TArray<FPyLine> UPyFile::PYLinesFromString(FString text,uint8 TabSize){
    TArray<FString> lines;
    text.ParseIntoArrayLines(lines,true);
    TArray<FPyLine> PyLines;
    PyLines.Reserve(lines.Num());
    for (int idx = 0; idx < lines.Num(); ++idx)
    {
        FPyLine pyLine;
        //uint8 tabs=0;
        while (lines[idx][pyLine.tabs]==' ' || lines[idx][pyLine.tabs]=='\t')
        {
            ++pyLine.tabs;
        }
        if(lines[idx][pyLine.tabs]=='#')
            continue;
        pyLine.LineNumber=idx;
        pyLine.line=lines[idx].RightChop(pyLine.tabs);
        pyLine.tabs=(pyLine.tabs+1)/TabSize;
        PyLines.Add(pyLine);
    }
    return PyLines;
}