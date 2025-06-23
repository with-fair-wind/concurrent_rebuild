// RAII
struct func
{
    int &m_i;
    func(int &i) : m_i{i} {}
    void operator()(int n) const
    {
        for (int i = 0; i <= n; ++i)
            m_i += i; // 可能悬空引用
        std::this_thread::sleep_for(1s);
        std::cout << m_i << std::endl;
    }
};

void f2() { throw std::runtime_error("test f2()"); }

// 严格来说其实thread_guard也不算 RAII，因为 thread_guard 的构造函数其实并没有申请资源，只是保有了线程对象的引用，在析构的时候进行了 join() 。
class thread_guard
{
public:
    explicit thread_guard(std::thread &t) : thread_(t) {}
    ~thread_guard()
    {
        std::puts("析构");
        if (thread_.joinable())
            thread_.join();
    }

    thread_guard &operator=(const thread_guard &) = delete;
    thread_guard(const thread_guard &) = delete;

private:
    std::thread &thread_;
};

// 函数 f 执行完毕，局部对象就要逆序销毁了。因此，thread_guard 对象 g 是第一个被销毁的，调用析构函数。
// 即使函数 f2() 抛出了一个异常，这个销毁依然会发生 !!!（前提是你捕获了这个异常）
// 这确保了线程对象 t 所关联的线程正常的执行完毕以及线程对象的正常析构
void f()
{
    int n = 0;
    std::thread t{func(n), 10};
    thread_guard g(t);
    f2();
}

int main()
{
#if 0
    f(); // 如果异常被抛出但未被捕获那么就会调用 std::terminate。是否对未捕获的异常进行任何栈回溯由实现定义。（简单的说就是不一定会调用析构）
#else
    try
    {
        f();
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "Caught an exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Caught an unknown exception!" << std::endl;
    }
#endif
}