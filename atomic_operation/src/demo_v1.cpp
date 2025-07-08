// 原子操作即不可分割的操作。系统的所有线程，不可能观察到原子操作完成了一半
// 如果一个线程写入原子对象，同时另一线程从它读取，那么行为有良好定义
// 任何 std::atomic 类型，初始化不是原子操作
// 未定义行为优化(ub优化) : 优化会假设程序中没有未定义行为

#if 0
// 标准原子类型的实现通常包括一个 is_lock_free() 成员函数，允许用户查询特定原子类型的操作是否是通过直接的原子指令实现（返回 true），还是通过锁来实现（返回 false）
// 如果原子类型的内部使用互斥量实现，那么不可能有性能的提升

std::atomic<int> a = 10;
int main() {
    std::cout << std::boolalpha << a.is_lock_free() << std::endl;
    std::cout << a.load() << std::endl;
}
#else
// 在 C++17 中，所有原子类型都有一个 static constexpr 的数据成员 is_always_lock_free : 如果当前环境上的原子类型 X 是无锁类型，那么 X::is_always_lock_free 将返回 true
// 标准库还提供了一组宏 ATOMIC_xxx_LOCK_FREE ，在编译时对各种整数原子类型是否无锁进行判断
int main() {
    // 检查 std::atomic<int> 是否总是无锁
    if constexpr (std::atomic<int>::is_always_lock_free) {
        std::cout << "当前环境 std::atomic<int> 始终是无锁" << std::endl;
    } else {
        std::cout << "当前环境 std::atomic<int> 并不总是无锁" << std::endl;
    }

// 使用 ATOMIC_INT_LOCK_FREE 宏进行编译时检查
#if ATOMIC_INT_LOCK_FREE == 2
    std::cout << "int 类型的原子操作一定无锁的。" << std::endl;
#elif ATOMIC_INT_LOCK_FREE == 1
    std::cout << "int 类型的原子操作有时是无锁的。" << std::endl;
#else
    std::cout << "int 类型的原子操作一定有锁的。" << std::endl;
#endif
}
#endif
