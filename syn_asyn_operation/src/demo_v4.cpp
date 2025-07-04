// future 与 std::packaged_task
// 类模板 std::packaged_task 包装任何可调用(Callable)目标（函数、lambda 表达式、bind 表达式或其它函数对象），使得能异步调用它
// 其返回值或所抛异常被存储于能通过 std::future 对象访问的共享状态中
// 通常它会和 std::future 一起使用，不过也可以单独使用
// std::packaged_task 只能移动，不能复制

#if 0
int main() {
#if 0
    std::packaged_task<double(int, int)> task([](int a, int b) {
        std::cout << "task ID: " << std::this_thread::get_id() << "\n";
        return std::pow(a, b);
    });
    std::future<double> future = task.get_future();  // 和 future 关联
    task(10, 2);                                     // 此处执行任务
    std::cout << future.get() << "\n";               // 不阻塞，此处获取返回值
#else
    // 在线程中执行异步任务，然后再获取返回值，可以这么做
    std::packaged_task<double(int, int)> task([](int a, int b) { return std::pow(a, b); });
    std::future<double> future = task.get_future();
    std::thread t{std::move(task), 10, 2};  // 任务在线程中执行
    // todo.. 幻想还有许多耗时的代码
    t.join();
    std::cout << future.get() << '\n';  // 并不阻塞，获取任务返回值罢了
#endif
}
#else
// std::packaged_task 也可以在线程中传递，在需要的时候获取返回值，而非像上面那样将它自己作为可调用对象
template <typename R, typename... Ts, typename... Args>
    requires std::invocable<std::packaged_task<R(Ts...)> &, Args...>
void async_task(std::packaged_task<R(Ts...)> &task, Args &&...args) {
    // todo..
    task(std::forward<Args>(args)...);
    std::this_thread::sleep_for(2s);
}

int main() {
    std::packaged_task<int(int, int)> task([](int a, int b) {
        std::this_thread::sleep_for(2s);
        return a + b;
    });
    int value = 50;
    std::future<int> future = task.get_future();
    // 创建一个线程来执行异步任务
    // 套了一个 lambda，这是因为函数模板不是函数，它并非具体类型，没办法直接被那样传递使用，只能包一层
    std::thread t{[&] { async_task(task, value, value); }};
    std::cout << future.get() << '\n';  // 会阻塞直至任务执行完毕
    std::cout << "end!" << std::endl;
    t.join();
}
#endif