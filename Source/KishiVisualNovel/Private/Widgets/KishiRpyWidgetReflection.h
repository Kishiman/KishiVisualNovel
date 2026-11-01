#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PaperSprite.h"
#include "FileMediaSource.h"
#include "Rpy/RpyScriptTypes.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
/** Calls sprite, media and transition functions that widgets declare in Blueprint, by name. */
namespace
{
	/** Returns false if the widget has no SetSprite function. */
	inline bool SetImageOnWidget(UUserWidget* Widget, UPaperSprite* Sprite, FVector2D TargetSize)
	{
		if (!IsValid(Widget)) return false;
		UFunction* Function = Widget->FindFunction(FName(TEXT("SetSprite")));
		if (!Function) Function = Widget->FindFunction(FName(TEXT("Set Sprite")));
		if (!Function) return false;
		FStructOnScope Params(Function);
		// WGT_Sprite names the parameter Sprite; UKishiSpriteWidget names it InSprite.
		FObjectPropertyBase* SpriteProperty = FindFProperty<FObjectPropertyBase>(Function, TEXT("Sprite"));
		if (!SpriteProperty) SpriteProperty = FindFProperty<FObjectPropertyBase>(Function, TEXT("InSprite"));
		if (SpriteProperty)
			SpriteProperty->SetObjectPropertyValue_InContainer(Params.GetStructMemory(), Sprite);
		if (FStructProperty* Property = FindFProperty<FStructProperty>(Function, TEXT("TargetSize")))
			if (Property->Struct == TBaseStructure<FVector2D>::Get()) Property->CopyCompleteValue(Property->ContainerPtrToValuePtr<void>(Params.GetStructMemory()), &TargetSize);
		FProperty* Method = FindFProperty<FProperty>(Function, TEXT("Method"));
		if (!Method) Method = FindFProperty<FProperty>(Function, TEXT("ScalingMethod"));
		if (Method)
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
		return true;
	}

	/** Returns false if the widget has no SetFileMediaSource function. */
	inline bool SetMediaOnWidget(UUserWidget* Widget, UFileMediaSource* MediaSource, FVector2D TargetSize)
	{
		if (!IsValid(Widget)) return false;
		UFunction* Function = Widget->FindFunction(FName(TEXT("SetFileMediaSource")));
		if (!Function) Function = Widget->FindFunction(FName(TEXT("Set File Media Source")));
		if (!Function) return false;
		FStructOnScope Params(Function);
		FObjectPropertyBase* MediaProperty = FindFProperty<FObjectPropertyBase>(Function, TEXT("FileMediaSource"));
		if (!MediaProperty) MediaProperty = FindFProperty<FObjectPropertyBase>(Function, TEXT("InMediaSource"));
		if (MediaProperty)
			MediaProperty->SetObjectPropertyValue_InContainer(Params.GetStructMemory(), MediaSource);
		if (FStructProperty* Property = FindFProperty<FStructProperty>(Function, TEXT("TargetSize")))
			if (Property->Struct == TBaseStructure<FVector2D>::Get()) Property->CopyCompleteValue(Property->ContainerPtrToValuePtr<void>(Params.GetStructMemory()), &TargetSize);
		Widget->ProcessEvent(Function, Params.GetStructMemory());
		return true;
	}

	/**
	 * Warns once per class when a Blueprint subclass overrides an interface event that its native Base
	 * implements in C++: the override replaces the C++ body unless it calls its parent node.
	 */
	inline void WarnOnInterfaceEventOverrides(const UObject* Object)
	{
#if WITH_EDITOR
		if (!IsValid(Object)) return;
		const UClass* NativeBase = Object->GetClass();
		while (NativeBase && !NativeBase->HasAnyClassFlags(CLASS_Native)) NativeBase = NativeBase->GetSuperClass();
		static TSet<const UClass*> WarnedClasses;
		if (!NativeBase || WarnedClasses.Contains(Object->GetClass())) return;
		WarnedClasses.Add(Object->GetClass());
		for (const UClass* Class = Object->GetClass(); Class && Class != NativeBase; Class = Class->GetSuperClass())
		{
			for (TFieldIterator<UFunction> It(Class, EFieldIteratorFlags::ExcludeSuper); It; ++It)
			{
				const UFunction* NativeEvent = NativeBase->FindFunctionByName(It->GetFName());
				if (NativeEvent && NativeEvent->HasAllFunctionFlags(FUNC_BlueprintEvent | FUNC_Native) && NativeEvent->GetOuterUClass()->HasAnyClassFlags(CLASS_Interface))
				{
					UE_LOG(LogTemp, Warning, TEXT("%s overrides %s, which %s implements in C++. The C++ behavior is skipped unless the override calls its parent node; use the %s Hooks events instead."),
						*Class->GetName(), *It->GetName(), *NativeBase->GetName(), *NativeBase->GetName());
				}
			}
		}
#endif
	}

	inline void PlayImageTransition(UUserWidget* Widget, const FRpySceneOptions& Options, uint8 PlayMode)
	{
		if (!IsValid(Widget)) return;
		UFunction* Function = Widget->FindFunction(FName(TEXT("PlayTransition")));
		if (!Function) return;
		FStructOnScope Params(Function);
		if (FStructProperty* Property = FindFProperty<FStructProperty>(Function, TEXT("sceneOptions")))
			if (Property->Struct == FRpySceneOptions::StaticStruct()) Property->CopyCompleteValue(Property->ContainerPtrToValuePtr<void>(Params.GetStructMemory()), &Options);
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

