#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Interfaces/RpySaveGame.h"
#include "Interfaces/RpyStateful.h"
#include "Rpy/RpyScript.h"
#include "Rpy/RpySession.h"

#include "RpyMainActor.generated.h"

UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API URpyMainActorState : public URpyStatefulActorState
{
    GENERATED_BODY()
public:
    void PostInitProperties() override
    {
        Super::PostInitProperties();

        StateName = "RpyMainActorState";
    }
    virtual void PrintDebug() const override
    {
        Super::PrintDebug();
        if (rpySessionState)
        {
            UE_LOG(LogTemp, Warning, TEXT("RpySessionState:"));
            rpySessionState->PrintDebug();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("RpySessionState: nullptr"));
        }
    }
    virtual void Serialize(FArchive &Ar) override
    {
        Super::Serialize(Ar);

        if (Ar.IsSaving())
        {
            bool bHasSession = (rpySessionState != nullptr);
            Ar << bHasSession;

            if (bHasSession)
            {
                // Serialize the class and its data manually
                FString SessionClassPath = rpySessionState->GetClass()->GetPathName();
                Ar << SessionClassPath;

                TArray<uint8> Bytes;
                FMemoryWriter MemoryWriter(Bytes, true);
                FObjectAndNameAsStringProxyArchive ProxyAr(MemoryWriter, true);
                ProxyAr.ArIsSaveGame = Ar.ArIsSaveGame;
                rpySessionState->Serialize(ProxyAr);
                Ar << Bytes;
            }
        }
        else if (Ar.IsLoading())
        {
            bool bHasSession = false;
            Ar << bHasSession;

            if (bHasSession)
            {
                FString SessionClassPath;
                Ar << SessionClassPath;
                UClass *SessionClass = LoadObject<UClass>(nullptr, *SessionClassPath);

                rpySessionState = NewObject<URpySessionState>(this, SessionClass);

                TArray<uint8> Bytes;
                Ar << Bytes;

                FMemoryReader MemoryReader(Bytes, true);
                FObjectAndNameAsStringProxyArchive ProxyAr(MemoryReader, true);
                ProxyAr.ArIsSaveGame = Ar.ArIsSaveGame;

                rpySessionState->Serialize(ProxyAr);
            }
        }
    }
    // rpy state
    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
    URpySessionState *rpySessionState;
};

UINTERFACE(Blueprintable)
class KISHIVISUALNOVEL_API URpyCloseVNEventListener : public URpyStatefulActor
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyCloseVNEventListener : public IRpyStatefulActor
{
    GENERATED_BODY()

public:
    // Event callback
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
    void OnCloseVNEvent();
    virtual void OnCloseVNEvent_Implementation() = 0;
};

UINTERFACE(Blueprintable)
class KISHIVISUALNOVEL_API URpyMainActor : public URpyStatefulActor
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyMainActor : public IRpyStatefulActor
{
    GENERATED_BODY()

public:
    // get State Class
    virtual TSubclassOf<URpyState> GetStateClass_Implementation() override
    {
        return URpyMainActorState::StaticClass();
    }
    // Check if VN is currently active
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
    bool IsActive() const;
    virtual bool IsActive_Implementation() const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
    URpySession *GetCurrentSession() const;
    virtual URpySession *GetCurrentSession_Implementation() const = 0;

    // Start a VN script
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
    void StartRpyScript(URpyScript *Script, FName const &Label);
    virtual void StartRpyScript_Implementation(URpyScript *Script, FName const &Label) = 0;

    // Close VN session
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
    void CloseVN();
    virtual void CloseVN_Implementation() = 0;

    // Add event listener
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
    void AddOnCloseVNEventListener(const TScriptInterface<IRpyCloseVNEventListener> &Listener);
    virtual void AddOnCloseVNEventListener_Implementation(const TScriptInterface<IRpyCloseVNEventListener> &Listener) = 0;
};
