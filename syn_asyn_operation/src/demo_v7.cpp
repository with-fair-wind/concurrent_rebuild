// future 是一次性的，需要注意移动。且调用 get 函数后，future 对象也会失去共享状态
/*
1:移动语义：移动操作标志着所有权的转移，意味着 future 不再拥有共享状态（如之前所提到）。get 和 wait 函数要求 future 对象拥有共享状态，否则会抛出异常。
2:共享状态失效：调用 get 成员函数时，future 对象必须拥有共享状态，但调用完成后，它就会失去共享状态，不能再次调用 get
*/

// 在 get 函数中: future _Local{_STD move(*this)}; 将当前对象的共享状态转移给了这个局部对象，而局部对象在函数结束时析构。这意味着当前对象失去共享状态，并且状态被完全销毁。
int main() {
    std::future<void> future = std::async([] {});
    std::cout << std::boolalpha << future.valid() << '\n';  // true
    future.get();
    std::cout << std::boolalpha << future.valid() << '\n';  // false
    try {
        future.get();  // 抛出 future_errc::no_state 异常
    } catch (std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}
