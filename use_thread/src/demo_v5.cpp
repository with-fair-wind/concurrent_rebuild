struct func {
    int &m_i;
    func(int &i) : m_i{i} {}
    void operator()(int n) const {
        for (int i = 0; i <= n; ++i) m_i += i;  // 可能悬空引用
        std::this_thread::sleep_for(1s);
        std::cout << m_i << std::endl;
    }
};

void f2() { throw std::runtime_error("test f2()"); }

void f() {
    int n = 0;
    std::thread t{func{n}, 10};
    try {
        // todo.. 一些当前线程可能抛出异常的代码
        f2();
    } catch (
        ...)  // 此处捕获异常只是确保函数 f 中创建的线程正常执行完成，其局部对象正常析构释放 不掩盖错误
    {
        t.join();  // 执行 join() 确保线程正常执行完成，线程对象可以正常析构
        throw;  // 1: 如果不抛出，还得执行一个 t.join() 显然逻辑不对，自然抛出
                // 2: 函数产生的异常，由调用方进行处理
    }
    t.join();  // f2()函数未抛出异常则使线程正常执行完成，线程对象可以正常析构
}

int main() {
    try {
        f();
    } catch (const std::runtime_error &e) {
        std::cout << "Caught an exception: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Caught an unknown exception!" << std::endl;
    }
}