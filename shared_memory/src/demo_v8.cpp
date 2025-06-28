// 保护共享数据的"初始化"过程
// 对于共享数据的"初始化"过程的保护: 通常就不会用互斥量，这会造成很多的额外开销
/*
1: 双检锁（错误）需使用原子变量，参考设计模式中单例模式中的懒汉模式
2: 使用 std::call_once
3: 静态局部变量初始化从 C++11 开始是线程安全
*/

// 标准库提供了 std::call_once 和 std::once_flag
// 比起锁住互斥量并显式检查指针，每个线程只需要使用 std::call_once 就可以
struct some {
    void do_something() {}
};

std::shared_ptr<some> ptr;
std::once_flag resource_flag;

void init_resource() { ptr.reset(new some); }

void foo() {
    std::call_once(resource_flag, init_resource);  // 线程安全的一次初始化
    ptr->do_something();
}

void test() {
    std::call_once(resource_flag, [] { std::cout << "f init\n"; });
}

// “初始化”，那自然是只有一次。但是 std::call_once 也有一些例外情况（比如异常）会让传入的可调用对象被多次调用，即“多次”初始化：
std::once_flag flag;
int n = 0;

void f() {
    std::call_once(flag, [] {
        ++n;
        std::cout << "第" << n << "次调用\n";
        throw std::runtime_error("异常");
    });
}

class my_class {};
inline my_class &get_my_class_instance() {
    static my_class instance = (std::cout << "get_my_class_instance\n", my_class{});  // 线程安全的初始化过程 初始化严格发生一次
    return instance;
}

int main() {
// 编译器有问题
#if 1
#if 0
    // https://godbolt.org/z/rrndqYsbc
    test();
    test();
    test();
#else
    // https://godbolt.org/z/Gbd6Kfsnq
    try {
        f();
    } catch (std::exception &) {
    }

    try {
        f();
    } catch (std::exception &) {
    }
#endif
#endif
    get_my_class_instance();
    get_my_class_instance();
    get_my_class_instance();
}
