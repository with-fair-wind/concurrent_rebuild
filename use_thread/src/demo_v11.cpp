// std::this_thread
int main() {
    std::cout << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(1s);
    auto current_time = std::chrono::system_clock::now();
    std::cout << "-----" << std::endl;
    std::this_thread::sleep_until(current_time + 2s);
    std::this_thread::yield();
    /*
    // 线程需要等待某个操作完成，如果直接用一个循环不断判断这个操作是否完成就会使得这个线程占满 CPU 时间，这会造成资源浪费。
    // 此时可以判断操作是否完成，如果还没完成就调用 yield 交出 CPU 时间片让其他线程执行，过一会儿再来判断是否完成，这样这个线程占用 CPU 时间会大大减少
    while (!isDone()) { // 忙等，一直占用CPU时间片
        // std::this_thread::yield();
        std::this_thread::sleep_for(1us); // 不是忙等， 或者让线程休眠，睡一会儿
        // to do...
    }
    */
}