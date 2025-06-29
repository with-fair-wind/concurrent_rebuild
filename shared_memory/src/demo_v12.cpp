// 线程变量 拥有线程存储期。它的存储在线程开始时分配，并在线程结束时解分配。每个线程拥有它自身的对象实例。
// 只有声明为 thread_local 的对象拥有此存储期（不考虑非标准用法）。
/*
它的初始化需要考虑局部与非局部两种情况：
1: 非局部变量：所有具有线程局部存储期的非局部变量的初始化，会作为线程启动的一部分进行，并按顺序早于线程函数的执行开始。
2: 静态局部变量(同普通静态局部对象)：控制流首次经过它的声明时才会被初始化（除非它被零初始化或常量初始化）。在其后所有的调用中，声明都会被跳过
*/

// gcc 与 clang 不打印, msvc会打印
// https://godbolt.org/z/Pr1PodPrK
thread_local int n = (std::cout << "thread_local init\n", 0);
void f1() { std::cout << "f\n"; }
void f2() { thread_local static int n = (std::cout << "f2 init\n", 0); }

int main() {
    std::cout << "main\n";
    std::thread{f1}.join();
    f2();
    f2();
    f2();
    std::thread{[]() {
        f2();
        f2();
    }}.join();
}