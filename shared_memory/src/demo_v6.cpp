// unique_lock
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
    // std::unique_lock 私有数据成员 _Owns -> bool, _Pmtx -> _Mutex *
    // std::defer_lock 是“不获得互斥体的所有权” _Owns初始化为false, 没有所有权自然构造函数就不会上锁
    // std::unique_lock 是有 lock() 、try_lock() 、unlock() 成员函数的，所以可以直接传递给 std::lock
    // lock() 函数中会将_Owns置为true, 即表示当前对象拥有互斥量的所有权, 析构函数中if(_Owns) _Pmtx->unlock();
    //  必须得是当前对象拥有互斥量的所有权析构函数才会调用 unlock() 解锁互斥量。
    // std::unique_lock 要想调用 lock() 成员函数，必须是当前没有所有权
    // 正常的用法是，先上锁了互斥量，然后传递 std::adopt_lock 构造 std::unique_lock 对象表示拥有互斥量的所有权，即可在析构的时候正常解锁。

    /*
    简而言之：
    1: 使用 std::defer_lock 构造函数不上锁，要求构造之后上锁
    2: 使用 std::adopt_lock 构造函数不上锁，要求在构造之前互斥量上锁
    3: 默认构造会上锁，要求构造函数之前和构造函数之后都不能再次上锁
    */

#define Adopt_lock
#ifdef Defer_lock
    // 此场景不会死锁
    std::unique_lock<std::mutex> loc1{lhs.m, std::defer_lock};
    std::unique_lock<std::mutex> loc2{rhs.m, std::defer_lock};
    std::lock(loc1, loc2);
    swap(lhs.object, rhs.object);
#elif defined(Adopt_lock)
#if 1
    // 此场景不会死锁
    std::lock(lhs.m, rhs.m);
    std::unique_lock<std::mutex> loc1{lhs.m, std::adopt_lock};
    std::unique_lock<std::mutex> loc2{rhs.m, std::adopt_lock};
#else
    // 此场景会死锁
    std::unique_lock<std::mutex> loc1{lhs.m, std::adopt_lock};
    std::unique_lock<std::mutex> loc2{rhs.m, std::adopt_lock};
    // 可以有这种写法，但是不推荐
    loc1.mutex()->lock();
    std::this_thread::sleep_for(5ms);
    loc2.mutex()->lock();
#endif
    swap(lhs.object, rhs.object);
#elif defined(Default)
    // 此场景会死锁
    std::unique_lock<std::mutex> loc1{lhs.m};
    std::this_thread::sleep_for(5ms);
    std::unique_lock<std::mutex> loc2{rhs.m};
    swap(lhs.object, rhs.object);
#endif
}

int main()
{
    X a{"🤣"}, b{"😅"};
    std::jthread t1{[&]
                    { swap(a, b); }};
    std::jthread t2{[&]
                    { swap(b, a); }};
}