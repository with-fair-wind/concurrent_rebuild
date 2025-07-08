#include <thread_pool.h>

int print_task(int n) {
    std::osyncstream{std::cout} << "Task " << n << " is running on thr: " << std::this_thread::get_id() << '\n';
    return n;
}
int print_task2(int n) {
    std::osyncstream{std::cout} << "🐢🐢🐢 " << n << " 🐉🐉🐉" << std::endl;
    return n;
}

struct X {
    void f1(int&& n) const { std::osyncstream{std::cout} << &n << '\n'; }
    void f2(int& n) const { std::osyncstream{std::cout} << &n << '\n'; }
    void f3(const int& n) const { std::osyncstream{std::cout} << &n << '\n'; }
    void f4(std::reference_wrapper<int>&& n) const { std::osyncstream{std::cout} << &(n.get()) << '\n'; }
};

int main() {
#if 0
    Thread_Pool pool{4};                    // 创建一个有 4 个线程的线程池
    std::vector<std::future<int>> futures;  // future 集合，获取返回值

    for (int i = 0; i < 10; ++i) {
        futures.emplace_back(pool.submit(print_task, i));
    }

    for (int i = 0; i < 10; ++i) {
        futures.emplace_back(pool.submit(print_task2, i));
    }
#if 1
    int sum = 0;
    for (auto &future : futures) {
        sum += future.get();  // get() 成员函数 阻塞到任务执行完毕，获取返回值
    }
    std::cout << "sum: " << sum << '\n';
#else
    // sleep_for不稳定
    // std::this_thread::sleep_for(1us);  // 线程池中的线程还未执行到 while(stop_), 此时已经迎来了析构，stop_为true，线程直接退出，并不执行tasks_中的任务
    std::this_thread::sleep_for(1ms);  // 线程池中的所有线程，都执行完了所有任务后，析构才调用 stop_为true 线程退出
#endif
#else
    Thread_Pool pool{4};  // 创建一个有 4 个线程的线程池

    X x;
    int n = 6;
    std::cout << &n << '\n';
    // std::bind 内部把所有参数按值存储成左值，再传给你的 f(int&&)，所以编译不过
    std::vector<std::future<void>> futures;  // future 集合，获取返回值
    futures.emplace_back(pool.submit(&X::f1, &x, n));
    // futures.emplace_back(pool.submit(&X::f2, &x, n));
    futures.emplace_back(pool.submit(&X::f3, &x, n));
    futures.emplace_back(pool.submit(&X::f4, &x, std::ref(n)));
    for (auto& future : futures) future.wait();  // 阻塞到任务执行完毕
#endif
}  // 析构自动 stop()自动 stop()