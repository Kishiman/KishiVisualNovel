#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PaperSprite.h"
#include "FileMediaSource.h"
#include "Rpy/RpyScriptTypes.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
namespace
{
	void SetImageOnWidget(UUserWidget* Widget, UPaperSprite* Sprite, FVector2D TargetSize)
	{
		if (!IsValid(Widget)) return;
		UFunction* Function = Widget->FindFunction(FName(TEXT("SetSprite")));
		if (!Function) Function = Widget->FindFunction(FName(TEXT("Set Sprite")));
		if (!Function) return;
		FStructOnScope Params(Function);
		if (FObjectPropertyBase* Property = FindFProperty<FObjectPropertyBase>(Function, TEXT("Sprite")))
			Property->SetObjectPropertyValue_InContainer(Params.GetStructMemory(), Sprite);
		if (FStructProperty* Property = FindFProperty<FStructProperty>(Function, TEXT("TargetSize")))
			if (Property->Struct == TBaseStructure<FVector2D>::Get()) Property->CopyCompleteValue_InContainer(Params.GetStructMemory(), &TargetSize);
		if (FProperty* Method = FindFProperty<FProperty>(Function, TEXT("Method")))
		{
			if (FEnumProperty* Enum = CastField<FEnumProperty>(Method))
			{
				const int64 FitValue = Enum->GetEnum()->GetValueByNameString(TEXT("Fit"));
				Enum->GetUnderlyingProperty()->SetIntPropertyValue(Enum->ContainerPtrToValuePtr<void>(Params.GetStructMemory()), FitValue);
			}
			else if (FByteProperty* Byte = CastField<FByteProperty>(Method))
			{
				if (Byte->Enum) Byte->SetIntPropertyValue(Byte->ContainerPtrToValuePtr<void>(Params.GetStructMemory()), Byte->Enum->GetValueByNameString(TEXT("Fit")));
			}
		}
		Widget->ProcessEvent(Function, Params.GetStructMemory());
	}

	void SetMediaOnWidget(UUserWidget* Widget, UFileMediaSource* MediaSource, FVector2D TargetSize)
	{
		if (!IsValid(Widget)) return;
		UFunction* Function = Widget->FindFunction(FName(TEXT("SetFileMediaSource")));
		if (!Function) Function = Widget->FindFunction(FName(TEXT("Set File Media Source")));
		if (!Function) return;
		FStructOnScope Params(Function);
		if (FObjectPropertyBase* Property = FindFProperty<FObjectPropertyBase>(Function, TEXT("FileMediaSource")))
			Property->SetObjectPropertyValue_InContainer(Params.GetStructMemory(), MediaSource);
		if (FStructProperty* Property = FindFProperty<FStructProperty>(Function, TEXT("TargetSize")))
			if (Property->Struct == TBaseStructure<FVector2D>::Get()) Property->CopyCompleteValue_InContainer(Params.GetStructMemory(), &TargetSize);
		Widget->ProcessEvent(Function, Params.GetStructMemory());
	}

	void PlayImageTransition(UUserWidget* Widget, const FRpySceneOptions& Options, uint8 PlayMode)
	{
		if (!IsValid(Widget)) return;
		UFunction* Function = Widget->FindFunction(FName(TEXT("PlayTransition")));
		if (!Function) return;
		FStructOnScope Params(Function);
		if (FStructProperty* Property = FindFProperty<FStructProperty>(Function, TEXT("sceneOptions")))
			if (Property->Struct == FRpySceneOptions::StaticStruct()) Property->CopyCompleteValue_InContainer(Params.GetStructMemory(), &Options);
		if (FProperty* Property = FindFProperty<FProperty>(Function, TEXT("PlayMode")))
		{
			if (FEnumProperty* Enum = CastField<FEnumProperty>(Property))
				Enum->GetUnderlyingProperty()->SetIntPropertyValue(Enum->ContainerPtrToValuePtr<void>(Params.GetStructMemory()), static_cast<int64>(PlayMode));
			else if (FNumericProperty* Numeric = CastField<FNumericProperty>(Property))
				Numeric->SetIntPropertyValue(Numeric->ContainerPtrToValuePtr<void>(Params.GetStructMemory()), static_cast<int64>(PlayMode));
		}
		Widget->ProcessEvent(Function, Params.GetStructMemory());
	}
}

