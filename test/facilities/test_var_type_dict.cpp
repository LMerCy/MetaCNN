#include "var_type_dict.h"
#include<iostream>
#include<MetaCNN/facilities/var_type_dict.h>
using namespace MetaCNN;
namespace{
// struct A;struct B; struct Weight;
struct FParams : public VarTypeDict<struct A, struct B, struct Weight>{};
template<typename Tin>
auto fun(const Tin& in){
    auto a = in.template get<A>();
    auto b = in.template get<B>();
    auto weight = in.template get<Weight>();
    return a*weight + b*(1-weight);
}
}
void test_var_type_dict(){
    std::cout << fun(FParams::create()
                    .set<A>(1.3f)
                    .set<B>(2.4f)
                    .set<Weight>(0.1f)) << std::endl;
    return;
}
int main(){
    test_var_type_dict();
    return 0;
}