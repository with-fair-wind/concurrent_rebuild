struct func
{
    int &m_i;
    func(int &i) : m_i{i} {}
    void operator()(int n) const
    {
        for (int i = 0; i <= n; ++i)
            m_i += i; // 可能悬空引用
        std::cout << m_i << std::endl;
    }
};

// 必须是 std::thread 的 joinable() 为 true 即线程对象有活跃线程，才能调用 join() 和 detach()

int main()
{
#if 1
    std::thread thread{
        []
        { std::cout << "hello world" << std::endl; }};
    thread.detach(); // 不阻塞
    // 在单线程的代码中，对象销毁之后再去访问，会产生未定义行为，多线程增加了这个问题发生的几率
    // 比如函数结束，那么函数局部对象的生存期都已经结束了，都被销毁了，此时线程函数还持有函数局部对象的指针或引用
    // 线程对象放弃了对线程资源的所有权，不再管理此线程，也就是线程对象没有关联活跃线程了，此时 joinable 为 false
    // 允许此线程独立的运行，在线程退出时释放所有分配的资源。
    std::cout << std::boolalpha << thread.joinable() << std::endl;
    std::this_thread::sleep_for(10ms);
#else
    int n = 0;
    std::thread my_thread{func{n}, 100};
    my_thread.detach(); // 分离，不等待线程结束
    // 分离的线程可能还在运行
    // 一定要确保在主线程结束之前(或同时) 被detach的线程，要执行完毕
    // std::this_thread::sleep_for(10ms);
#endif
}