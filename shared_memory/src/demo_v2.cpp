// std::lock_guard
std::mutex m;

// “粒度”通常用于描述锁定的范围大小，较小的粒度意味着锁定的范围更小，因此有更好的性能和更少的竞争。
void f()
{
#if 0
    m.lock();
    std::cout << std::this_thread::get_id() << '\n';
    m.unlock();
#else
    // code...
    {
        // C++17 添加了一个新的特性，类模板实参推导 std::lock_guard 可以根据传入的参数自行推导，而不需要写明模板类型参数
        // std::lock_guard<std::mutex> lc{m};
        std::lock_guard lc{m};
        std::cout << std::this_thread::get_id() << '\n';
    } // 控制锁的粒度
    // code...
#endif
}

void add_to_list(int n, std::list<int> &list)
{
    std::vector<int> numbers(n + 1);
    std::iota(numbers.begin(), numbers.end(), 0);
    int sum = std::accumulate(numbers.begin(), numbers.end(), 0);
    {
        std::lock_guard<std::mutex> lc{m};
        list.push_back(sum);
    }
}
void print_list(const std::list<int> &list)
{
    std::lock_guard<std::mutex> lc{m};
    for (const auto &i : list)
        std::cout << i << ' ';
    std::cout << '\n';
}

// C++17 还引入了一个新的“管理类”：std::scoped_lock
// 相较于 lock_guard的区别在于，它可以管理多个互斥量。不过对于处理一个互斥量的情况，它和 lock_guard 几乎完全相同。

int main()
{
#if 0
    std::vector<std::thread> threads;
    for (std::size_t i = 0; i < 10; ++i)
        threads.emplace_back(f);

    for (auto &thread : threads)
        thread.join();
#else
    std::list<int> list;
    std::thread t1{add_to_list, 10, std::ref(list)};
    std::thread t2{add_to_list, 10, std::ref(list)};
    std::thread t3{print_list, std::cref(list)};
    std::thread t4{print_list, std::cref(list)};
    t1.join();
    t2.join();
    t3.join();
    t4.join();
#endif
}