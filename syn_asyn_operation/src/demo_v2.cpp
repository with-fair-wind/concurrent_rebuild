template <typename T>
class threadsafe_queue {
    mutable std::mutex m;               // 互斥量，用于保护队列操作的独占访问
    std::condition_variable data_cond;  // 条件变量，用于在队列为空时等待
    std::queue<T> data_queue;           // 实际存储数据的队列
   public:
    threadsafe_queue() {}
    void push(T new_value) {
        {
            std::lock_guard<std::mutex> lk{m};
            std::cout << "push:" << new_value << std::endl;
            data_queue.push(new_value);
            std::this_thread::sleep_for(2s);
        }
        data_cond.notify_one();
        // std::this_thread::sleep_for(10ms);
    }
    // 从队列中弹出元素（阻塞直到队列不为空）
    void pop(T &value) {
        std::unique_lock<std::mutex> lk{m};
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        value = data_queue.front();
        std::cout << "pop:" << value << std::endl;
        data_queue.pop();
    }
    // 从队列中弹出元素（阻塞直到队列不为空），并返回一个指向弹出元素的 shared_ptr
    std::shared_ptr<T> pop() {
        std::unique_lock<std::mutex> lk{m};
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        std::shared_ptr<T> res{std::make_shared<T>(data_queue.front())};
        data_queue.pop();
        return res;
    }
    bool empty() const {
        std::lock_guard<std::mutex> lk(m);
        return data_queue.empty();
    }
};

void producer(threadsafe_queue<int> &q) {
    for (int i = 0; i < 5; ++i) q.push(i);
}
void consumer(threadsafe_queue<int> &q) {
    for (int i = 0; i < 5; ++i) {
        int value{};
        q.pop(value);
    }
}

int main() {
    threadsafe_queue<int> q;
    std::jthread t1{producer, std::ref(q)};
    std::jthread t2{consumer, std::ref(q)};
}