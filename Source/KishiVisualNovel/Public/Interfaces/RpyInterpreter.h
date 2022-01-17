#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interfaces/C++/RpyInstruction.h"

#include "RpyInterpreter.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyInterpreter : public UInterface
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyInterpreter
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    TScriptInterface<IRpyInstruction> GetCurrentInstruction() const;
    virtual TScriptInterface<IRpyInstruction> GetCurrentInstruction_Implementation() const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SetCurrentInstruction(const TScriptInterface<IRpyInstruction> &instruction);
    virtual bool SetCurrentInstruction_Implementation(const TScriptInterface<IRpyInstruction> &instruction) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SetLabel(FName label, const TScriptInterface<IRpyInstruction> &instruction);
    virtual bool SetLabel_Implementation(FName label, const TScriptInterface<IRpyInstruction> &instruction) = 0;

    // instructions
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Say(FName name, const FString &statement);
    virtual bool Say_Implementation(FName name, const FString &statement) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Scene(const TMap<FName, FString> &params);
    virtual bool Scene_Implementation(const TMap<FName, FString> &params) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Show(FName name, const TSet<FName> &params);
    virtual bool Show_Implementation(FName name, const TSet<FName> &params) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Hide(FName name, const TSet<FName> &params);
    virtual bool Hide_Implementation(FName name, const TSet<FName> &params) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool PlayMusic(FName name, const TMap<FName, FString> &params);
    virtual bool PlayMusic_Implementation(FName name, const TMap<FName, FString> &params) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Pause(float timeout);
    virtual bool Pause_Implementation(float timeout) = 0;


    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Menu(const TScriptInterface<IRpyInstruction> &menu, const TArray<FString> &choices);
    virtual bool Menu_Implementation(const TScriptInterface<IRpyInstruction> &menu, const TArray<FString> &choices) = 0;

    // getters
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetBool(FName name) const;
    virtual bool GetBool_Implementation(FName name) const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    int GetInt(FName name) const;
    virtual int GetInt_Implementation(FName name) const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetFloat(FName name) const;
    virtual float GetFloat_Implementation(FName name) const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FName GetName(FName name) const;
    virtual FName GetName_Implementation(FName name) const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FString GetString(FName name) const;
    virtual FString GetString_Implementation(FName name) const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FVector GetVector(FName name) const;
    virtual FVector GetVector_Implementation(FName name) const = 0;

    // setters

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SetBool(FName name, bool value);
    virtual bool SetBool_Implementation(FName name, bool value) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SetInt(FName name, int value);
    virtual bool SetInt_Implementation(FName name, int value) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SetFloat(FName name, float value);
    virtual bool SetFloat_Implementation(FName name, float value) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SetName(FName name, FName value);
    virtual bool SetName_Implementation(FName name, FName value) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SetString(FName name, const FString &value);
    virtual bool SetString_Implementation(FName name, const FString &value) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SetVector(FName name, FVector value);
    virtual bool SetVector_Implementation(FName name, FVector value) = 0;

    // Handlers Interface

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Choose(int idx);
    virtual void Choose_Implementation(int idx) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Next();
    virtual void Next_Implementation() = 0;
};

UCLASS()
class KISHIVISUALNOVEL_API URpyInterpreterLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
protected:
private:
};
