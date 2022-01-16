#include "Interfaces/KVNCharacter.h"
bool UKVNCharacterLibrary::HasExpression(const TScriptInterface<IKVNCharacter>& Target,FName Expression){
    TArray<FName> expressions=IKVNCharacter::Execute_GetSupportedExpressions(Target.GetObject());
    return expressions.Contains(Expression);
}