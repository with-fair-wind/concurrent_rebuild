// 成员函数、lambda表达式作为可调用对象
struct X {
    void task_run(int &a) const { std::cout << &a << std::endl; }
};

int main() {
    X x;
    int n = 0;
    std::cout << &n << std::endl;
    // 成员指针必须和对象一起使用，这是唯一标准用法，成员指针不可以转换到函数指针单独使用，即使是非静态成员函数没有使用任何数据成员
    // 传入成员函数指针、与其配合使用的对象、调用成员函数的参数，构造线程对象 t，启动线程
    // std::thread t1{&X::task_run, &x, n};
    // &X::task_run 是一个整体，它们构成了成员指针，&类名::非静态成员
    // t1.join();

    // std::bind 也是默认按值复制的，即使我们的成员函数形参类型为引用
    std::thread t2{std::bind(&X::task_run, &x, n)};
    t2.join();

    std::thread t3{std::bind(&X::task_run, &x, std::ref(n))};
    t3.join();

    std::thread t4{[](auto i) { std::cout << i << std::endl; },
                   "kk"};  // C++14 泛型lambda
    t4.join();
}