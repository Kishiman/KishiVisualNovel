#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Save/RpySaveGame.h"
#include "Session/RpyStateful.h"
#include "Script/RpyScript.h"
#include "Session/RpySession.h"

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
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyCloseVNEventListener|Hooks", meta = (ToolTip = "Optional: called when the VN this listener is registered with closes. The default does nothing."))
    void OnCloseVNEvent();
    virtual void OnCloseVNEvent_Implementation() {}
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

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyMainActor|Required", meta = (ToolTip = "Required: return whether a VN session is running on this actor."))
    bool IsActive() const;
    virtual bool IsActive_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyMainActor::IsActive")); return false; }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyMainActor|Required", meta = (ToolTip = "Required: return the session running on this actor, or None."))
    URpySession *GetCurrentSession() const;
    virtual URpySession *GetCurrentSession_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyMainActor::GetCurrentSession")); return nullptr; }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyMainActor|Required", meta = (ToolTip = "Required: start Script at Label, closing the running session first."))
    void StartRpyScript(URpyScript *Script, FName const &Label);
    virtual void StartRpyScript_Implementation(URpyScript *Script, FName const &Label) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyMainActor::StartRpyScript")); }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyMainActor|Required", meta = (ToolTip = "Required: close the running session and notify the close listeners."))
    void CloseVN();
    virtual void CloseVN_Implementation() { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyMainActor::CloseVN")); }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyMainActor|Required", meta = (ToolTip = "Required: register Listener to be notified when the VN closes."))
    void AddOnCloseVNEventListener(const TScriptInterface<IRpyCloseVNEventListener> &Listener);
    virtual void AddOnCloseVNEventListener_Implementation(const TScriptInterface<IRpyCloseVNEventListener> &Listener) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyMainActor::AddOnCloseVNEventListener")); }
};
