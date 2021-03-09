#include "var_type_dict.h"
#include<iostream>
// #include<MetaCNN/facilities/var_type_dict.h>
using namespace MetaCNN;
struct A;
struct B; 
struct Weight;

struct FParams : public VarTypeDict<struct A, struct B, struct Weight>{};
template<typename Tin>
auto fun(const Tin& in){
    auto a = in.template Get<A>();
    auto b = in.template Get<B>();
    auto weight = in.template Get<Weight>();
    return a*weight + b*(1-weight);
}
void test_var_type_dict(){
    std::cout << fun(FParams::Create()
                    .Set<A>(1.3f)
                    .Set<B>(2.4f)
                    .Set<Weight>(0.1f)) << std::endl;
    return;
}
int main(){
    test_var_type_dict();
    return 0;
}