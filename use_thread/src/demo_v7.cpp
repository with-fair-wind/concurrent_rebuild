// 传递参数
struct X {
    operator int() const {
        std::cout << __func__ << std::endl;
        return 0;
    }
};

void f(const int &a) { std::cout << "&a: " << &a << std::endl; }

int main() {
    int n = 1;
    std::cout << "&n: " << &n << std::endl;
    // 传参会复制到新线程的内存空间中，即使函数中的参数是引用，依然实际是复制
    // 内部实现:会先进行 decay 衰退，通过复制/移动构造传递给tuple，最后通过std::move传递给 invoke
    std::thread t{f, n};

    // void f(int &) 如果不使用 std::ref 会产生编译错误
    // std::thread 内部会将保有的参数副本转换为右值表达式进行传递，这是为了那些只支持移动的类型，左值引用没办法引用右值表达式，产生编译错误

    // std::ref 返回一个包装类 std::reference_wrapper
    // 这个类就是包装引用对象类模板，将对象包装，可以隐式转换为被包装对象的引用。
    // 该隐式转换由 std::reference_wrapper 内部定义的转换函数实现
    std::thread t1{f, std::ref(n)};
    t.join();
    t1.join();

    std::reference_wrapper<int> r = std::ref(n);
    int &p = r;  // r 隐式转换为 n 的引用 此时 p 引用的就是 n

    std::reference_wrapper<const int> cr = std::cref(n);
    const int &cp = cr;  // r 隐式转换为 n 的 const 的引用 此时 p 引用的就是 n

    std::cout << "&p: " << &p << "\t&r: " << &r << std::endl;
    std::cout << "&cp: " << &cp << "\t&cr: " << &cr << std::endl;

    X x;
    int a = x;  // 自动调用定义的转化函数
}