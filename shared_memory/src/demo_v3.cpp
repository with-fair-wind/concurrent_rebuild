// try_lock
std::mutex mtx;

void thread_function(int id)
{
    // 尝试加锁
    if (mtx.try_lock())
    {
        std::osyncstream{std::cout} << "线程：" << id << " 获得锁" << std::endl;
        // 临界区代码
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟临界区操作
        mtx.unlock();                                                // 解锁
        std::osyncstream{std::cout} << "线程：" << id << " 释放锁" << std::endl;
    }
    else
    {
        std::osyncstream{std::cout} << "线程：" << id << " 获取锁失败 处理步骤" << std::endl;
    }
}

int main()
{
    std::thread t1(thread_function, 1);
    std::thread t2(thread_function, 2);

    t1.join();
    t2.join();
}