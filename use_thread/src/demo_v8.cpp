// 只支持移动的类型
struct move_only {
    move_only() { std::puts("默认构造"); }
    move_only(const move_only &) = delete;
    move_only(move_only &&) noexcept { std::puts("移动构造"); }
};

void f1(move_only obj) { std::cout << &obj << std::endl; }
void f2(move_only &&obj) { std::cout << &obj << std::endl; }
void f3(const move_only &obj) { std::cout << &obj << std::endl; }

int main() {
    move_only obj;
    std::cout << &obj << std::endl;
    std::cout << "\n";

    // std::thread t{f1, obj};
    // error 左值传递给 tuple 时，无法调用复制构造(只能移动的类型)

    std::thread t1{f1, std::move(obj)};
    t1.join();
    std::cout << "\n";
    // 两次打印移动构造均为std::thread内部实现，第一次是移动构造给 tuple, 第二次是 invoke 传递给f进行调用

    std::thread t2{f2, std::move(obj)};
    t2.join();
    std::cout << "\n";
    std::thread t3{f3, std::move(obj)};
    t3.join();
}