/*
"同步操作"
"同步操作"是指在计算机科学和信息技术中的一种操作方式，其中不同的任务或操作按顺序执行，一个操作完成后才能开始下一个操作。
在多线程编程中，各个任务通常需要通过**同步设施**进行相互**协调和等待**，以确保数据的**一致性**和**正确性**
*/

/*
等待事件及条件
详见 md
1: 忙等待(自旋)
2: 加延时
3: 条件变量
*/

// 忙等和加延时中:
// 忙等待
#define Condition
#ifdef BusyWait
bool flag = false;
std::mutex m;
void wait_for_flag() {
    // 循环中，函数对互斥量解锁，再再对互斥量上锁，另外的线程很难有机会获取锁并设置标识（因为修改函数和等待函数共用一个互斥量）对cpu的占用很高
    std::unique_lock<std::mutex> lk{m};
    while (!flag) {
        lk.unlock();  // 1 解锁互斥量
        lk.lock();    // 2 上锁互斥量
    }
    std::cout << "end\n";
}

int main() {
    std::jthread t1{wait_for_flag};
    std::jthread t2{[] {
        std::this_thread::sleep_for(5s);
        std::unique_lock<std::mutex> lk{m};
        flag = true;
    }};
}

// 加延时
#elif defined(Delay)
bool flag = false;
std::mutex m;

// 很难确定正确的休眠时间, 在需要快速响应的程序中就意味着丢帧或错过了一个时间片
void wait_for_flag() {
    // 循环中，休眠②前函数对互斥量解锁①，再休眠结束后再对互斥量上锁，让另外的线程有机会获取锁并设置标识（因为修改函数和等待函数共用一个互斥量）
    std::unique_lock<std::mutex> lk{m};
    while (!flag) {
        lk.unlock();  // 1 解锁互斥量
        std::this_thread::sleep_for(10ms);
        lk.lock();  // 2 上锁互斥量
    }
    std::cout << "end\n";
}

int main() {
    std::jthread t1{wait_for_flag};
    std::jthread t2{[] {
        std::this_thread::sleep_for(5s);
        std::unique_lock<std::mutex> lk{m};
        flag = true;
    }};
}

#elif defined(Condition)
// 通过另一线程触发等待事件的机制是最基本的唤醒方式，这种机制就称为“条件变量”
// std::condition_variable std::condition_variable_any
// condition_variable_any 是 condition_variable 的泛化
// 相对于只在 std::unique_lock<std::mutex> 上工作的  std::condition_variable
// condition_variable_any 能在任何满足可基本锁定(lock/unlock)(BasicLockable)要求的锁上工作
// any 版更加通用但是却有更多的性能开销。所以通常首选 std::condition_variable
std::mutex mtx;
std::condition_variable cv;
bool arrived = false;

void wait_for_arrival() {
    std::unique_lock<std::mutex> lck(mtx);
    // 会释放锁, 等到被唤醒，以及条件满足时，会去抢互斥锁
    cv.wait(lck, [] { return arrived; });  // 等待 arrived 变为 true
    std::cout << "到达目的地，可以下车了！" << std::endl;
#if 1
    std::this_thread::sleep_for(5s);
    std::cout << "Reset arrived to allow other threads to continue waiting and blocking\n";
    arrived = false;
#endif
}

void race_arrival() {
    std::unique_lock<std::mutex> lck(mtx);
    // 会释放锁, 等到被唤醒，以及条件满足时，会去抢互斥锁
    cv.wait(lck, [] { return arrived; });  // 等待 arrived 变为 true
    std::cout << "race_arrival" << std::endl;
#if 0
    std::this_thread::sleep_for(5s);
    std::cout << "Reset arrived to allow other threads to continue waiting and blocking\n";
    arrived = false;
#endif
}

void simulate_arrival() {
    std::this_thread::sleep_for(std::chrono::seconds(5));  // 模拟地铁到站，假设5秒后到达目的地
    {
        std::lock_guard<std::mutex> lck(mtx);
        arrived = true;  // 设置条件变量为 true，表示到达目的地
    }
    // cv.notify_one(); // 通知等待的线程
    cv.notify_all();
}

int main() {
    std::jthread t1{wait_for_arrival};
    std::jthread t3{race_arrival};
    std::jthread t2{simulate_arrival};
}

// 条件变量的 wait 成员函数有两个版本，以上代码使用的就是第二个版本，传入了一个谓词
/*
void wait(std::unique_lock<std::mutex>& lock);                 // 1

template<class Predicate>
void wait(std::unique_lock<std::mutex>& lock, Predicate pred); // 2

②等价于：
while (!pred())
    wait(lock);

第二个版本只是对第一个版本的包装，等待并判断谓词，会调用第一个版本的重载。这可以避免“虚假唤醒（spurious wakeup）” 

虚假唤醒（Spurious Wakeup）
操作系统或线程库在某些情况下可能会“无故”唤醒一个正在等待的线程，即使没有其他线程调用 notify_one() 或 notify_all()

!!! 不可使用 if 代替 while
表现：线程从 wait(lock) 返回，pred() 仍然返回 false。
后果：如果你用 if (!pred()) wait(lock);，那第一次 wait 被虚假唤醒后，就会直接执行下面的代码逻辑
    即使条件根本不满足，程序也认为“条件成立”并继续向下执行，可能导致数据不一致、访问越界、死锁等难以察觉的错乱。
*/
#endif
