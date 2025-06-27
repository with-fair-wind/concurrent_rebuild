// 死锁
std::mutex m1, m2;
std::size_t n{};
void f1()
{
    std::lock_guard<std::mutex> lc1{m1};
    std::this_thread::sleep_for(5ms);
    std::lock_guard<std::mutex> lc2{m2};
    ++n;
}
void f2()
{
    std::lock_guard<std::mutex> lc1{m2};
    std::this_thread::sleep_for(5ms);
    std::lock_guard<std::mutex> lc2{m1};
    ++n;
}

// 有的时候即使固定锁顺序，依旧会产生问题
struct X
{
    X(const std::string &str) : object{str} {}
    void print() const { std::puts(object.c_str()); }
    void address() const { std::cout << "object in add: " << static_cast<const void *>(object.data()) << std::endl; }

    friend void swap(X &lhs, X &rhs);

private:
    std::string object;
    std::mutex m;
};

void swap(X &lhs, X &rhs)
{
    if (&lhs == &rhs)
        return;
#define SocpedLock
#ifdef DeadLock
    std::lock_guard<std::mutex> lock1{lhs.m};
    std::this_thread::sleep_for(5ms);
    std::lock_guard<std::mutex> lock2{rhs.m};
    // C++ 标准库有很多办法解决这个问题，可以使用 std::lock ，它能一次性锁住多个互斥量，并且没有死锁风险。
#elif defined(STDLock)
    std::lock(lhs.m, rhs.m);
    std::lock_guard<std::mutex> lock1{lhs.m, std::adopt_lock};
    std::this_thread::sleep_for(5ms);
    std::lock_guard<std::mutex> lock2{rhs.m, std::adopt_lock};
// 因为前面已经使用了 std::lock 上锁，所以后的 std::lock_guard 构造都额外传递了一个 std::adopt_lock 参数，让其选择到不上锁的构造函数。函数退出也能正常解锁
#elif defined(SocpedLock)
    // C++17 新增了 std::scoped_lock ，提供此函数的 RAII 包装，通常它比裸调用 std::lock 更好
    //  使用 std::scoped_lock 可以将所有 std::lock 替换掉，减少错误发生
    std::scoped_lock guard{lhs.m, rhs.m};
    std::this_thread::sleep_for(5ms);
#endif
    std::swap(lhs.object, rhs.object);
}

int main()
{
#if 0
    std::jthread t1{f1};
    std::jthread t2{f2};
#else
    // 考虑用户调用的时候将参数交换，就会产生死锁：
    // X a{"🤣"}, b{"😅"};
    X a{std::string(20, 'a')}, b{std::string(20, 'b')};
    // std::string swap，短字符串（SSO）时交换内容，长字符串时交换指针
    a.address();
    b.address();
    a.print();
    b.print();
    std::jthread t1{[&]
                    { swap(a, b); a.address(); b.address(); a.print(); b.print(); }}; // 1

    std::jthread t2{[&]
                    { swap(b, a); a.address(); b.address(); a.print(); b.print(); }}; // 2
#endif
}