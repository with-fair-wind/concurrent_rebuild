// 保护不常更新的数据结构
// 读写锁: std::shared_timed_mutex（C++14）、 std::shared_mutex（C++17）
// 它们的区别简单来说，前者支持更多的操作方式，后者有更高的性能优势。

// std::shared_mutex 同样支持 std::lock_guard、std::unique_lock。和 std::mutex 做的一样，保证写线程的独占访问
// 而那些无需修改数据结构的读线程，可以使用 std::shared_lock<std::shared_mutex> 获取访问权，多个线程可以一起读取

class Settings {
   private:
    std::map<std::string, std::string> data_;
    mutable std::shared_mutex mutex_;  // “M&M 规则”：mutable 与 mutex 一起出现

   public:
    void set(const std::string &key, const std::string &value) {
        std::lock_guard<std::shared_mutex> lock{mutex_};
        data_[key] = value;
    }

    std::string get(const std::string &key) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = data_.find(key);
        return (it != data_.end()) ? it->second : "";  // 如果没有找到键返回空字符串
    }
};

int main() { return 0; }