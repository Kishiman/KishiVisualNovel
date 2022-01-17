#pragma once

#include "Interfaces/C++/RpyInstruction.h"

class SayInstruction :public IRpyInstruction
{
public:
    virtual bool IsAsync() const override;
    virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
    virtual TScriptInterface<IRpyInterpreter> GetNext(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};