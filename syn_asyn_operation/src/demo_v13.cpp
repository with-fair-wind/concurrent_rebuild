// 闩 (latch) 与屏障 (barrier) 是线程协调机制
// 允许任何数量的线程阻塞直至期待数量的线程到达
// 闩不能重复使用，而屏障则可以, 它们定义在标头 <latch> 与 <barrier>
// std::latch “闩” : 单次使用的线程屏障
/*
latch 类维护着一个 std::ptrdiff_t 类型的计数，且只能减少计数，无法增加计数
在创建对象的时候初始化计数器的值。线程可以阻塞，直到 latch 对象的计数减少到零
由于无法增加计数，这使得 latch 成为一种单次使用的屏障
*/

#if 0
std::latch work_start{3};

void work() {
    std::cout << "等待其它线程执行\n";
    work_start.wait();  // 等待计数为 0
    std::cout << "任务开始执行\n";
}

int main() {
    std::jthread thread{work};
    std::this_thread::sleep_for(3s);
    std::cout << "休眠结束\n";
    work_start.count_down();   // 默认值是 1 减少计数 1
    work_start.count_down(2);  // 传递参数 2 减少计数 2
}
#else
// 由于 latch 的计数不可增加，它的使用通常非常简单，可以用来划分任务执行的工作区间
std::latch latch{10};
// arrive_and_wait 函数等价于：count_down(n); wait(); 也就是减少计数 + 等待
// 必须等待所有线程执行到 latch.arrive_and_wait(); 将 latch 的计数减少至 0 才能继续往下执行。
void f(int id) {
    // todo.. 脑补任务
    std::this_thread::sleep_for(1s);
    std::cout << std::format("线程 {} 执行完任务，开始等待其它线程执行到此处\n", id);
    latch.arrive_and_wait();
    std::cout << std::format("线程 {} 彻底退出函数\n", id);
}

int main() {
    std::vector<std::jthread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(f, i);
    }
}
#endif