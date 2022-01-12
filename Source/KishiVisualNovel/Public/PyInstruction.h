#pragma once

#include "CoreMinimal.h"
#include "Libraries/ArrayTree.h"

#include "KishiVisualNovelGameMode.h"

#include "PyInstruction.generated.h"
UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE(FVNEvent);

UCLASS(abstract, Blueprintable)
class KISHIVISUALNOVEL_API PyInstruction : public UObject, public IBaseTree {
  GENERATED_BODY()
public:
//interface override
  virtual TScriptInterface<IBaseTree> GetParentTree_Implementation() const override;
  virtual TArray<TScriptInterface<IBaseTree>>  GetDirectChildren_Implementation() const override;
  virtual TScriptInterface<IArrayTree>  GetChild_Implementation(uint8 index) const override;
  virtual uint8 GetDirectChildrenSize_Implementation() const override;

  PyInstruction();

  //virtual methods
  virtual PyInstruction* GetNextInstruction(AKishiVisualNovelGameMode* gameMode) = 0;
  virtual TArray<FName> GetAwaitEvents() const;

protected:
  FPyLine pyLine;
  PyInstruction* parent;
  TArray<PyInstruction*> children;
private:
};