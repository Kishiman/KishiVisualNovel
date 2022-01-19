#include "Interfaces/C++/RpyInstructions.h"
#include "Interfaces/RpyInterpreter.h"

bool SayInstruction::Execute(const TScriptInterface<IRpyInterpreter> &interpreter) {
    return IRpyInterpreter::Execute_Say(interpreter.GetObject(),this->name,this->statement);
};

bool SceneInstruction::Execute(const TScriptInterface<IRpyInterpreter> &interpreter) {
    return IRpyInterpreter::Execute_Scene(interpreter.GetObject(),this->params);
};

bool ShowInstruction::Execute(const TScriptInterface<IRpyInterpreter> &interpreter) {
    return IRpyInterpreter::Execute_Show(interpreter.GetObject(),this->name,this->params);
};
bool HideInstruction::Execute(const TScriptInterface<IRpyInterpreter> &interpreter) {
    return IRpyInterpreter::Execute_Hide(interpreter.GetObject(),this->name,this->params);
};


