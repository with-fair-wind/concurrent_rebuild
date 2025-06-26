/*
C++20 std::jthread 相比于 C++ 11 引入的 std::thread，只是多了两个功能：
1: RAII 管理：在析构时自动调用 join()
2: 线程停止功能：线程的取消/停止
*/
// std::jthread 的通常实现就是单纯的保有 std::thread + std::stop_source(通常为8字节) 这两个数据成员

// std::jthread 所谓的线程停止只是一种基于用户代码的控制机制，而不是一种与操作系统系统有关系的线程终止
// 使用 std::stop_source 和 std::stop_token 提供了一种优雅地请求线程停止的方式，但实际上停止的决定和实现都由用户代码来完成

void f(std::stop_token stop_token, int value)
{
    while (!stop_token.stop_requested())
    { // 检查是否已经收到停止请求
        // std::cout << value++ << ' ';
        std::cout << value++ << ' ' << std::flush;
        // 加不加 std::flush 好像没区别
        std::this_thread::sleep_for(200ms);
    }
    std::cout << std::endl;
}
/*
1: get_stop_source：返回与 jthread 对象关联的 std::stop_source，允许从外部请求线程停止。
2: get_stop_token：返回与 jthread 对象停止状态关联的 std::stop_token，允许检查是否有停止请求。
3: request_stop：请求线程停止。
*/

/* !!!
“停止状态”指的是由 std::stop_source 和 std::stop_token 管理的一种标志，用于通知线程应该停止执行。
这种机制不是强制性的终止线程，而是提供一种线程内外都能检查和响应的信号(!!!)。
*/

int main()
{
#if 0
    std::jthread thread{f, 1}; // 打印 1..15 大约 3 秒
    std::this_thread::sleep_for(3s);
    // jthread 的析构函数调用 request_stop() 和 join()
#else
    std::jthread thread{f, 1}; // 打印 1..15 大约 3 秒
    // 隐式传递的参数为 与成员变量 std::stop_source(_Ssource) 相关联的 std::stop_token(_Ssource.get_token())
    std::this_thread::sleep_for(3s);
    thread.request_stop(); // 发送信息，线程终止
    std::this_thread::sleep_for(1s);
    std::cout << "end!\n";
#endif
}

/*
std::stop_source：
这是一个可以发出停止请求的类型。当调用 stop_source 的 request_stop() 方法时，它会设置内部的停止状态为“已请求停止”
任何持有与这个 stop_source 关联的 std::stop_token 对象都能检查到这个停止请求

std::stop_token：
这是一个可以检查停止请求的类型。线程内部可以定期检查 stop_token 是否收到了停止请求
通过调用 stop_token.stop_requested()，线程可以检测到停止状态是否已被设置为“已请求停止”
*/