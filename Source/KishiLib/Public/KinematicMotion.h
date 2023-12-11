// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "KishiLib.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KinematicMotion.generated.h"

/**
 * 
 */
UCLASS()
class KISHILIB_API UKinematicMotion : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
//float
	UFUNCTION(BlueprintPure, category = HelloWorld)
	static void GetHelloWorld(FString &helloWorld) { helloWorld = "HelloWorld"; }
	UFUNCTION(BlueprintPure, category = float)
	static void ATFromSUV(const float S, const float U, const float V, float &A, float &T);
	UFUNCTION(BlueprintPure, category = float)
	static void VTFromSUA(const float S, const float U, float &V, const float A, float &T);
	UFUNCTION(BlueprintPure, category = float)
	static void VAFromSUT(const float S, const float U, float &V, float &A, const float T);
	UFUNCTION(BlueprintPure, category = float)
	static void UTFromSVA(const float S, float &U, const float V, const float A, float &T);
	UFUNCTION(BlueprintPure, category = float)
	static void UAFromSVT(const float S, float &U, const float V, float &A, const float T);
	UFUNCTION(BlueprintPure, category = float)
	static void UVFromSAT(const float S, float &U, float &V, const float A, const float T);
	UFUNCTION(BlueprintPure, category = float)
	static void STFromUVA(float &S, const float U, const float V, const float A, float &T);
	UFUNCTION(BlueprintPure, category = float)
	static void SAFromUVT(float &S, const float U, const float V, float &A, const float T);
	UFUNCTION(BlueprintPure, category = float)
	static void SVFromUAT(float &S, const float U, float &V, const float A, const float T);
	UFUNCTION(BlueprintPure, category = float)
	static void SUFromVAT(float &S, float &U, const float V, const float A, const float T);
//FVector
	UFUNCTION(BlueprintPure, category = FVector)
	static void FV_ATFromSUV(const FVector S, const FVector U, const FVector V, FVector &A, float &T);
	UFUNCTION(BlueprintPure, category = FVector)
	static void FV_VTFromSUA(const FVector S, const FVector U, FVector &V, const FVector A, float &T);
	UFUNCTION(BlueprintPure, category = FVector)
	static void FV_VAFromSUT(const FVector S, const FVector U, FVector &V, FVector &A, const float T);
	UFUNCTION(BlueprintPure, category = FVector)
	static void FV_UTFromSVA(const FVector S, FVector &U, const FVector V, const FVector A, float &T);
	UFUNCTION(BlueprintPure, category = FVector)
	static void FV_UAFromSVT(const FVector S, FVector &U, const FVector V, FVector &A, const float T);
	UFUNCTION(BlueprintPure, category = FVector)
	static void FV_UVFromSAT(const FVector S, FVector &U, FVector &V, const FVector A, const float T);
	UFUNCTION(BlueprintPure, category = FVector)
	static void FV_STFromUVA(FVector &S, const FVector U, const FVector V, const FVector A, float &T);
	UFUNCTION(BlueprintPure, category = FVector)
	static void FV_SAFromUVT(FVector &S, const FVector U, const FVector V, FVector &A, const float T);
	UFUNCTION(BlueprintPure, category = FVector)
	static void FV_SVFromUAT(FVector &S, const FVector U, FVector &V, const FVector A, const float T);
	UFUNCTION(BlueprintPure, category = FVector)
	static void FV_SUFromVAT(FVector &S, FVector &U, const FVector V, const FVector A, const float T);
};
