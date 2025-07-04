// promise
// 类模板 std::promise 用于存储一个值或一个异常，之后通过 std::promise 对象所创建的 std::future 对象异步获得
// 只能移动不可复制，多用作函数形参

#define ReSet
#ifdef Normal
void calculate_square(std::promise<int> promiseObj, int num) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // 计算平方并设置值到 promise 中
    promiseObj.set_value(num * num);

    std::this_thread::sleep_for(2s);
}

int main() {
    std::promise<int> promise;
    std::future<int> future = promise.get_future();

    std::thread t{calculate_square, std::move(promise), 5};

    /*
    *** 区别于 std::packaged_task 阻塞至 std::packaged_task 任务执行完，promise 关联的 future 只阻塞到set_value/set_exception
    */

    std::cout << future.get() << std::endl;  // 阻塞直至结果可用
    std::cout << "end!" << std::endl;
    t.join();
}
#elif defined(Exception)
// set_exception() 接受一个 std::exception_ptr 类型的参数，这个参数通常通过 std::current_exception() 获取，用于指示当前线程中抛出的异常
// std::future 对象通过 get() 函数获取这个异常，如果 promise 所在的函数有异常被抛出，则 std::future 对象会重新抛出这个异常，从而允许主线程捕获并处理它
// 编译器不知道为啥不行
// https://godbolt.org/z/xs3x178vE
void throw_function(std::promise<int> prom) {
    try {
        throw std::runtime_error("一个异常");
    } catch (...) {
        prom.set_exception(std::current_exception());
    }
}

int main() {
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();

    std::jthread t(throw_function, std::move(prom));

    try {
        std::cout << "等待线程执行，抛出异常并设置\n";
        throw fut.get();
    } catch (std::exception &e) {
        std::cerr << "来自线程的异常: " << e.what() << '\n';
    }
}
#elif defined(ReSet)
// 共享状态的 promise 已经存储值或者异常，再次调用 set_value（set_exception） 会抛出 std::future_error 异常，将错误码设置为 promise_already_satisfied
void throw_function(std::promise<int> prom) {
    prom.set_value(100);
    try {
        throw std::runtime_error("一个异常");
    } catch (...) {
        try {
            // 共享状态的 promise 已存储值，调用 set_exception/set_value 产生异常
            prom.set_exception(std::current_exception());
            // prom.set_value(50);
        } catch (std::exception &e) {
            std::cerr << "来自 set_exception 的异常: " << e.what() << '\n';
        }
    }
}

int main() {
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();

    std::thread t(throw_function, std::move(prom));

    std::cout << "等待线程执行，抛出异常并设置\n";
    std::cout << "值：" << fut.get() << '\n';  // 100

    t.join();
}
#endif
