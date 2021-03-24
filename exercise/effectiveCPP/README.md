[toc]
#1. 习惯c++

## 条款02 - 使用const,enums,template inline代替define

- 以const代替#define，一是可以提高编译时报错定位效率，二是define并不重视作用域，尤其是类内的变量。
- 使用template inline 去代替使用宏定义的函数。

## 条款03 - 尽可能使用const

- const默认作用于左边的东西，否则作用于其右边的东西。
- 令某个函数返回const，可以降低使用这个函数发生错误的可能性，比如operator*操作返回const，可以避免出现右值被使用的情况。
- 利用常量性不同进行重载，同时可以通过两次类型转换，使得non-const版本调用const版本，减少代码重复。

## 条款04 - 确定对象在被使用前已先被初始化

- 构造函数需要使用成员初值列进行初始化，而不是在构造函数内部进行赋值操作，后者会先调用各个成员变量的默认构造函数，再进行拷贝赋值。另外使用成员初值列还能避免很多其他潜在的问题，包括const或者references这种一定需要初值的类型。
- 手工初始化内置类型
- 使用local static 对象替换non-local static对象，就是单件模式。。。

#2. 构造/析构/赋值运算

## 条款05 - 了解c++默默编写并调用哪些函数

- 一共4个，copy构造函数，copy assignment 操作符，析构函数，另外在没有声明任何构造函数的情况下会生成默认构造函数。
- c++不允许“让reference改指向不同的对象”和“const成员不可更改”，所以对于类内含有reference和const成员的情况，编译器不会生成copy assignment.
- 基类的copy assignment是私有的话，编译器也不会为派生类生成copy assignment.

## 条款06 - 若不想使用编译器自动生成的函数，就该明确拒绝

- 使用privete的copy assignment和copy构造函数，避免被拷贝
- 继承uncopyable类，改积累实现了private的拷贝相关构造函数，且该类的析构函数不一定要是虚函数。
- 使用delete（原书是03版本，还没有delete）

## 条款07 - 为多态基类声明virtual析构函数

- 如题目所示，只有用于多态的基类，才需要将析构函数声明为virtual
- 如果类内有virtual函数，那么他必然要有virtual析构函数（原文是几乎确定有）。
- 不要去继承没有virtual析构函数的类，其实这一点也挺重要的，意味着==不要去继承stl容器之类的类==。
- 也不要为所有的类去声明一个virtual析构函数，因为virtual函数会带来虚表，从而带来更大的占用。
- ==纯虚函数也是可以提供定义的！！== 为什么要这么做，文中说有时候需要一个抽象class（啥情况啊。。。）

## 条款08 - 别让异常逃离析构函数

- 析构函数绝对不要吐出异常，因为若是有多个对象析构，会可能存在多个异常，而只要有两个异常同时存在，程序如果不结束运行就会有不明确行为。
- 如果一个析构函数调用的函数可能会抛出异常，析构函数应该捕获这个异常，然后吞下他们或者直接结束程序std::abort()【实际上这里说的就是用try catch做处理嘛...】
- 给用户提供一个可以处理异常函数的接口，同时封装的类的析构函数也调用这个接口。

## 条款09 - 绝不在构造和析构过程中调用virtual函数

- 如果在构造函数内调用虚函数，那么派生类在构造时，实际调用的会是基类的虚函数，这一行为有点违反认知。编译器可能会报警，也可能不会，但是如果有多层嵌套，编译器大部分时候不会报警。
- 那如果有类似创建时记录当前类的信息的需求改怎么做？由派生类的构造函数传递信息给基类，去做记录。

## 条款10 - 令operator=返回一个reference *this

- 所有和operator=相关的操作都应该返回reference *this，可以满足连锁赋值。（这是一个协议，标准库也都是这样做的）

## 条款11 - 在operator= 中处理“自我赋值”

- 这件事是这样的，对于一个需要自己进行资源管理的类，在进行赋值拷贝时，需要删除自身所占内存如:
    ```C++
    Wigdet& Wigdet::operator=(const Wigdet &rhs){
        delete pb;
        pb = new Bitmap(*rhs.pb);
        return *this;
    }
    ```
    但是这种做法会导致自我赋值的时候出现删除自身的情况，所以通常需要在最前面加上证同测试：
    ```C++
    if(this == &rhs) return *this;
    ```
    但是这时候还有个异常安全性的问题，new的时候如果空间不够，会抛出异常，导致没有拷贝成功的情况下删除了自身。不过有个好事是，如果我们考虑了异常安全性，可以同时避免自我拷贝安全性：
    ```C++
    Widget& Widget::operator=(const Widget &rhs){
        Bitmap* pOrig = pb;
        pb = new Bitmap(*rhs.pb);
        delete pOrig;
        return *this;
    }
    ```
    但是这个时候又有问题了。。。自我拷贝的时候效率不高，可以加上证同测试，但是加上证同测试会导致非自我拷贝时多一个循环判断，会影响很多的指令集判断，降低效率。所以是否要加证同测试，取决于业务实际情况。。
