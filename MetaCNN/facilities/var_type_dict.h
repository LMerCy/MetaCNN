namespace MetaCNN{
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

// newTupleType
template<typename TVal, size_t N, size_t M,template<typename...>class TCont, typename... TPrcoessedTypes, typename CurTypes, typename... TRemainTypes>
struct NewTupleType_<TVal, N ,M , TCont<TPrcoessedTypes...>, CurTypes, TRemainTypes...>
{
    using type = typename NewTupleType_<TVal, N, M+1,TCont<TPrcoessedTypes,CurTypes>,TRemainTypes>::type;
};

template<typename TVal, size_t N,template<typename...> class TCont, typename... TProcessedTypes, typename CurTypes, typename... TRemainTypes>
struct NewTupleType_<TVal, N, N ,TCont<TProcessedTypes...>, CurTypes, TRemainTypes...>
{
    using type = TCont<TProcessedTypes,TVal, TRemainTypes...>;
};

template<typename TVal, size_t N, size_t M, typename TProcessedTypes, typename... TRemainTypes>
struct NewTupleType_;

template<typename TVal, size_t TagPos, typename TCont, typename... TRemainTypes> // Here initial TCont is Values<>, whose size of TPocessedTypes is 0
using NewTupleType = NewTupleType_<TVal, TagPos, 0, TCont, TRemainTypes>::type;

// pos2type
template<size_t TargetPos,size_t CurPos,typename CurType, typename... TTypes>
struct Pos2type_<TargetPos, CurPos,CurType,TTypes...>
{
    using type = typename Pos2type_<TargetPos, CurPos + 1, TTypes...>::type;
};

template<size_t TargetPos, typename CurType, typename... TTypes>
struct Pos2type_<TargetPos, TargetPos, CurType, TTypes...>
{
    using type = CurType;
};

template<size_t TargetPos, typename...TTypes>
using Pos2type = typename Pos2type_<TargetPos,0,TTypes...>::type;

};

template<typename...TParameters> // TParameters contains the Key of Values
struct VarTypeDict
{
    template<typename...TTypes>// TType contains the Type of Values
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
            constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;// 1.TPrameters represent "Key"
            //first, origin type and value should be replaced with new.
            //then the Values should be changed.
            using rawVal = std::decay_t<TVal>;
            rawVal* tmp = new rawVal(std::foward<TVal>(val)); // here change val to smart-ptr to be converted to void ptr later.
            m_tuple[TagPos] = std::shared_ptr<void>(tmp, [void* ptr]{
                rawVal * nPtr = static_cast<rawVal*>(ptr);// tmp already is rawVal*, why need converted here?
                delete nPtr;
            })
            // how about this? m_tuple[TagPos] = std::shared_ptr<void>(tmp)
            using new_type = NewTupleType<rawVal, TagPos, Values<>, TTypes...>; // 2.TTypes represent "Type"
            return new_type(std::move(m_tuple)); // 3.m_tuple saves "value"
        }
        template<typename TTag>
        const auto& Get() const{
            using namespace NSVarTypedict;
            constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;
            using AimType = Pos2type<TagPos,TTypes...>;
            void* temp = m_tuple[TagPos].get();
            AimType* res= static_cast<AimType *>(temp);
            return *res;
        };
    private:
        std::shared_ptr<void> m_tuple[sizeof...(TTypes)];// only used for save the value, no type
    };
public:
    static auto Create(){
        using namespace NSVarTypeDict;
        using type = typename Create_<sizeof...<TParameters>,Values>::type;
    }
}; 
}