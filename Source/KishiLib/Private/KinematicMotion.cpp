// Fill out your copyright notice in the Description page of Project Settings.
#include "KinematicMotion.h"
#include "KishiLib.h"

void UKinematicMotion::ATFromSUV(const float S, const float U, const float V, float &A, float &T)
{
    TFKinematicMotion<float> motion(S, U, V, A, T);
    motion.SolveAccelerationTime();
    A = motion.GetA();
    T = motion.GetT();
};
void UKinematicMotion::VTFromSUA(const float S, const float U, float &V, const float A, float &T)
{
    TFKinematicMotion<float> motion(S, U, V, A, T);
    motion.SolveVelocityTime(false);
    V = motion.GetV();
    T = motion.GetT();
};
void UKinematicMotion::VAFromSUT(const float S, const float U, float &V, float &A, const float T)
{
    TFKinematicMotion<float> motion(S, U, V, A, T);
    motion.SolveVelocityAcceleration(false);
    V = motion.GetV();
    A = motion.GetA();
};
void UKinematicMotion::UTFromSVA(const float S, float &U, const float V, const float A, float &T)

{
    TFKinematicMotion<float> motion(S, U, V, A, T);
    motion.SolveVelocityTime();
    U = motion.GetU();
    T = motion.GetT();
};

void UKinematicMotion::UAFromSVT(const float S, float &U, const float V, float &A, const float T)
{
    TFKinematicMotion<float> motion(S, U, V, A, T);
    motion.SolveVelocityAcceleration();
    U = motion.GetU();
    A = motion.GetA();
};
void UKinematicMotion::UVFromSAT(const float S, float &U, float &V, const float A, const float T)
{
    TFKinematicMotion<float> motion(S, U, V, A, T);
    motion.SolveVelocityBoth();
    U = motion.GetU();
    V = motion.GetV();
};
void UKinematicMotion::STFromUVA(float &S, const float U, const float V, const float A, float &T)
{
    TFKinematicMotion<float> motion(S, U, V, A, T);
    motion.SolveDistanceTime();
    S = motion.GetS();
    T = motion.GetT();
};
void UKinematicMotion::SAFromUVT(float &S, const float U, const float V, float &A, const float T)
{
    TFKinematicMotion<float> motion(S, U, V, A, T);
    motion.SolveDistanceAcceleration();
    S = motion.GetS();
    A = motion.GetA();
};
void UKinematicMotion::SVFromUAT(float &S, const float U, float &V, const float A, const float T)
{
    TFKinematicMotion<float> motion(S, U, V, A, T);
    motion.SolveDistanceVelocity(false);
    S = motion.GetS();
    V = motion.GetV();
};
void UKinematicMotion::SUFromVAT(float &S, float &U, const float V, const float A, const float T)
{
    TFKinematicMotion<float> motion(S, U, V, A, T);
    motion.SolveDistanceVelocity();
    U = motion.GetU();
    S = motion.GetS();
};

void UKinematicMotion::FV_ATFromSUV(const FVector S, const FVector U, const FVector V, FVector &A, float &T){};
void UKinematicMotion::FV_VTFromSUA(const FVector S, const FVector U, FVector &V, const FVector A, float &T){};
void UKinematicMotion::FV_VAFromSUT(const FVector S, const FVector U, FVector &V, FVector &A, const float T){};
void UKinematicMotion::FV_UTFromSVA(const FVector S, FVector &U, const FVector V, const FVector A, float &T){};
void UKinematicMotion::FV_UAFromSVT(const FVector S, FVector &U, const FVector V, FVector &A, const float T){};
void UKinematicMotion::FV_UVFromSAT(const FVector S, FVector &U, FVector &V, const FVector A, const float T){};
void UKinematicMotion::FV_STFromUVA(FVector &S, const FVector U, const FVector V, const FVector A, float &T){};
void UKinematicMotion::FV_SAFromUVT(FVector &S, const FVector U, const FVector V, FVector &A, const float T){};
void UKinematicMotion::FV_SVFromUAT(FVector &S, const FVector U, FVector &V, const FVector A, const float T){};
void UKinematicMotion::FV_SUFromVAT(FVector &S, FVector &U, const FVector V, const FVector A, const float T){};