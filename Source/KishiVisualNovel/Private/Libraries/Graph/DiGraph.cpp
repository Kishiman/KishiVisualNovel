#include "Libraries/Graph/DiGraph.h"
#include "Templates/KishiScriptInterface.h"
#include "Macros/Interface.h"
#include "Algo/Reverse.h"

/*
DefaultImplementation
*/

uint8 IDiGraph::GetInNodesSize_Implementation() const
{
    return GetInNodes_Implementation().Num();
};
uint8 UDiGraphImplementation::IGetInNodesSize_Default(const TScriptInterface<IDiGraph> &Target)
{
    return IDiGraph::Execute_GetInNodes(Target.GetObject()).Num();
}

uint8 IDiGraph::GetOutNodesSize_Implementation() const
{
    return GetOutNodes_Implementation().Num();
};
uint8 UDiGraphImplementation::IGetOutNodesSize_Default(const TScriptInterface<IDiGraph> &Target)
{
    return IDiGraph::Execute_GetOutNodes(Target.GetObject()).Num();
}

/*
Overrides Implementation
*/

TArray<TScriptInterface<IBaseGraph>> IDiGraph::GetNodes_Implementation() const
{
    TArray<TScriptInterface<IBaseGraph>> Nodes;
    TArray<TKishiScriptInterface<IBaseGraph>> InNodes(GetInNodes_Implementation());
    TArray<TKishiScriptInterface<IBaseGraph>> OutNodes(GetOutNodes_Implementation());
    Nodes.Append(std::move(InNodes));
    Nodes.Append(std::move(OutNodes));
    return Nodes;
};
TArray<TScriptInterface<IBaseGraph>> UDiGraphImplementation::IGetNodes_Default(const TScriptInterface<IDiGraph> &Target)
{
    TArray<TScriptInterface<IBaseGraph>> Nodes;
    TArray<TKishiScriptInterface<IBaseGraph>> InNodes(IDiGraph::Execute_GetInNodes(Target.GetObject()));
    TArray<TKishiScriptInterface<IBaseGraph>> OutNodes(IDiGraph::Execute_GetOutNodes(Target.GetObject()));
    Nodes.Append(std::move(InNodes));
    Nodes.Append(std::move(OutNodes));
    return Nodes;
}

uint8 IDiGraph::GetNodesSize_Implementation() const
{
    return GetOutNodesSize_Implementation() + GetInNodesSize_Implementation();
};
uint8 UDiGraphImplementation::IGetNodesSize_Default(const TScriptInterface<IDiGraph> &Target)
{
    return IDiGraph::Execute_GetOutNodesSize(Target.GetObject()) + IDiGraph::Execute_GetInNodesSize(Target.GetObject());
}

/*
Interface Proxy Functions
*/

TArray<TKishiScriptInterface<IDiGraph>> UDiGraphLibrary::IGetInNodes(const TKishiScriptInterface<IDiGraph> &Target)
{
    checkf(Target.GetObject() != NULL, TEXT("Error : Target is NULL or does not implement Interface IBaseGraph\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    TArray<TKishiScriptInterface<IDiGraph>> result(IDiGraph::Execute_GetInNodes(Target.GetObject()));
    return result;
}
TArray<TKishiScriptInterface<IDiGraph>> UDiGraphLibrary::IGetOutNodes(const TKishiScriptInterface<IDiGraph> &Target)
{
    checkf(Target.GetObject() != NULL, TEXT("Error : Target is NULL or does not implement Interface IBaseGraph\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    TArray<TKishiScriptInterface<IDiGraph>> result(IDiGraph::Execute_GetOutNodes(Target.GetObject()));
    return result;
}
uint8 UDiGraphLibrary::IGetInNodesSize(const TKishiScriptInterface<IDiGraph> &Target)
{
    checkf(Target.GetObject() != NULL, TEXT("Error : Target is NULL or does not implement Interface IBaseGraph\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    auto result = IDiGraph::Execute_GetInNodesSize(Target.GetObject());
    return result;
}
uint8 UDiGraphLibrary::IGetOutNodesSize(const TKishiScriptInterface<IDiGraph> &Target)
{
    checkf(Target.GetObject() != NULL, TEXT("Error : Target is NULL or does not implement Interface IBaseGraph\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    auto result = IDiGraph::Execute_GetOutNodesSize(Target.GetObject());
    return result;
}

/*
Library
*/