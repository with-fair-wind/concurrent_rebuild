// new、delete 是线程安全的吗？
// C++ 只保证了 operator new、operator delete 这两个方面即内存的申请与释放的线程安全（不包括用户定义的）
/*
new 表达式线程安全要考虑三方面：operator new、构造函数、修改指针
delete 表达式线程安全考虑两方面：operator delete、析构函数
*/
// 详见 md

// 线程存储期
int global_counter = 0;
thread_local int thread_local_counter = 0;

void print_counters() {
    std::cout << "global: " << global_counter++ << '\n';
    std::cout << "thread_local: " << thread_local_counter++ << '\n';
    std::cout << "&global: " << &global_counter << '\n';
    std::cout << "&thread_local: " << &thread_local_counter << '\n';
}

int main() {
    // 地址复用
    std::thread{print_counters}.join();
    std::thread{print_counters}.join();
}