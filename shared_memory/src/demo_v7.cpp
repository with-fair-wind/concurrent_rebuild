// 不同作用域传递互斥量
// 互斥量满足互斥体 (Mutex)的要求，不可复制不可移动!!!
// 所谓的在不同作用域传递互斥量，其实只是传递了它们的指针或者引用罢了。可以利用各种类来进行传递，比如前面提到的 std::unique_lock

// 此时 X 也为不可复制不可移动
struct X
{
    // std::mutex m;
    // 解决方法:
    static inline std::mutex x;
    // std::mutex *m;
    // std::shared_ptr<std::mutex> m;
};

// std::unique_lock 是只能移动不可复制的类，它移动即标志其管理的互斥量的所有权转移了
// 有些时候，这种转移（就是调用移动构造）是自动发生的，比如当函数返回 std::unique_lock 对象。另一种情况就是得显式使用 std::move。

// 一种可能的使用是允许函数去锁住一个互斥量，并将互斥量的所有权转移到调用者上，所以调用者可以在这个锁保护的范围内执行代码

std::mutex some_mutex;

std::unique_lock<std::mutex> get_lock()
{
    extern std::mutex some_mutex;
    std::unique_lock<std::mutex> lk{some_mutex}; // 默认构造上锁
    return lk;                                   // 选择到 unique_lock 的移动构造，转移所有权
    // 转移所有权后 _Owns == false, 析构不会解锁，在调用方控制或析构解锁
}
void process_data()
{
    // 转移到主函数的 lk 中
    std::unique_lock<std::mutex> lk{get_lock()};
    // 执行一些任务...
} // 最后才会析构解锁

int main()
{
    X x;
    X x2{x};
    process_data();
}