void hello() {
    std::cout << "Hello World" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

int main() {
#if 1
    std::thread t{hello};
    std::cout << std::boolalpha << t.joinable()
              << std::endl;  // true 当前线程对象关联了活跃线程
    // t.join(); 等待线程对象 `t` 关联的线程执行完毕，否则将一直阻塞。这里的调用是必须的，否则 `std::thread` 的析构函数将调用 std::terminate() 无法正确析构。
    t.join();  // 有两件事情  1:阻塞，让线程执行完毕 2:修改对象的状态,joinable返回值
    std::cout << std::boolalpha << t.joinable()
              << std::endl;  // false 当前线程对象已经没有关联活跃线程了
#else
    std::thread t;  // 默认构造，构造不关联线程的对象
    std::cout << std::boolalpha << t.joinable() << std::endl;
#endif
}
