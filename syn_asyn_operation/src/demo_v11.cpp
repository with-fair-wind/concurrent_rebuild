std::condition_variable cv;
bool done{};
std::mutex m;

bool wait_loop() {
    const auto timeout = std::chrono::steady_clock::now() + 500ms;
    std::unique_lock<std::mutex> lk{m};
#if 0
    while (!done) {
        if (cv.wait_until(lk, timeout) == std::cv_status::timeout) {
            std::cout << "超时 500ms\n";
            return false;
        }
    }
#else
    if (!cv.wait_until(lk, timeout, [] { return done; })) {
        std::cout << "超时 500ms\n";
        return false;
    }
#endif
    return true;
}

int main() {
    std::thread t{wait_loop};
    done = true;
    std::this_thread::sleep_for(600ms);
    // done = true;
    cv.notify_one();
    t.join();
}