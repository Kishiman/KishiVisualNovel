#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Interfaces/ScriptInterpreterManagers/RpyAudioManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyChoiceManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpySceneManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyShowManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyStatementManager.h"
#include "KishiVNPlayerWidgetBase.generated.h"

/** C++ implementation of the interpreter contract used by WGT_VNPlayer. */
UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API UKishiVNPlayerWidgetBase : public UUserWidget, public IRpyScriptInterpreter
{
	GENERATED_BODY()

public:
	virtual void StartUp_Implementation() override;
	virtual void Shutdown_Implementation() override;
	virtual bool getSaveMenuVisible_Implementation() const override { return bSaveMenuVisible; }
	virtual void setSaveMenuVisible_Implementation(bool bVisible) const override;
	virtual bool getLoadMenuVisible_Implementation() const override { return bLoadMenuVisible; }
	virtual void setLoadMenuVisible_Implementation(bool bVisible) const override;
	virtual bool getConfigMenuVisible_Implementation() const override { return bConfigMenuVisible; }
	virtual void setConfigMenuVisible_Implementation(bool bVisible) const override;
	virtual bool getBackLogMenuVisible_Implementation() const override { return bBackLogMenuVisible; }
	virtual void setBackLogMenuVisible_Implementation(bool bVisible) const override;
	virtual URpySession *GetSession_Implementation() const override { return Session; }
	virtual TScriptInterface<IRpyAudioManager> GetAudioManager_Implementation() const override;
	virtual TScriptInterface<IRpyChoiceManager> GetChoiceManager_Implementation() const override;
	virtual TScriptInterface<IRpySceneManager> GetSceneManager_Implementation() const override;
	virtual TScriptInterface<IRpyShowManager> GetShowManager_Implementation() const override;
	virtual TScriptInterface<IRpyStatementManager> GetStatementManager_Implementation() const override;
	virtual void OnInstructionComplete_Implementation(EInstructionRunTimeType Type) override;
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) override;
	virtual void OnScriptComplete_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Rpy")
	void SetSession(URpySession *InSession);

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Menus")
	UUserWidget *SaveMenu = nullptr;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Managers")
	UUserWidget *AudioManager = nullptr;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Managers")
	UUserWidget *ChoiceManager = nullptr;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Managers")
	UUserWidget *SceneManager = nullptr;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Managers")
	UUserWidget *ShowManager = nullptr;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Managers")
	UUserWidget *StatementManager = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Rpy")
	URpySession *Session = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Rpy")
	bool bStarted = false;
	UPROPERTY(BlueprintReadWrite, Category = "Rpy|Menus")
	mutable bool bSaveMenuVisible = false;
	UPROPERTY(BlueprintReadWrite, Category = "Rpy|Menus")
	mutable bool bLoadMenuVisible = false;
	UPROPERTY(BlueprintReadWrite, Category = "Rpy|Menus")
	mutable bool bConfigMenuVisible = false;
	UPROPERTY(BlueprintReadWrite, Category = "Rpy|Menus")
	mutable bool bBackLogMenuVisible = false;

	UFUNCTION(BlueprintImplementableEvent, Category = "Rpy")
	void OnInterpreterStarted();
	UFUNCTION(BlueprintImplementableEvent, Category = "Rpy")
	void OnInterpreterShutdown();
	UFUNCTION(BlueprintImplementableEvent, Category = "Rpy")
	void OnInstructionCompleted(EInstructionRunTimeType Type);
	UFUNCTION(BlueprintImplementableEvent, Category = "Rpy")
	void OnViseme(FName Name, EMouthViseme Viseme, float TimeOffset);
	UFUNCTION(BlueprintImplementableEvent, Category = "Rpy")
	void OnVNScriptCompleted();
	UFUNCTION(BlueprintImplementableEvent, Category = "Rpy|Menus")
	void OnSaveMenuVisibilityChanged(bool bVisible);
	UFUNCTION(BlueprintImplementableEvent, Category = "Rpy|Menus")
	void OnLoadMenuVisibilityChanged(bool bVisible);
	UFUNCTION(BlueprintImplementableEvent, Category = "Rpy|Menus")
	void OnConfigMenuVisibilityChanged(bool bVisible);
	UFUNCTION(BlueprintImplementableEvent, Category = "Rpy|Menus")
	void OnBackLogMenuVisibilityChanged(bool bVisible);
	virtual void NativeOnInitialized() override;
	void WireManagers();
};
