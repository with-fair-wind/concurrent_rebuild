// std::atomic_flag : 可以在两个状态间切换：设置（true）和清除（false）
// 在 C++20 之前，std::atomic_flag 类型的对象需要以
// 在 C++20 中 std::atomic_flag 的默认构造函数保证对象为“清除”（false）状态，就不再需要使用 ATOMIC_FLAG_INIT

// ATOMIC_FLAG_INIT 在 MSVC STL 它只是一个 {}，在 libstdc++ 与 libc++ 它只是一个 { 0 }

/*
当标志对象已初始化，它只能做三件事情：销毁、清除、设置。这些操作对应的函数分别是：
1: clear() （清除）：将标志对象的状态原子地更改为清除（false）
2: test_and_set（测试并设置）：将标志对象的状态原子地更改为设置（true），并返回它先前保有的值。
3: 销毁：对象的生命周期结束时，自动调用析构函数进行销毁操作。
每个操作都可以指定内存顺序。clear() 是一个“读-改-写”操作，可以应用任何内存顺序。默认的内存顺序是 memory_order_seq_cst
*/

// 有限的特性使得 std::atomic_flag 非常适合用作制作自旋锁
// 自旋锁可以理解为一种忙等锁，因为它在等待锁的过程中不会主动放弃 CPU，而是持续检查锁的状态
// 与此相对，std::mutex 互斥量是一种睡眠锁。当线程请求锁（lock()）而未能获取时，它会放弃 CPU 时间片，让其他线程得以执行，从而有效利用系统资源。
// 从性能上看，自旋锁的响应更快，但是睡眠锁更加节省资源，高效。

class spinlock_mutex {
    std::atomic_flag flag{};

   public:
    spinlock_mutex() noexcept = default;
    void lock() noexcept { while (flag.test_and_set(std::memory_order_acquire)); }

    void unlock() noexcept { flag.clear(std::memory_order_release); }
};

spinlock_mutex m;

void f() {
    std::lock_guard<spinlock_mutex> lc{m};
    std::cout << "😅😅" << "❤️❤️\n";
}

int main() {
    std::vector<std::thread> vec;
    for (int i = 0; i < 5; ++i) {
        vec.emplace_back(f);
    }
    for (auto &t : vec) {
        t.join();
    }
}