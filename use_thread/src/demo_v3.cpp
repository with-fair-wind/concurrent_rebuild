struct Task {
    void operator()() const { std::cout << "void operator()() const" << std::endl; }
};

// 在确定每个形参的类型后，类型是 “T 的数组”或某个函数类型 T 的形参会调整为具有类型“指向 T 的指针”
std::thread f(Task());      // 声明  形参:函数类型
std::thread f(Task (*p)())  // 定义  形参:函数指针类型
{
    return {};
}

struct X : decltype([] { std::cout << "crazy" << std::endl; }) {};

int main() {
#if 0
    Task task;
    task();
    std::thread t{task};
    t.join();
#else
    std::thread t{Task{}};
    // std::thread t(Task());
    // 这被编译器解析为函数声明，是一个返回类型为 std::thread，函数名为 t，接受一个返回 Task 的空参的函数指针类型，也就是 Task(*)()
    t.join();

    f(nullptr);

    X x;
    x();
#endif
}
