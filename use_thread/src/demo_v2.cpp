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

        std::vector<value_type> results{num_threads};
        std::vector<std::thread> threads;

        auto start = first;
        for (std::size_t i = 0; i < num_threads; ++i) {
            auto end = std::next(start, chunk_size + (i < remainder ? 1 : 0));
            threads.emplace_back([start, end, &results, i] { results[i] = std::accumulate(start, end, value_type{}); });
            start = end;
        }

        for (auto &thread : threads) thread.join();

        return std::accumulate(results.begin(), results.end(), value_type{});
    }
    return std::accumulate(first, second, value_type{});
}

// 如果需要多线程求和，可以使用 C++17 引入的求和算法 std::reduce 并指明执行策略。它的效率接近我们实现的 sum 的两倍，当前环境核心越多数据越多，和单线程效率差距越明显。

int main() {
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << "支持 " << n << " 个并发线程。\n";

    std::vector<std::string> vecs{"1", "2", "3", "4"};
    auto result = sum(vecs.begin(), vecs.end());
    std::cout << result << '\n';

    vecs.clear();
    for (std::size_t i = 0; i <= 1024001u; ++i) vecs.push_back(std::to_string(i));

    result = sum(vecs.begin(), vecs.end());
    // std::cout << result << '\n';
}