- 除了上面一套。。还可以使用copy and swap技术，具体见条款29吧。

## 条款12 复制对象时勿忘其每一个成分

- 派生类的拷贝函数不仅需要拷贝派生类自己的成员变量，还需要调用父类的拷贝函数来对父类的成员变量进行赋值。（否则基类自身会调用构造函数进行构造。）
- 同一个类不要用一个copy函数调用另一个copy函数。

# 3. 资源管理

## 条款13 以对象管理资源

- RAII就是获得一笔资源后于同一语句内以初始化某个管理对象，他们在构造函数中获得资源，在析构函数中释放资源。
- 典型RAII如shared_ptr。但是需要注意没有特别针对C++动态分配数组而设计的shared_ptr之类的东西，因为shared_ptr的析构函数做的是delete而不是delete[]（两者差别见条款16），另外我们也可以用vector来取代数组。

## 条款14 在资源管理类中小心copying行为

- RAII对象主要是管理heap-based资源，但是像Mutex之类的资源对象，则需要自定义的RAII对象，这里的意思是我们希望确保将一个被锁住(lock)的Mutex解锁（unlock）,而不是单纯创建和释放对象。但是这个时候需要对RAII对象的拷贝操作：
    - 禁止复制，将copying函数声明为private.
    - 使用shared_ptr保存Mutex *mutexPtr，但是当计数器为0的时候需要调用unlock，而不是释放资源。以上两点最终的代码如下：
    ```C++
    class Lock: private Uncopyable{
    public:
        explict Lock(Mutex* pm):mutexPtr(pm, unlock){//注意这里是显示的，没有隐示转换。
            lock(mutexPtr.get());
        }
        // 并不需要析构函数，因为mutexPtr不是static的成员变量，编译器会自动合成正确的析构函数。当引用计数器为0时，会自动调用unlock.
    private:
        std::shared_ptr<Mutex> mutexPtr;
    }
    ```
- 另外的拷贝行为还包括深度拷贝和移动。

## 条款15 在资源管理类中提供对原始资源的访问

- 例如shared_ptr可以通过get()获取到原始资源，且其重载了指针取值和指针操作符（operator-> 和operator*）
- 除了通过get()来获取资源，也可以通过显示转换或者隐式转换来实现，通常使用显示转换来保证安全性。

## 条款16 成对使用new和delete时要采取相同形式

- 当你在new表达式中使用[],必须在相应的delete表达式中使用[],反之亦然。

## 条款17 以独立语句讲newed对象置入智能指针

- shared_ptr的构造函数是显式的，不可以隐式转换
- 不可以写如下代码：
    ```C++
    prcocessWidget(std::tr1::shared_ptr<Widget>(new Widget), priority())
    ```
    因为首先new widget，调用shared_ptr构造函数，priority三者之间的调用关系是不固定的，若是priority()发生在shared_ptr构造函数调用前，且发送异常，那么new Widget返回的指针将会遗失，会导致资源泄露，所以我们要改写成这样：
    ```C++ 
    std::shared_ptr<Widget> pw(new Widget);
    processWidget(pw, priority());
    ```

## 条款18 让接口容易被正确使用，不易被误用

- 使用类来实现类似具名参数的作用，以减少赋值错误。
- 让类别容易使用的一种方式是让其尽量保持和内置类型一样的接口。
- 阻止无用的方法包括建立新类型（类似具名参数），限制类型上的操作，束缚对象值（如以const修饰operator*，防止出现a*b=c的赋值操作），消除客户的资源管理责任（shared_ptr）
- shared_ptr支持定制型删除器，可以防范cross-dll problem，也可被用来自动解除互斥锁。

## 条款19 设计class犹如设计type

过于复杂。。。

## 条款20 宁以pass-by-reference-to-const替换pass-by-value

- 尽量使用pass-by-reference-to-const替换pass-by-value，因为更高效，且能避免切割问题，切割问题是指如果函数的形参是基类，实参是派生类，以值传参的时候只会传入基类部分。
- 以上规则不适用于内置类型，stl的迭代器和函数对象，对于stl的迭代器，具体原因是：
    >One thing that comes to my mind and which is against the constness in the reference: the iterators need to be modified when using them.
    >
    >Another implementation detail may be that iterators are really just implemented as pointers. So are references in most cases. If you pass the pointer by value, you copy it once but dereference it only when needed. If, however, the iterator-pointer itself is passed by a reference-pointer, then that has to be dereferenced first, just in order to get to the iterator, and that must be done each time the iterator is accessed. That is superfluous.
