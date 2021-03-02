## c++反射

### 1. 计算结构体内字段的数量(编译期反射)
https://netcan.github.io/2020/08/01/%E5%A6%82%E4%BD%95%E4%BC%98%E9%9B%85%E7%9A%84%E5%AE%9E%E7%8E%B0C-%E7%BC%96%E8%AF%91%E6%9C%9F%E9%9D%99%E6%80%81%E5%8F%8D%E5%B0%84/

要明白上文链接中的计算结构体字段数量的实现，首先需要了解一些基础知识点：

1. 强制类型转换

    通过重载operaton()+模板实现任意类型转换。
    ```C++
    struct AnyType {
        template <typename T>
        operator T();
    };
    ```

2. std::void_t

    具体可以参考：https://blog.csdn.net/ding_yingzi/article/details/79983042

    可以配合模板特例化做一些类型判断，这里利用的是decltype(T::memeber)来做类型判断。

了解到上面两点之后，就能实现计算结构体中字段的数量：
```C++
struct AnyType {
    template <typename T>
    operator T();
};
template <typename T, typename = void, typename ...Ts>
struct CountMember {
    constexpr static size_t value = sizeof...(Ts) - 1;
};

template <typename T, typename ...Ts>
struct CountMember<T, std::void_t<decltype(T{Ts{}...}) >, Ts...> {
    
```

### 2. 结构体字段反射
https://bot-man-jl.github.io/articles/?post=2018/Cpp-Struct-Field-Reflection

要理解结构体字段反射，需要先理解如何去遍历tuple...这篇博客也讲得很清楚：https://www.cnblogs.com/happenlee/p/14219925.html


结构体字段的反射就是在此基础上进一步输出类型字段名

遍历tuple的代码：
```c++
template <typename Tuple, typename Func, size_t ... N>
void func_call_tuple(const Tuple& t, Func&& func, std::index_sequence<N...>) {
    static_cast<void>(std::initializer_list<int>{(func(std::get<N>(t)), 0)...});
}

template <typename ... Args, typename Func>
void travel_tuple(const std::tuple<Args...>& t, Func&& func) {
    func_call_tuple(t, std::forward<Func>(func), std::make_index_sequence<sizeof...(Args)>{});
}

int main() {
    auto t = std::make_tuple(1, 4.56, "happen lee");
    travel_tuple(t, [](auto&& item) {
        std::cout << item << ",";
    });
}
```
1. make_index_sequence
    具体实现可以参考https://blog.csdn.net/dboyguan/article/details/51706357
2. initializer_list
    具体使用参考：https://www.zhihu.com/question/443285720，链接中也提出了在c++17中可以用折叠表达式很方便的代替。
3. std::forward或者万能引用、右值引用等参考基本的语法书即可。