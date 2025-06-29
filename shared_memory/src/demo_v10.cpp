// std::recursive_mutex

// 线程对已经上锁的 std::mutex 再次上锁是错误的，这是未定义行为
// 在某些情况下，一个线程会尝试在释放一个互斥量前多次获取，所以提供了std::recursive_mutex

/*
std::recursive_mutex 允许同一线程多次锁定同一个互斥量，而不会造成死锁
当同一线程多次对同一个 std::recursive_mutex 进行锁定时，只有在解锁与锁定次数相匹配时，互斥量才会真正释放 lock 与 unlock 调用次数必须相同
但它并不影响不同线程对同一个互斥量进行锁定的情况。不同线程对同一个互斥量进行锁定时，会按照互斥量的规则进行阻塞
*/
std::recursive_mutex mtx;

void recursive_function(int count) {
#if 0
    // 递归函数，每次递归都会锁定互斥量
    mtx.lock();
    std::cout << "Locked by thread: " << std::this_thread::get_id() << ", count: " << count << std::endl;
    if (count > 0)
        recursive_function(count - 1); // 递归调用
    mtx.unlock(); // 解锁互斥量
#else
    // 同样也可以使用 std::lock_guard、std::unique_lock 帮助管理 std::recursive_mutex，而非显式调用 lock 与 unlock
    std::lock_guard<std::recursive_mutex> lc{mtx};
    std::cout << "Locked by thread: " << std::this_thread::get_id() << ", count: " << count << std::endl;
    if (count > 0) recursive_function(count - 1);
#endif
}

int main() {
    std::jthread t1(recursive_function, 3);
    std::jthread t2(recursive_function, 2);
}