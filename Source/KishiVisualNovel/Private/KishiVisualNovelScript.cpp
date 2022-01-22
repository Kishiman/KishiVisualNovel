
#include <string>
#include <regex>

using namespace std;

// FString UVisualNovelScriptNode::ToString()const{
//     FString out=FString::Printf(TEXT("Line:%d|Tabs:%d|path:"),this->ScriptLine.LineNumber,this->ScriptLine.tabs);
//     TArray<int> path=GetPath();
//     for (int i: path)
//     {
//         out.Appendf(TEXT("%d,"),i);
//     }
//     out+="|Line:";
//     out+=this->ScriptLine.line;
//     return out;
// }
// TArray<int> UVisualNovelScriptNode::GetPath()const{
//     TArray<int> path;
//     const UVisualNovelScriptNode* it=this;
//     while (it->Parent)
//     {
//         path.Add(it->Index);
//         it=it->Parent;
//     }
//     for (int i = 0; i < path.Num()/2; i++)
//     {
//         int tmp=path[i];
//         path[i]=path[path.Num()-i-1];
//         path[path.Num()-i-1]=tmp;
//     }
//     return path;
// }
// UVisualNovelScriptNode* UVisualNovelScriptNode::Next()const{
//     if (!Parent)
//     {
//         return nullptr;
//     }
//     if (Index==Parent->Children.Num()-1)
//     {
//         return Parent->Next();
//     }
//     return Parent->Children[Index+1];
// }

// void UVisualNovelScriptNode::AddChild(UVisualNovelScriptNode* child){
//     this->Children.Add(child);
//     child->Parent=this;
//     child->Index=this->Children.Num()-1;
// }


// // extern TArray<UVisualNovelScriptCompiler*> AllScriptNodeHandlers;
// UVisualNovelScriptCompiler::UVisualNovelScriptCompiler(){
//     // AllScriptNodeHandlers.Add(this);
// }
// bool UVisualNovelScriptCompiler::MatchScriptNode_Implementation(UVisualNovelScript* script,UVisualNovelScriptNode* node)const{

//     return false;
// }
// // void UVisualNovelScriptCompiler::Handle(UVisualNovelScript* script,UVisualNovelScriptNode* node){
// //     return;
// // }
// void UVisualNovelScript::ScriptTreeFromPyFines(TArray<FRpyLine>& rpyLines){
//     UVisualNovelScriptNode* TreeStack[16];
//     this->ScriptTree=NewObject<UVisualNovelScriptNode>(this);
//     TreeStack[0]=(this->ScriptTree);
//     for (FRpyLine rpyLine:rpyLines)
//     {
//         UVisualNovelScriptNode* newNode=NewObject<UVisualNovelScriptNode>(this);
//         newNode->ScriptLine=rpyLine;
//         TreeStack[rpyLine.tabs+1]=newNode;
//         TreeStack[rpyLine.tabs]->AddChild(newNode);
//     }
    
// }
    


