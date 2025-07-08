// C++20 以后任何 atomic 的特化都拥有 wait、notify_one 、notify_all 这些成员函数
// https://godbolt.org/z/z51vaoKj3
std::atomic<std::shared_ptr<int>> ptr = std::make_shared<int>();

void wait_for_wake_up() {
    std::osyncstream{std::cout} << "线程 " << std::this_thread::get_id() << " 阻塞，等待更新唤醒\n";

    // 等待 ptr 变为其它值
    ptr.wait(ptr.load());

    std::osyncstream{std::cout} << "线程 " << std::this_thread::get_id() << " 已被唤醒\n";
}

void wake_up() {
    std::this_thread::sleep_for(5s);

    // 更新值并唤醒
    ptr.store(std::make_shared<int>(10));
    ptr.notify_one();
}

int main() {
    std::jthread t{wait_for_wake_up};
    wake_up();
}