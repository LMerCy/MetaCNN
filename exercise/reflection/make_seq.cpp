#include<iostream>
#include<tuple>
template<size_t... N>
decltype(auto) fun(std::index_sequence<N...>){
    return std::make_tuple(N...);
}
template<size_t... N>
struct index_seq{};

template<size_t... N>
decltype(auto) fun2(index_seq<N...>){
    return std::make_tuple(N...);
}
template<size_t N, size_t ...M>
struct make_index_seq: public make_index_seq<N - 1, N -1, M...> {};
 
template<size_t ...M>
struct make_index_seq<0, M...> : public index_seq<M...> {};

int main() {
    // auto t = fun(std::index_sequence<0,1,2,3>());
    // auto t = fun(std::make_index_seq<3>{});
    auto t = fun2(make_index_seq<3>{});
    std::cout << std::get<0>(t) << std::endl;
    std::cout << std::get<1>(t) << std::endl;
    std::cout << std::get<2>(t) << std::endl;
    return 0;
}

