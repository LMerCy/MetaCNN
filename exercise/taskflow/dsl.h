#include "data.h"
template<typename F, typename T>
struct Edge
{
    using From = F;
    using To = T;
};

template<typename Node = void>
struct EdgeTrait
{
    template<typename Edge> struct IsFrom
    {
        constexpr static bool value = std::is_same_v<Node, typename Edge::From>;
    };
    template<typename Edge> struct IsTo
    {
        constexpr static bool value = std::is_same_v<Node, typename Edge::To>;
    };
    template<typename Edge> struct GetFrom
    {
        using type =typename Edge::From;
    };
    template<typename Edge> struct GetTo
    {
        using type = typename Edge::To;
    };
};

template<typename T, typename OUT = TypeList<>>
struct chain;

template<typename F, typename OUT>
struct chain<auto(*)(F)-> void, OUT>
{
    using From = F;
    using type = OUT;
};

template<typename F, typename T, typename OUT>
struct chain<auto(*)(F)->T, OUT>
{
private:
    using From = F;
    using To = chain<T,OUT>::From;
public:
    using type = typename chain<T,typename OUT::template append<Edge<From, To>>>::type;
};

template<typename ...Chains>
class Graph{
    using Edges = Fold_t<TypeList<typename chain<Chains>::type...>, TypeList<>,Concat>;
};

template<typename From, typename Target, typename PATH = TypeList<>, typename = void>
struct PathFinder;

template<typename Target, typename PATH>
struct PathFinder<Target,Target,PATH>:PATH::template append<Target>{};

template<typename Target,typename CurNode, typename PATH>
struct PathFinder<CurNode, Target, PATH, std::enable_if_t<Elem_v<PATH, CurNode>> >:TypeList<> {};

template<typename CURR_NODE, typename TARGET, typename PATH>
class PathFinder<CURR_NODE, TARGET, PATH,
    std::enable_if_t<! std::is_same_v<CURR_NODE, TARGET>
        && !Elem_v<PATH, CURR_NODE>>> { 
    using EdgesFrom = Filter_t<Edges, EdgeTrait<CURR_NODE>::template IsFrom>; 
    // for each (from, v) in edges: # expand next nodes
    using NextNodes = Map_t<EdgesFrom, EdgeTrait<>::GetTo>; 
    // cur_path = from + find_shortest_path(v, to)
    template<typename NEXT_NODE>
    struct GetPath: PathFinder<NEXT_NODE, TARGET,
        typename PATH::template append<CURR_NODE>> {};
    using AllPaths = Map_t<NextNodes, GetPath>; 
    template<typename ACC, typename Path> struct MinPath:
        std::conditional_t<(ACC::size == 0 ||
            ((ACC::size > Path::size) && Path::size > 0)), Path, ACC> {};
public:
    // path = min(path, cur_path)
    using type = Fold_t<AllPaths, TypeList<>, MinPath>; 
};