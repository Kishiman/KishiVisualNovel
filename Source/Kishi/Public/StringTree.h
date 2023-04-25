#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StringTree.generated.h"




UINTERFACE()
class KISHI_API UStringTree : public UInterface
{
    GENERATED_BODY()
};
class KISHI_API IStringTree
{
    GENERATED_BODY()
public:

    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    FString GetName()const;
    virtual FString GetName_Implementation()const=0;

    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    void SetName(const FString& Value);
    virtual void SetName_Implementation(const FString& Value)=0;

    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    TScriptInterface<IStringTree> GetParentTree()const;
    virtual TScriptInterface<IStringTree> GetParentTree_Implementation()const=0;

    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    TArray<TScriptInterface<IStringTree>> GetChildren()const;
    virtual TArray<TScriptInterface<IStringTree>> GetChildren_Implementation()const=0;

    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    TScriptInterface<IStringTree> GetChild(const FString& Name)const;
    virtual TScriptInterface<IStringTree> GetChild_Implementation(const FString& Name)const=0;

    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    bool AddChild(const TScriptInterface<IStringTree>& ChildToAdd);
    virtual bool AddChild_Implementation(const TScriptInterface<IStringTree>& ChildToAdd)=0;

    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    TScriptInterface<IStringTree> AddNewChild(const FString& Name);
    virtual TScriptInterface<IStringTree> AddNewChild_Implementation(const FString& Name)=0;

    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    bool RemoveChild(const TScriptInterface<IStringTree>& ChildToRemove);
    virtual bool RemoveChild_Implementation(const TScriptInterface<IStringTree>& ChildToRemove)=0;
};
#define ISTRINGTREE(Object,Method)          IStringTree::Execute_##Method(Object.GetObject())
#define ISTRINGTREE_1(Object,Method,A)      IStringTree::Execute_##Method(Object.GetObject(),A)
UCLASS()
class KISHI_API UStringTreeLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure)
    static bool IsLeaf(const TScriptInterface<IStringTree>& Target);
    UFUNCTION(BlueprintPure)
    static bool IsRoot(const TScriptInterface<IStringTree>& Target);

    UFUNCTION(BlueprintPure)
    static bool IsDirectParentOf(const TScriptInterface<IStringTree>& Target,const TScriptInterface<IStringTree>& Child);
    UFUNCTION(BlueprintPure)
    static bool IsDirectChildOf(const TScriptInterface<IStringTree>& Target,const TScriptInterface<IStringTree>& Parent);

    UFUNCTION(BlueprintCallable)
    static bool AddChildAtName(const TScriptInterface<IStringTree>& Target,const TScriptInterface<IStringTree>& Child,const FString& Name);  
    UFUNCTION(BlueprintCallable)
    static TScriptInterface<IStringTree> ForceChild(const TScriptInterface<IStringTree>& Target,const FString& Name);
    UFUNCTION(BlueprintCallable)
    static bool RemoveChild(const TScriptInterface<IStringTree>& InTarget,const FString& Name);



    //SubChildrenLibrary
    UFUNCTION(BlueprintPure)
    static FString GetPath(const TScriptInterface<IStringTree>& Target);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IStringTree> GetRootParent(const TScriptInterface<IStringTree>& Target);
    UFUNCTION(BlueprintPure)
    static TArray<TScriptInterface<IStringTree>> GetParentsFromRoot(const TScriptInterface<IStringTree>& Target);

    UFUNCTION(BlueprintPure)
    static TScriptInterface<IStringTree> GetChildAt(const TScriptInterface<IStringTree>& Target,FString Path); 
    UFUNCTION(BlueprintPure)
    static bool IsParentOf(const TScriptInterface<IStringTree>& Target,const TScriptInterface<IStringTree>& Child,FString& OutPathP2C);
    UFUNCTION(BlueprintPure)
    static bool IsChildOf(const TScriptInterface<IStringTree>& Target,const TScriptInterface<IStringTree>& Parent,FString& OutPathP2C);
    UFUNCTION(BlueprintPure)
    static TArray<TScriptInterface<IStringTree>> GetChildrenUnder(const TScriptInterface<IStringTree>& Target,FString Path);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IStringTree> FindAnyChildByName(const TScriptInterface<IStringTree>& Target,const FString& Name,FString& OutPath);
    UFUNCTION(BlueprintCallable)
    static bool AddChildAt(const TScriptInterface<IStringTree>& InTarget,FString Path,const TScriptInterface<IStringTree>& InChild);
    UFUNCTION(BlueprintCallable)
    static bool AddChildUnder(const TScriptInterface<IStringTree>& InTarget,FString Path,const TScriptInterface<IStringTree>& InChild);
    UFUNCTION(BlueprintCallable)
    static TScriptInterface<IStringTree> AddNewChildAt(const TScriptInterface<IStringTree>& InTarget,FString Path);
    UFUNCTION(BlueprintCallable)
    static TScriptInterface<IStringTree> AddNewChildUnder(const TScriptInterface<IStringTree>& InTarget,FString Path,const FString& Name);

    UFUNCTION(BlueprintCallable)
    static TScriptInterface<IStringTree> ForceChildAt(const TScriptInterface<IStringTree>& InTarget,FString Path);
    UFUNCTION(BlueprintCallable)
    static TScriptInterface<IStringTree> ForceChildUnder(const TScriptInterface<IStringTree>& InTarget,FString Path,const FString& Name);

    UFUNCTION(BlueprintCallable)
    static bool RemoveChildAt(const TScriptInterface<IStringTree>& InTarget,FString Path);
protected:
private:
};