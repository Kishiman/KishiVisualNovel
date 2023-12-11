#pragma once
#include "KishiMath.h"
#include <stdarg.h>

template <typename T>
class TVariable
{
    #define Order derivates.Num()
private:
    TArray<T> derivates;
public:

    TVariable()=default;
    TVariable(T* derivs, int count):derivates(derivs,count){}
    TVariable(TArray<T> derivs):derivates(derivs){}
    TVariable(T derivs, ... ){
        va_list list;
        va_start(list,derivs);
        for (int i = 0; i < derivs; i++)
        {
            derivates.Add(va_arg(list,T));
        }
    }

    T operator[](int index){
        if (index<0)
        {
            throw;
        }
        
         return GetDerivate(index);
        
        
    }
    int GetOrder(){
        return Order;
    }
    T GetDerivate(int index){
        if(index<0 || index>derivates.Num()){
            return T();
        }
        return derivates[index];
    }
    T GetPrimitive(int index,float dTime){
        if(index<0){
            return T();
        }
        T primitive=derivates[0];
        for (int i = 0; i < index; i++)
        {
            primitive*=dTime;
        }
        return primitive;
    }
    TVariable PropagateFrom(T value, int index, float dTime){
        T tmp=*this[index];
        derivates[index]=value;
        for (int i = index+1; i < derivates.Num(); i++)
        {
            T tmp2=derivates[i];
            derivates[i]=(derivates[i-1]-tmp)/dTime;
            tmp=tmp2;
        }
        for (int i = index - 1; i >= 0; i--)
        {
            derivates[i]+=(derivates[i+1]*dTime);
        }
    }
    TVariable &GetVariableDerivated(int index){
        TVariable clone=*(new TVariable(&(derivates[index], Order-index)));
        return clone;
    }
};
