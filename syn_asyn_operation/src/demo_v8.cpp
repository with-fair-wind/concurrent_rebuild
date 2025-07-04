// 多个线程的等待 std::shared_future
// std::future 有一个局限：future 是一次性的，它的结果只能被一个线程获取。get() 成员函数只能调用一次，当结果被某个线程获取后，std::future 就无法再用于其他线程

#if 0
// 可能有多个线程都需要耗时的异步任务的返回值
int task() {
    // todo...
    return 10;
}

void thread_functio1(std::shared_future<int> fut)
// void thread_functio1(std::shared_future<int> &fut)
// void thread_functio1(std::future<int> &fut)
{
    // todo...
    try {
        int res = fut.get();
        std::cout << "func1 need: " << res << "\n";
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    // todo...
}

void thread_functio2(std::shared_future<int> fut)
// void thread_functio2(std::shared_future<int> &fut)
// void thread_functio2(std::future<int> &fut)
{
    // todo...
    try {
        int res = fut.get();
        std::cout << "func2 need: " << res << "\n";
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    // todo...
}

int main() {
    std::shared_future<int> future = std::async(task);
    std::jthread t1{thread_functio1, future};
    std::jthread t2{thread_functio2, future};
    // std::jthread t1{thread_functio1, std::ref(future)};
    // std::jthread t2{thread_functio2, std::ref(future)};
}
#else
// 此时就需要使用 std::shared_future 来替代 std::future 了。
// std::future 与 std::shared_future 的区别就如同 std::unique_ptr、std::shared_ptr 一样
// std::future 是只能移动的，其所有权可以在不同的对象中互相传递，但只有一个对象可以获得特定的同步结果
// 而 std::shared_future 是可复制的，多个对象可以指代同一个共享状态

/*
***
在多个线程中对"同一个" std::shared_future 对象进行操作时（如果没有进行同步保护）存在条件竞争。
而从多个线程访问同一共享状态，若每个线程都是通过其自身的 shared_future 对象副本进行访问，则是安全的
***
*/
std::string fetch_data() {
    std::this_thread::sleep_for(std::chrono::seconds(1));  // 模拟耗时操作
    return "从网络获取的数据！";
}

int main() {
    std::future<std::string> future_data = std::async(std::launch::async, fetch_data);

    // // 转移共享状态，原来的 future 被清空  valid() == false
    std::shared_future<std::string> shared_future_data = future_data.share();
#if 0
    // 多个线程持有一个 shared_future对象并操作
    // 这段代码存在数据竞争, 它并没有提供线程安全的方式, lambda 是按引用传递，也就是“同一个”进行操作了。

    // 第一个线程等待结果并访问数据
    std::thread thread1([&shared_future_data] {
        std::cout << "线程1:等待数据中..." << std::endl;
        shared_future_data.wait();
        std::cout << "线程1:收到数据:" << shared_future_data.get() << std::endl;
    });

    // 第二个线程等待结果并访问数据
    std::thread thread2([&shared_future_data]  // 多个线程持有一个 shared_future对象并操作
                        {
                            std::cout << "线程2:等待数据中..." << std::endl;
                            shared_future_data.wait();
                            std::cout << "线程2:收到数据:" << shared_future_data.get() << std::endl;
                        });
#else
    // 第一个线程等待结果并访问数据
    std::thread thread1([shared_future_data] {
        std::cout << "线程1:等待数据中..." << std::endl;
        shared_future_data.wait();
        std::cout << "线程1:收到数据:" << shared_future_data.get() << std::endl;
    });

    // 第二个线程等待结果并访问数据
    std::thread thread2([shared_future_data]  // 多个线程持有一个 shared_future对象并操作
                        {
                            std::cout << "线程2:等待数据中..." << std::endl;
                            shared_future_data.wait();
                            std::cout << "线程2:收到数据:" << shared_future_data.get() << std::endl;
                        });
#endif
    thread1.join();
    thread2.join();
}
#endif
