#include "Libraries/Graph/BaseGraph.h"
#include "Templates/KishiScriptInterface.h"
#include "Macros/Interface.h"
#include "Algo/Reverse.h"



TArray<TScriptInterface<IBaseGraph>> UBaseGraphLibrary::IGetNodes(TKishiScriptInterface<IBaseGraph> Target)
{
    checkf(Target.GetObject() != NULL, TEXT("Error : Target is NULL or does not implement Interface IBaseGraph\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    checkCode(
        auto nodes = IBaseGraph::Execute_GetNodes(Target.GetObject());
        for (auto &&node : nodes)
        {
            auto OtherNodes=IBaseGraph::Execute_GetNodes(node.GetObject());
            checkf(OtherNodes.Contains(Target), TEXT("Error : Broken Tree, node not connected to target"));
        }
    );
    auto nodes = IBaseGraph::Execute_GetNodes(Target.GetObject());
    return nodes;
}

uint8 UBaseGraphLibrary::IGetNodesSize(TKishiScriptInterface<IBaseGraph> Target)
{
    checkf(Target.GetObject() != NULL, TEXT("Error : Target is NULL or does not implement Interface IBaseGraph\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    uint8 nodesSize = IBaseGraph::Execute_GetNodesSize(Target.GetObject());
    checkCode(
        auto nodes = IGetNodes(Target);
        checkf(nodesSize == nodes.Num(), TEXT("Error : Broken Tree, nodes size don't match nodes returned"));
        );
    return nodesSize;
}

uint8 UBaseGraphLibrary::IGetNodesSize_Default(const TScriptInterface<IBaseGraph> &Target)
{
    auto nodes = IGetNodes(Target);
    return nodes.Num();
}