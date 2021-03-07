## 条款02 - 使用const,enums,template inline代替define

- 以const代替#define，一是可以提高编译时报错定位效率，二是define并不重视作用域，尤其是类内的变量。
- 使用template inline 去代替使用宏定义的函数。

## 条款03 - 尽可能使用const

- const默认作用于左边的东西，否则作用于其右边的东西。
- 令某个函数返回const，可以降低使用这个函数发生错误的可能性，比如operator*操作返回const，可以避免出现右值被使用的情况。
- 利用常量性不同进行重载，同时可以通过两次类型转换，使得non-const版本调用const版本，减少代码重复。