- 对于自定义的struct有需要讨论的地方，对于确实很小的struct（构造不昂贵），不同的编译器可能会存在差别待遇，另外struct本身是有可能增长的，所以最好还是pass-by-reference.

## 条款21 必须返回对象时，别妄想返回其reference

- 绝不要返回pointer或者reference指向一个==local stack==对象，或返回reference指向一个==heap-allocated==对象，或返回pointer或reference指向一个local static对象而有可能同时需要多个这个对象。
    - 第一点实际上指向了一个会被销毁的临时对象
    - 第二点可能会在堆上分配一个无法被销毁的对象，如连乘的过程中产生的中间变量
    - 第三点是指如果同时需要多个这个对象，而由于其是static的，客户看到的都是同一个值。

## 条款22 将成员变量声明为private

- 将成员变量声明为private，好处多多:  
    - 赋予客户访问数据的一致性，不会出现obj.test和obj.test()的混用
    - 通过对获取变量的函数加上const或者修饰符，可以实现对变量的访问权限进行细粒度的控制。
    - 对客户不可见，class的作者使用起来弹性更高，因为public对象的修改，对整个客户的代码影响是巨大的。
    - protected变量修改对派生类的影响和public对象一样，所以其实对于封装来说只有两种情况，private-提供封装，和不提供封装。

## 条款23 宁以non-member、non-friend替换member函数

- 面向对象要求的封装性，要求尽可能少的函数可以直接访问private变量，所以如果一个函数可以通过另外的成员函数实现，就应该让这个函数是non-member，non-friend的。
- 使用non-member、non-friend函数的另一个好处是，我们可以把这类函数==单独放到一个新文件的相同namespace下，这样可以避免编译依赖，增加扩展性，这也是标准库的vector，algorithm做法==。另外其实使用派生类也能实现这个扩展性，但是并不是所有的类都可以作为base类。
- 背景知识：
    - friend函数对private成员的访问权利和member函数相同。
    - 一个class必须被完整定义，不能被切割成片片断断。

## 条款24 若所有参数皆需类型转换，请为此采用non-member函数

- 因为若不是non-member的函数，可能会出现将自定义类型转化为简单类型的情况，而一般自定义类型的构造函数提供了将简单类型转化为自定义类型的方式，而反过来是没有的。这种只需要将此函数写成non-member的，==就可以强制要求将简单类型都转化为自定义类型。==

## 条款25 考虑写出一个不抛出异常的swap函数

其实这个条款更多的在说如何为一个用户自定义的pimpl(pointer to implementations)提供一个高效的swap函数，而且尽可能好用。

- 首先要提供一个public的swap成员函数，能高效地交换底层指针（这个函数绝对不能抛出异常）
- 在pimpl的命名空间内提供一个non member的重载版本swap函数，来调用上一条提到的public函数。
- 假设这个pimpl是一个class(非class template)，还需要再std命名空间中添加一个全特化的std::swap，并用其调用class的public swap函数：
    - 这样做的目的是为了解决有些人调用swap的方式可能存在的错误，正确的调用方式应该是直接使用swap(obj1,obj2)，因为根据c++的==名称查找法则==，会优先找obj所在的命名空间的swap。但是有些人会写成std::swap(obj1,obj2),==为了让这种写法也可用==，才需要加一个std的全特化版本swap.
    - 其次这里要注意前提条件是这里的pimpl是一个class，因为对于class template，你无法为其提供一个全特化的swap，只能写一个偏特化的swap，而这个在c++中不被允许。
- 调用swap的时候正确的调用方式是：  
    ```c++
    using std::swap;
    swap(obj1,obj2);
    ```
- 最后需要强调的是成员版本的swap不可以抛出异常！而非成员版本的是可以的，这是因为非成员版本的以来的copy构造函数和copy assignment操作符，这两者都是可以抛出异常的。本质上成员版本的不抛出异常的原因是，其一般操作的都是内置类型，如常用的底层指针，而内置类型上的操作绝对不会抛出异常。

- 背景知识：  
    - 需要区分什么是重载，什么是全特化：   
        ```c++
        //这是全特化，只能对class使用。
        template<>
        void swap<Widget>(Widget &a, Widget &b){
            a.swap(b);
        }
        //这是重载，可以对class template使用。
        template<typename T>
        void swap(Widget<T>& a, Widget<T&> b){
            a.swap(b);
        }
        ```
    - 在std命名空间中进行std templates全特化是允许的，但是重载函数这种就会是对std全新的东西，是不被允许的。