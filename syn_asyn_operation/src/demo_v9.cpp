// 返回类型是一个枚举类 std::future_status ，三个枚举项分别表示三种 future 状态。
/*
deferred 共享状态持有的函数正在延迟运行，结果将仅在明确请求时计算
ready 共享状态就绪
timeout 共享状态在经过指定的等待时间内仍未就绪
*/

int main() {
#define Deferred
#ifdef Deferred
    auto future = std::async(std::launch::deferred, [] { std::cout << "deferred\n"; });
    if (future.wait_for(5s) == std::future_status::deferred) std::cout << "std::launch::deferred" << '\n';
    future.wait();  // 在 wait() 或 get() 调用时执行，不创建线程
#elif defined(Ready)
    std::future<int> future = std::async([] {
        std::this_thread::sleep_for(34ms);
        return 6;
    });
    if (future.wait_for(35ms) == std::future_status::ready)
        std::cout << future.get() << '\n';
    else
        std::cout << "not ready\n";
#elif defined(TimeOut)
    std::future<int> future = std::async([] {
        std::this_thread::sleep_for(36ms);
        return 6;
    });
    if (future.wait_for(35ms) == std::future_status::timeout)
        std::cout << "任务还未执行完毕\n";
    else
        std::cout << "任务执行完毕\n" << future.get() << std::endl;
#endif
}