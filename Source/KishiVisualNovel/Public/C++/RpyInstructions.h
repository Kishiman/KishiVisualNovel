#pragma once

#include "C++/RpyInstruction.h"
#include "Interfaces/RpyInterpreter.h"


struct BlankInstruction : public RpyInstruction {
  BlankInstruction(URpyScript* script, FRpyLine* rpyLine) : RpyInstruction(script, rpyLine) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    RpyInstruction::Execute(interpreter);
    return true;
  };
};

struct JumpInstruction : public RpyInstruction {
  JumpInstruction(URpyScript* script, FRpyLine* rpyLine, FName name) : RpyInstruction(script, rpyLine) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    return false;
  };
};

struct CallInstruction : public RpyInstruction {
  CallInstruction(URpyScript* script, FRpyLine* rpyLine, FName name) : RpyInstruction(script, rpyLine) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    return false;
  };
};

struct ReturnInstruction : public RpyInstruction {
  ReturnInstruction(URpyScript* script, FRpyLine* rpyLine) : RpyInstruction(script, rpyLine) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    return false;
  };
};

struct SceneInstruction : public RpyInstruction {
  TMap<FName, FString> params;

  SceneInstruction(URpyScript* script, FRpyLine* rpyLine, TMap<FName, FString> params) : RpyInstruction(script, rpyLine), params(params) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    RpyInstruction::Execute(interpreter);
    return IRpyInterpreter::Execute_Scene(interpreter.GetObject(), this->params);
  };
};

struct ShowInstruction : public RpyInstruction {
  FName name;
  FName at;
  FName with;

  ShowInstruction(URpyScript* script, FRpyLine* rpyLine, FName name, FName at, FName with) : RpyInstruction(script, rpyLine), name(name), at(at), with(with) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    RpyInstruction::Execute(interpreter);
    UPaperSprite* image = script->images[name];
    if (!image)
      return false;
    return IRpyInterpreter::Execute_Show(interpreter.GetObject(), image, at, with);
  };
};

struct HideInstruction : public RpyInstruction {
  FName name;
  TMap<FName, FString> params;

  HideInstruction(URpyScript* script, FRpyLine* rpyLine, FName name, TMap<FName, FString> params) : RpyInstruction(script, rpyLine), name(name), params(params) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    RpyInstruction::Execute(interpreter);
    return IRpyInterpreter::Execute_Hide(interpreter.GetObject(), this->name, this->params);
  };
};

struct SayInstruction : public RpyInstruction {
  FName name;
  FString statement;

  SayInstruction(URpyScript* script, FRpyLine* rpyLine, FName name, FString statement) : RpyInstruction(script, rpyLine), name(name), statement(statement) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    RpyInstruction::Execute(interpreter);
    return IRpyInterpreter::Execute_Say(interpreter.GetObject(), this->name, this->statement);
  };
};

struct PlayInstruction : public RpyInstruction {
  FName name;
  TMap<FName, FString> params;

  PlayInstruction(URpyScript* script, FRpyLine* rpyLine, FName name, TMap<FName, FString> params) : RpyInstruction(script, rpyLine), name(name), params(params) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    RpyInstruction::Execute(interpreter);
    return IRpyInterpreter::Execute_PlayMusic(interpreter.GetObject(), this->name, this->params);
  };
};

struct PauseInstruction : public RpyInstruction {
  float timeout;

  PauseInstruction(URpyScript* script, FRpyLine* rpyLine, float timeout) : RpyInstruction(script, rpyLine), timeout(timeout) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    RpyInstruction::Execute(interpreter);
    return IRpyInterpreter::Execute_Pause(interpreter.GetObject(), this->timeout);
  };
};
