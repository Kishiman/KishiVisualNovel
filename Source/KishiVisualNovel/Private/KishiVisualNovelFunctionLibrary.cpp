#include "KishiVisualNovelFunctionLibrary.h"

#include "KishiVisualNovelScript.h"


// TArray<UVisualNovelScriptCompiler*> AllScriptNodeHandlers;
// TArray<UVisualNovelScriptCompiler*>& UVisualNovelFunctionLibrary::GetAllScriptCompilers()const{
//     return AllScriptNodeHandlers;
// };
int Fib(int n){
   if(n==0){
       return 1;
   }    
   if(n==1){
       return 1;
   }
    if(n==2){
       return 2;
   }
   int fib_half=Fib(n/2);
   int fib_half_1=Fib((n/2)-1);
   if(n%2==0){
       return fib_half*fib_half+fib_half_1*fib_half_1;
   }else{
       return ((fib_half+fib_half_1)*fib_half+fib_half_1*fib_half);
   }  
}
