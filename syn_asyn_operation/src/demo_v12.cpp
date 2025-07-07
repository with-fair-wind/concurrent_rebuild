// C++20 引入了信号量
// 信号量源自操作系统，是一个古老而广泛应用的同步设施，在各种编程语言中都有自己的抽象实现
// 信号量是一个非常轻量简单的同步设施，它维护一个计数，这个计数不能小于 0
/*
信号量提供两种基本操作：释放（增加计数）和等待（减少计数）。
如果当前信号量的计数值为 0，那么执行“等待”操作的线程将会一直阻塞，直到计数大于 0，也就是其它线程执行了“释放”操作。

C++ 提供了两个信号量类型：std::counting_semaphore 与 std::binary_semaphore，定义在 <semaphore> 中 
binary_semaphore 只是 counting_semaphore 的一个特化别名 : using binary_semaphore = counting_semaphore<1>;

acquire 函数就是“等待”（原子地减少计数），release 函数就是"释放"（原子地增加计数）
*/

/*
1:counting_semaphore 是一个轻量同步原语，能控制对共享资源的访问。
2:不同于 std::mutex，counting_semaphore 允许同一资源进行多个并发的访问，至少允许 LeastMaxValue 个同时访问者 
3:binary_semaphore 是 std::counting_semaphore 的特化的别名，其 LeastMaxValue 为 1 
4:LeastMaxValue 是我们设置的非类型模板参数，意思是信号量维护的计数最大值
5:如其名所示，LeastMaxValue 是最小 的最大值，而非实际 最大值。静态成员函数 max() 可能产生大于 LeastMaxValue 的值。
*/

// 全局二元信号量对象
// 设置对象初始计数为 0
#if 0
std::binary_semaphore smph_signal_main_to_thread{0};
std::binary_semaphore smph_signal_thread_to_main{0};

void thread_proc() {
    smph_signal_main_to_thread.acquire();
    std::cout << "[线程] 获得信号" << std::endl;

    std::this_thread::sleep_for(3s);

    std::cout << "[线程] 发送信号\n";
    smph_signal_thread_to_main.release();
}

int main() {
    std::jthread thr_worker{thread_proc};

    std::cout << "[主] 发送信号\n";
    smph_signal_main_to_thread.release();

    smph_signal_thread_to_main.acquire();
    std::cout << "[主] 获得信号\n";
}
#else
// 定义一个信号量，最大并发数为 3
std::counting_semaphore<3> semaphore{3};

void handle_request(int request_id) {
    // 请求到达，尝试获取信号量
    std::cout << "进入 handle_request 尝试获取信号量\n";

    semaphore.acquire();

    std::cout << "成功获取信号量\n";

    // 此处延时三秒可以方便测试，会看到先输出 3 个“成功获取信号量”，因为只有三个线程能成功调用 acquire，剩余的会被阻塞
    std::this_thread::sleep_for(3s);

    // 模拟处理时间
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<> dis(1, 5);
    int processing_time = dis(gen);
    std::this_thread::sleep_for(std::chrono::seconds(processing_time));

    std::cout << std::format("请求 {} 已被处理\n", request_id);

    semaphore.release();
}

int main() {
    // 模拟 10 个并发请求
    std::vector<std::jthread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(handle_request, i);
    }
}
#endif

// 自己的理解
/*
std::binary_semaphore 可以近似当作互斥量使用
std::condition_variable 条件变量因为需要结合 unique_lock 即使被 notify_all 也只有一个线程可以抢到锁
std::counting_semaphore 信号量允许同一资源进行多个并发的访问，此时若涉及到对共享资源的操作，还是需要进行互斥量保护
*/