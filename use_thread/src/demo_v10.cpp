// 传参中的悬空引用问题
void f(const std::string &) {}
// std::thread 的构造函数中调用了创建线程的函数（windows 下可能为 _beginthreadex）
// 它将我们传入的参数，f、buffer ，传递给这个函数，在新线程中执行函数 f
// 也就是说，调用和执行 f(buffer) 并不是说要在 std::thread 的构造函数中，而是在创建的新线程中，具体什么时候执行，取决于操作系统的调度
// 所以完全有可能函数 test 先执行完，而新线程此时还没有进行 f(buffer) 的调用，转换为std::string，那么 buffer 指针就悬空了，会导致问题
void test()
{
    char buffer[1024]{};
    // todo.. code
    // buffer 是一个数组对象，作为 std::thread 构造参数的传递的时候会decay-copy （确保实参在按值传递时会退化） 隐式转换为了指向这个数组的指针
    std::thread t{f, buffer}; // “启动线程”，而不是调用传递的可调用对象
    t.detach();
}
// 解决方案: 1: 将 detach() 替换为 join() 2: 显式将 buffer 转换为 std::string : std::thread t{ f,std::string(buffer) }

int main()
{
    // A的引用只能引用A，或者以任何形式转换到A
    double a = 1;
    const int &p = a; // a 隐式转换到了 int 类型，此转换为右值表达式
    const std::string &str = "123";
}