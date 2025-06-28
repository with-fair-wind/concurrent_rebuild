// std::thread 转移所有权
// 线程对象管理线程资源
// std::thread 不可复制。两个 std::thread 对象不可表示一个线程，std::thread 对线程资源是独占所有权
// 移动操作可以将一个 std::thread 对象的线程资源所有权转移给另一个 std::thread 对象

std::thread f() {
    std::thread t{[] {}};
    return t;  // 自动调用了移动构造，重载决议 会选择到移动构造
    // return t 先调用了 t 的移动构造，将资源转移给函数返回值的 std::thread 对象
    // 此时 t 没有了线程资源，才开始正常析构
}

// 函数调用传参 : 本质上时初始化(构造)形参的对象
void g(std::thread t) { t.join(); }

int main() {
#if 0
    // 移动构造
    std::thread t{[]
                  {
                      std::this_thread::sleep_for(500ms);
                      std::cout << std::this_thread::get_id() << '\n';
                  }};
    std::cout << std::boolalpha << t.joinable() << '\n'; // 线程对象 t 当前关联了活跃线程 打印 true
    std::thread t2{std::move(t)};                        // 将 t 的线程资源的所有权移交给 t2
    std::cout << std::boolalpha << t.joinable() << '\n'; // 线程对象 t 当前没有关联活跃线程 打印 false
    // t.join(); // Error! t 没有线程资源
    t2.join(); // t2 当前持有线程资源
    std::cout << std::boolalpha << t2.joinable() << '\n';
#else
    // 移动赋值
    std::thread t;  // 默认构造，没有关联活跃线程
    std::cout << std::boolalpha << t.joinable() << '\n';  // false
    std::thread t2{[] {}};
    t = std::move(t2);  // 转移线程资源的所有权到 t
    std::cout << std::boolalpha << t.joinable() << '\n';  // true
    t.join();

    t2 = std::thread([] {});  // 临时对象是右值表达式，不用调用 std::move
    t2.join();
#endif

    // return t 重载决议选择到了移动构造，将 t 线程资源的所有权转移给函数调用 f() 返回的临时 std::thread 对象中
    // 这个临时对象再用来初始化 rt ，临时对象是右值表达式，这里一样选择到移动构造，将临时对象的线程资源所有权移交给 rt
    // 此时 rt 具有线程资源的所有权，由它调用 join() 正常析构
    // 如果标准达到 C++17，强制的复制消除（RVO）保证这里少一次移动构造的开销（临时对象初始化 rt 的这次）
    // 经自己测试后，发现两次移动的开销都被省略
    std::thread rt = f();
    rt.join();

    std::thread tmp{[] {}};
    g(std::move(tmp));
    g(std::thread{[] {}});
}