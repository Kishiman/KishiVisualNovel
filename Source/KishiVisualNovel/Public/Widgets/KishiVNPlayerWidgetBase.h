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

/**
 * Abstract native implementation of IRpyScriptInterpreter for the VN player widget.
 * Create a child Widget Blueprint from this class. The interpreter interface is implemented
 * here; customize the player through the Blueprint events below instead of implementing it again.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "VN Player Interpreter Base"))
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
	virtual void SetSession_Implementation(URpySession *InSession) override;
	virtual URpySession *GetSession_Implementation() const override { return Session; }
	virtual TScriptInterface<IRpyAudioManager> GetAudioManager_Implementation() const override;
	virtual TScriptInterface<IRpyChoiceManager> GetChoiceManager_Implementation() const override;
	virtual TScriptInterface<IRpySceneManager> GetSceneManager_Implementation() const override;
	virtual TScriptInterface<IRpyShowManager> GetShowManager_Implementation() const override;
	virtual TScriptInterface<IRpyStatementManager> GetStatementManager_Implementation() const override;
	virtual void HandleInstructionComplete_Implementation() override;
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) override;
	virtual void OnScriptComplete_Implementation() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Menus")
	UUserWidget *SaveMenu = nullptr;
	// Cached from the widget tree by interface, so manager widget names do not matter.
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rpy|Managers")
	UUserWidget *AudioManager = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rpy|Managers")
	UUserWidget *ChoiceManager = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rpy|Managers")
	UUserWidget *SceneManager = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rpy|Managers")
	UUserWidget *ShowManager = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rpy|Managers")
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

	/** Child Blueprint hook called after the native interpreter starts. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi VN Player Widget Base|Hooks")
	void OnInterpreterStarted();
	/** Child Blueprint hook called after the native interpreter shuts down. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi VN Player Widget Base|Hooks")
	void OnInterpreterShutdown();
	/** Child Blueprint hook called after an instruction completes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi VN Player Widget Base|Hooks")
	void OnInstructionCompleted();
	/** Child Blueprint hook for dialogue viseme updates. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi VN Player Widget Base|Hooks")
	void OnViseme(FName Name, EMouthViseme Viseme, float TimeOffset);
	/** Child Blueprint hook called when the VN script completes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi VN Player Widget Base|Hooks")
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
	void DiscoverManagers(bool bWarnOnMissing);
	void WireManagers();
};
