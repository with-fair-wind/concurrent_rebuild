// std::barrier 和 std::latch 最大的不同是，前者可以在阶段完成之后将计数重置为构造时传递的值，而后者只能减少计数
#if 0
std::barrier barrier{10, [n = 1]() mutable noexcept { std::cout << "\t第" << n++ << "轮结束\n"; }};
// 一个过程完成后，重置计数，然后调用传入的 lambda 表达式

void f(int start, int end) {
    for (int i = start; i <= end; ++i) {
        std::osyncstream{std::cout} << i << ' ';
        barrier.arrive_and_wait();  // 减少计数并等待 解除阻塞时就重置计数并调用函数对象

        std::this_thread::sleep_for(300ms);
    }
}
// arrive_and_wait 等价于 wait(arrive());原子地将期待计数减少 1，然后在当前阶段的同步点阻塞直至运行当前阶段的阶段完成步骤
// arrive_and_wait() 会在期待计数减少至 0 时调用我们构造 barrier 对象时传入的 lambda 表达式，并解除所有在阶段同步点上阻塞的线程。之后重置期待计数为构造中指定的值。屏障的一个阶段就完成了。
//  std::osyncstream C++20 确保输出流在多线程环境中同步，避免除数据竞争，而且将不以任何方式穿插或截断

int main() {
    std::vector<std::jthread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(f, i * 10 + 1, (i + 1) * 10);
    }
}
#else
//  arrive_and_drop 可以改变重置的计数值：它将所有后继阶段的初始期待计数减少一，当前阶段的期待计数也减少一, 用来控制在需要的时候，让一些线程退出同步
std::atomic_int active_threads{4};
std::barrier barrier{4, [n = 1]() mutable noexcept { std::cout << "\t第" << n++ << "轮结束，活跃线程数: " << active_threads << '\n'; }};
// lambda 表达式必须声明为 noexcept ，因为 std::barrier 要求其函数对象类型必须是不抛出异常的
// 即要求 std::is_nothrow_invocable_v<_Completion_function&> 为 true

void f(int thread_id) {
    for (int i = 1; i <= 5; ++i) {
        std::osyncstream{std::cout} << "线程 " << thread_id << " 输出: " << i << '\n';
        if (i == 3 && thread_id == 2) {  // 假设线程 ID 为 2 的线程在完成第三轮同步后退出
            std::osyncstream{std::cout} << "线程 " << thread_id << " 完成并退出\n";
            --active_threads;           // 减少活跃线程数
            barrier.arrive_and_drop();  // 减少当前计数 1，并减少重置计数 1
            return;
        }
        barrier.arrive_and_wait();  // 减少计数并等待，解除阻塞时重置计数并调用函数对象
    }
}

int main() {
    std::vector<std::jthread> threads;
    for (int i = 1; i <= 4; ++i) {
        threads.emplace_back(f, i);
    }
}
#endif