namespace NSVarTypedict{
// Create_ demontrates how to create a container without type info of NullParamter with Constant length. 
// T is important, it contains the paramter that has already generated. At first time T is empty.
template<size_t N, template<typename...> class TCont, typename...T> 
struct Create_
{
    using type = typename Create_<N - 1, TCont, NullParamter, ...T>::type; // N changes, and T add a new NullParamter
};

template<template<typename...> class TCont, typename...T>
struct Create_<0, TCont, T...>
{
    using type = TCont<T...>;
};

// Tag2ID find TTag in TParameters...
template<typename TTag,size_t N,typename CurParameters, typename ... TParameters>
struct findPos
{
    constexpr static size_t value = typename findPos<TTag, N + 1, >::value;
};
template<typename TTag, size_t N, typename ... TParameters>
struct findPos<TTag, N, TTag, TParameters...>
{
    constexpr static size_t value = N;
};


template<typename TTag, typename ...TParameters>
constexpr static size_t Tag2ID = findPos<TTag,0,...TParameters>::value;

};

template<typename...TParameters>
struct VarTypeDict
{
    template<typename...TTypes>
    struct Values
    {
        Values() = default;
        Values(std::shared_ptr<void>(&&input)[sizeof...(TTypes)]){
            for(size_t i = 0; i < sizeof...(TTypes); ++i){
                m_tuple[i] = std::move(input[i]);
            }
        }
    public:
        template<typename TTag, typename TVal>
        auto Set(TVal && val) &&{
            using namespace NSVarTypedict;
            constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;
            //first, origin type and value should be replaced with new.
            //then the Values should be changed.
            using rawVal = std::decay_t<TVal>;
            rawVal* tmp = new rawVal(std::foward<TVal>(val)); // here change val to smart-ptr to be converted to void ptr later.
            m_tuple[TagPos] = std::shared_ptr<void>(tmp, [void* ptr]{
                rawVal * nPtr = static_cast<rawVal*>(ptr);// tmp already is rawVal*, why need converted here?
                delete nPtr;
            })
            // how about this? m_tuple[TagPos] = std::shared_ptr<void>(tmp)
        }
        template<typename TTag>
        const auto& Get() const;
    private:
        std::shared_ptr<void> m_tuple[sizeof...(TTypes)];// only used for save the value, no type
    };
public:
    static auto Create(){
        using namespace NSVarTypeDict;
        using type = typename Create_<sizeof...<TParameters>,Values>::type;
    }
}; 