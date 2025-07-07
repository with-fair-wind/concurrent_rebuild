// qt、boost 库的多线程见 md

#include <memory>
inline std::size_t default_thread_pool_size() noexcept {
    std::size_t num_threads = std::thread::hardware_concurrency();
    num_threads = num_threads == 0 ? 2 : num_threads;
    return num_threads;
}

// 构造函数：初始化线程池并启动线程
// 析构函数：停止线程池并等待所有线程结束(而非任务结束)，可能任务还在队列中，但stop_已经是true，线程退出，任务还在队列中未执行

class Thread_Pool {
   public:
    using Task = std::function<void()>;
    Thread_Pool(const Thread_Pool &) = delete;
    Thread_Pool &operator=(const Thread_Pool &) = delete;

    Thread_Pool(std::size_t num_thread = default_thread_pool_size()) : stop_{false}, num_thread_{num_thread} { start(); }

    ~Thread_Pool() { stop(); }

    template <typename F, typename... Args>
    std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>> submit(F &&f, Args &&...args) {
        using RetType = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;
        if (stop_) throw std::runtime_error("ThreadPool is stopped");
#define FUNPTR
#ifdef INVOKE
        using Tuple = std::tuple<std::decay_t<F>, std::decay_t<Args>...>;
        auto tupPtr = std::make_shared<Tuple>(std::forward<F>(f), std::forward<Args>(args)...);
        auto task = std::make_shared<std::packaged_task<RetType()>>([tupPtr]() { return invoke_tuple<RetType>(tupPtr.get()); });
#elif defined(FUNPTR)
        using Tuple = std::tuple<std::decay_t<F>, std::decay_t<Args>...>;
        auto tupPtr = std::make_shared<Tuple>(std::forward<F>(f), std::forward<Args>(args)...);
        auto invoker = get_invoke<RetType, Tuple>(std::make_index_sequence<1 + sizeof...(Args)>{});
        auto task = std::make_shared<std::packaged_task<RetType()>>([invoker, tupPtr]() { return invoker(tupPtr.get()); });
#else
        auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
#endif
        std::future<RetType> ret = task->get_future();

        {
            std::lock_guard<std::mutex> lock{mutex_};
            tasks_.emplace([task] { (*task)(); });
        }
        cv_.notify_one();

        return ret;
    }

   private:
#ifdef INVOKE
    template <typename Ret, typename Tuple, size_t... I>
    static Ret invoke_helper(Tuple *tup, std::index_sequence<I...>) {
        return std::invoke(std::move(std::get<0>(*tup)),        // function at index 0
                           std::move(std::get<I + 1>(*tup))...  // args start at index 1
        );
    }

    template <typename Ret, typename Tuple>
    static Ret invoke_tuple(Tuple *tup) {
        constexpr size_t N = std::tuple_size<Tuple>::value;
        return invoke_helper<Ret>(tup, std::make_index_sequence<N - 1>{});
    }

#elif defined(FUNPTR)
    template <typename Ret, typename Tuple, std::size_t... Indices>
    static constexpr auto get_invoke(std::index_sequence<Indices...>) noexcept {
        return &Invoke<Ret, Tuple, Indices...>;  // 返回函数指针
    }

    template <typename Ret, class Tuple, std::size_t... Indices>
    static Ret Invoke(void *RawVals) noexcept {
        const std::unique_ptr<Tuple> FnVals(static_cast<Tuple *>(RawVals));
        Tuple &Tup = *FnVals.get();
        return std::invoke(std::move(std::get<Indices>(Tup))...);  // 真正的调用
    }
#endif

    void start() {
        for (std::size_t i = 0; i < num_thread_; ++i) {
            pool_.emplace_back([this] {
                while (!stop_) {
                    std::unique_lock<std::mutex> lock{mutex_};
                    cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                    if (tasks_.empty()) return;
                    Task task = std::move(tasks_.front());
                    tasks_.pop();
                    task();
                }
            });
        }
    }

    void stop() {
        stop_ = true;
        cv_.notify_all();
        for (auto &thread : pool_) {
            if (thread.joinable()) thread.join();
        }
        pool_.clear();
    }

   private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic_bool stop_;
    std::size_t num_thread_;
    std::queue<Task> tasks_;
    std::vector<std::thread> pool_;
};