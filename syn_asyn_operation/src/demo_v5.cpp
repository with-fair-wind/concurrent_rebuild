// 改写sum : std::package_task + std::future 的形式
template <typename ForwardIt>
auto sum(ForwardIt first, ForwardIt second)
// typename std::iterator_traits<ForwardIt>::value_type sum(ForwardIt first, ForwardIt second)  // C++11
{
    using value_type = std::iter_value_t<ForwardIt>;  // C++20
    // using value_type = typename std::iterator_traits<ForwardIt>::value_type // C++11
    std::size_t num_threads = std::thread::hardware_concurrency();
    std::ptrdiff_t distance = std::distance(first, second);

    if (distance > 1024000) {
        std::size_t chunk_size = distance / num_threads;
        std::size_t remainder = distance % num_threads;

        std::vector<std::future<value_type>> futures{num_threads};
        std::vector<std::packaged_task<value_type()>> tasks;

        std::vector<std::thread> threads;

        auto start = first;
        for (std::size_t i = 0; i < num_threads; ++i) {
            auto end = std::next(start, chunk_size + (i < remainder ? 1 : 0));
            tasks.emplace_back([start, end] { return std::accumulate(start, end, value_type{}); });
            futures[i] = tasks[i].get_future();
            threads.emplace_back(std::move(tasks[i]));
            start = end;
        }

        for (auto &thread : threads) thread.join();

        value_type total_sum{};
        for (auto &future : futures) total_sum += future.get();
        return total_sum;
    }
    return std::accumulate(first, second, value_type{});
}

int main() {
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << "支持 " << n << " 个并发线程。\n";

    std::vector<std::string> vecs{"1", "2", "3", "4"};
    auto result = sum(vecs.begin(), vecs.end());
    std::cout << result << '\n';

    vecs.clear();
    for (std::size_t i = 0; i <= 102u; ++i) vecs.push_back(std::to_string(i));

    result = sum(vecs.begin(), vecs.end());
    std::cout << result << '\n';
}