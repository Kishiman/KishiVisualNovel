// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Modules/ModuleManager.h"
#include "Theories/ThEqual.h"
#include "Theories/ThClone.h"
#include "Theories/ThAdd.h"
#include "CoreMinimal.h"
#include "KishiMath.h"
#include "KishiTheory.h"
#include "KishiData.h"
#include "KishiConverts.h"
#include "Motion/Motion.h"
#include "Motion/FStationaryMotion.h"
#include "Motion/FLinearMotion.h"
#include "Motion/FKinematicMotion.h"
#include "Variable.h"
class FKishiLibModule : public IModuleInterface
{
public:
  /** IModuleInterface implementation */
  virtual void StartupModule() override;
  virtual void ShutdownModule() override;
};
