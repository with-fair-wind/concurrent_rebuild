// 启动线程时可能存在其它数据成员还未初始化，导致未定义行为
/*
初始化的实际顺序如下：
1: 如果构造函数是最终派生类的，那么按基类声明的深度优先、从左到右的遍历中的出现顺序（从左到右指的>是基说明符列表中所呈现的顺序），初始化各个虚基类。
2: 然后，以在此类的基类说明符列表中出现的从左到右顺序，初始化各个直接基类。
3: 然后，以类定义中的声明顺序，初始化各个非静态成员。
4: 最后，执行构造函数体。
*/
struct X {
    X() {
        // 假设 X 的初始化没那么快
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::puts("X");
        v.resize(10, 6);
    }
    std::vector<int> v;
};

#define Type2
struct Test {
#ifdef Type1
#define Type1_1
#ifdef Type1_1
    Test()
        : t{&Test::f, this}  // 线程已经开始执行
    {}
#elif defined(Type1_2)
    Test() { t = std::thread{&Test::f, this}; }
#endif
#elif defined(Type2)
    Test() {}
    void start() { t = std::thread{&Test::f, this}; }
#endif

    ~Test() {
        if (t.joinable()) t.join();
    }
    void f() const {  // 如果在函数执行的线程 f 中使用 x 则会存在问题。使用了未初始化的数据成员 ub
        std::cout << "f\n";
        std::cout << x.v[5] << std::endl;
    }

#ifdef Type1
#ifdef Type1_1
    X x;
    std::thread t;
#elif defined(Type1_2)
    std::thread t;
    X x;
#endif
#elif defined(Type2)
    std::thread t;  // 声明顺序决定了初始化顺序，优先初始化 t
    X x;
#endif
};

int main() {
#ifdef Type1
    Test t;
#elif defined(Type2)
    Test t;
    t.start();  // 此方式不用管初始化顺序，因为此时初始化肯定已经全部完成了
#endif
}
