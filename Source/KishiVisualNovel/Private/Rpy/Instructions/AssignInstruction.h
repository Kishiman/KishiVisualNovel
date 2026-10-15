#pragma once

#include "RpyInstructionHelpers.h"

template <typename T>
struct AssignInstruction : public RpyInstruction
{
  FName varName;
  T value;

  AssignInstruction(URpyScript *script, FRpyLine *rpyLine, FName varName, T value) : RpyInstruction(script, rpyLine), varName(varName), value(value) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::NONE; }
  void SetValue(URpySession *session);
  virtual bool Execute(URpySession *session)
  {
    SetValue(session);
    return true;
  };
};
template <> inline void AssignInstruction<bool>::SetValue(URpySession *session) { session->runtimeData.bools.Add(varName, value); }
template <> inline void AssignInstruction<FName>::SetValue(URpySession *session) { session->runtimeData.names.Add(varName, value); }
template <> inline void AssignInstruction<FString>::SetValue(URpySession *session) { session->runtimeData.strings.Add(varName, value); }
template <> inline void AssignInstruction<int>::SetValue(URpySession *session) { session->runtimeData.ints.Add(varName, value); }
template <> inline void AssignInstruction<float>::SetValue(URpySession *session) { session->runtimeData.floats.Add(varName, value); }
