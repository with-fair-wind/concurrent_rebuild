// std::thread 源码 见 md 源码解析
struct X {
    X(X &&x) noexcept {}
    template <class Fn, class... Args>
    X(Fn &&f, Args &&...args) {}
    X(const X &) = delete;

    // 添加约束，使其无法选择到有参构造函数
    // template <class Fn, class... Args, std::enable_if_t<!std::is_same_v<std::remove_cvref_t<Fn>, X>, int> = 0>
    // X(Fn &&f, Args &&...args) {}
};

int main() {
    std::thread t{[] {}};
    std::cout << sizeof(std::thread) << std::endl;
    t.join();

    X x{[] {}};
    X x2{x};  // 正常属于拷贝构造，而拷贝构造被弃置
              // 但此时选择到了有参构造函数，不导致编译错误
}