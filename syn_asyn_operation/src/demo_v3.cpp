// future
/*
1:用于处理线程中需要等待某个事件的情况，线程知道预期结果。等待的同时也可以执行其它的任务
2:独占的 std::future 、共享的 std::shared_future 类似独占/共享智能指针
3:std::future 只能与单个指定事件关联，而 std::shared_future 能关联多个事件
4:它们都是模板，它们的模板类型参数，就是其关联的事件（函数）的返回类型
5:当多个线程需要访问一个独立 future 对象时， 必须使用互斥量或类似同步机制进行保护
6:??? 而多个线程访问同一共享状态，若每个线程都是通过其自身的 shared_future 对象副本进行访问，则是安全的
*/

// 创建异步任务获取返回值
// 假设需要执行一个耗时任务并获取其返回值，但是并不急切的需要它。那么就可以启动新线程计算
// 然而 std::thread 没提供直接从线程获取返回值的机制。所以可以使用 std::async 函数模板。
/*
1:使用 std::async 启动一个异步任务，会返回一个 std::future 对象，这个对象和任务关联，将持有最终计算出来的结果。
2:当需要任务执行完的结果的时候，只需要调用 get() 成员函数，就会阻塞直到 future 为就绪为止（即任务执行完毕），返回执行结果。
3:valid() 成员函数检查 future 当前是否关联共享状态，即是否当前关联任务。还未关联，或者任务已经执行完（调用了 get()、set()），都会返回 false
*/
#define Strategy
#ifdef Init
int task(int n) {
    std::cout << "异步任务 ID: " << std::this_thread::get_id() << "\n";
    return n * n;
}

int main() {
    std::future<int> future = std::async(task, 10);
    std::cout << "main: " << std::this_thread::get_id() << '\n';
    std::cout << std::boolalpha << future.valid() << '\n';  // true
    std::cout << future.get() << '\n';
    std::cout << std::boolalpha << future.valid() << '\n';  // false
}

#elif defined(Params)
// std::async支持所有可调用(Callable)对象，并且也是默认按值复制，必须使用 std::ref 才能传递引用。
// 和 std::thread 一样，内部会将保有的参数副本转换为右值表达式进行传递，这是为了那些只支持移动的类型

struct X {
    int operator()(int n) const { return n * n; }
};
struct Y {
    int f(int n) const { return n * n; }
};
void f(int &p) { std::cout << &p << '\n'; }

int main() {
    Y y;
    int n = 0;
    auto t1 = std::async(X{}, 10);
    auto t2 = std::async(&Y::f, &y, 10);
    auto t3 = std::async([] {});
    auto t4 = std::async(f, std::ref(n));  //
    std::cout << &n << '\n';
}

#elif defined(MoveOnly)
struct move_only {
    move_only() { std::puts("默认构造"); }
    move_only(move_only &&) noexcept { std::puts("移动构造"); }
    move_only &operator=(move_only &&) noexcept {
        std::puts("移动赋值");
        return *this;
    }
    move_only(const move_only &) = delete;
};

move_only task(move_only x) {
    std::cout << "异步任务 ID: " << std::this_thread::get_id() << '\n';
    return x;
}

int main() {
    move_only x;
    std::future<move_only> future = std::async(task, std::move(x));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "main\n";
    move_only result = future.get();  // 等待异步任务执行完毕
}

#elif defined(Strategy)
/*
1:std::launch::async 在不同线程上执行异步任务。
2:std::launch::deferred 惰性求值，不创建线程，等待 future 对象调用 wait 或 get 成员函数的时候执行任务。
*/
/*
std::async 函数模板有两个重载，不给出执行策略就是以：std::launch::async | std::launch::deferred 调用另一个重载版本
此策略表示由实现选择到底是否创建线程执行异步任务。
如果系统资源充足，并且异步任务的执行不会导致性能问题，那么系统可能会选择在新线程中执行任务。
如果系统资源有限，或者延迟执行可以提高性能或节省资源，那么系统可能会选择延迟执行。
而在MSVC中 只要不是 launch::deferred 策略，那么 MSVC STL 实现中都是必然在线程中执行任务。因为是线程池，所以执行新任务是否创建新线程，任务执行完毕线程是否立即销毁，不确定
*/

void f() { std::cout << std::this_thread::get_id() << '\n'; }

void t1() {
    std::this_thread::sleep_for(3s);
    std::cout << "t1 end!\n";
}

void t2() { std::cout << "wait fot t1 end!\n"; }

int main() {
    std::cout << std::this_thread::get_id() << '\n';
    auto f1 = std::async(std::launch::deferred, f);
    f1.wait();                                                            // 在 wait() 或 get() 调用时执行，不创建线程
    auto f2 = std::async(std::launch::async, f);                          // 创建线程执行异步任务
    auto f3 = std::async(std::launch::deferred | std::launch::async, f);  // 实现选择的执行方式
    // 如果从 std::async 获得的 std::future 没有被移动或绑定到引用，那么在完整表达式结尾， std::future 的**析构函数将阻塞，直到到异步任务完成**。因为临时对象的生存期就在这一行，而对象生存期结束就会调用调用析构函数。
    // 这并不能创建异步任务，它会阻塞，然后逐个执行
    std::async(std::launch::async, t1);  // 临时量的析构函数等待 t1()
    std::async(std::launch::async, t2);  // t1() 完成前不开始
}
// 被移动的 std::future 没有所有权，失去共享状态，不能调用 get、wait 成员函数。
#endif