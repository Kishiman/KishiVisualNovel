#pragma once

#include "C++/RpyInstruction.h"
#include "Interfaces/RpyInterpreter.h"


//Labels & Control Flow
struct BlankInstruction : public RpyInstruction {
  BlankInstruction(URpyScript* script, FRpyLine* rpyLine) : RpyInstruction(script, rpyLine) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    RpyInstruction::Execute(interpreter);
    return true;
  };
};

struct JumpInstruction : public RpyInstruction {
  FName name;
  JumpInstruction(URpyScript* script, FRpyLine* rpyLine, FName name) : RpyInstruction(script, rpyLine), name(name) {};
  virtual RpyInstruction* GetNext(const TScriptInterface<IRpyInterpreter>& interpreter) {
    auto label = script->labels[name];
    if (!label) {
      UE_LOG(LogTemp, Error, TEXT("unvalid label name:%s"), (*name.ToString()));
      return nullptr;
    }
    return label;
  };
};

struct CallInstruction : public JumpInstruction {
  CallInstruction(URpyScript* script, FRpyLine* rpyLine, FName name) : JumpInstruction(script, rpyLine, name) {};
  virtual RpyInstruction* GetNext(const TScriptInterface<IRpyInterpreter>& interpreter) {
    script->callStack.Add(next);
    return JumpInstruction::GetNext(interpreter);
  };
};

struct ReturnInstruction : public RpyInstruction {
  ReturnInstruction(URpyScript* script, FRpyLine* rpyLine) : RpyInstruction(script, rpyLine) {};
  virtual RpyInstruction* GetNext(const TScriptInterface<IRpyInterpreter>& interpreter) {
    return script->callStack.Pop();
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
  FRpyImage rpyImage;
  FName at;
  FName with;

  ShowInstruction(URpyScript* script, FRpyLine* rpyLine, FRpyImage rpyImage, FName at, FName with) : RpyInstruction(script, rpyLine), rpyImage(rpyImage), at(at), with(with) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    RpyInstruction::Execute(interpreter);
    rpyImage.image = script->images[rpyImage.name];
    if (!rpyImage.image)
      return false;
    return IRpyInterpreter::Execute_Show(interpreter.GetObject(), rpyImage, at, with);
  };
};

struct HideInstruction : public RpyInstruction {
  FRpyImage rpyImage;
  FName at;
  FName with;

  HideInstruction(URpyScript* script, FRpyLine* rpyLine, FRpyImage rpyImage, FName at, FName with) : RpyInstruction(script, rpyLine), rpyImage(rpyImage), at(at), with(with) {};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter>& interpreter) {
    RpyInstruction::Execute(interpreter);
    rpyImage.image = script->images[rpyImage.name];
    if (!rpyImage.image)
      return false;
    return IRpyInterpreter::Execute_Hide(interpreter.GetObject(), rpyImage, at, with);
  };
};

struct SayInstruction : public RpyInstruction {
  FName name;
  FString statement;
  FName with;

  SayInstruction(URpyScript* script, FRpyLine* rpyLine, FName name, FString statement, FName with = "") :
    RpyInstruction(script, rpyLine), name(name), statement(statement), with(with) {};
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
