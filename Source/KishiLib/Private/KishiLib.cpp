// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "KishiLib.h"

#define LOCTEXT_NAMESPACE "FKishiLibModule"

void FKishiLibModule::StartupModule()
{
  UE_LOG(LogTemp, Warning, TEXT("KishiLib On"));
  // TMotionFrame<FVector> startFV(FVector(1, 1, 0));
  // TMotionFrame<FVector> endFV(FVector(2, 2, 2));
  // TMotionFrame<float> startF(0, 1, 0);
  // TMotionFrame<float> endF(0, -1, 0);

  // TFKinematicMotion<float> motionFloat(startF, endF, 2);
  // TFKinematicMotion<float> motionFloat2 = -motionFloat;
  // TFLinearMotion<float> motionFloat3 = -motionFloat2;
  TFStationaryMotion<float> motionFloat2(float(2.0), 4);
  IMotionable<float> motionablefloat2 = (motionFloat2);
  IMotionable<float> motionablefloat2Inverted = -motionablefloat2;
  IMotionable<float> motionablefloat2double = motionablefloat2 - motionablefloat2Inverted;
  IMotionable<float> motionablefloat2Null = motionablefloat2 + motionablefloat2Inverted;
  int y = 0;
  UE_LOG(LogTemp, Warning, TEXT(" 2= %f = %f = 2=%f "), motionFloat2.GetStartFrame().point, motionablefloat2.GetStartFrame().point, motionablefloat2Inverted.GetStartFrame().point);

  //motionFloat2.SolveAccelerationTime();
  // This code will execute after your module is loaded into memory; the exact
  // timing is specified in the .uplugin file per-module
}

void FKishiLibModule::ShutdownModule()
{
  UE_LOG(LogTemp, Warning, TEXT("KishiLib Off"));

  // This function may be called during shutdown to clean up your module.  For
  // modules that support dynamic reloading, we call this function before
  // unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FKishiLibModule, KishiLib)