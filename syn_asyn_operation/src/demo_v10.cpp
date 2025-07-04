// Windows 内核中的时间间隔计时器默认每隔 15.6 毫秒触发一次中断。因此，如果你使用基于系统时钟的计时方法，默认情况下精度约为 15.6 毫秒。不可能达到纳秒级别。
#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")
#endif
#include "Windows.h"
int main() {
    // timeBeginPeriod(1);
    auto start = std::chrono::system_clock::now();
    std::this_thread::sleep_for(1s);
    auto end = std::chrono::system_clock::now();
    auto res = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000>>>(end - start);
    std::cout << res.count() << std::endl;
    // timeEndPeriod(1);
}