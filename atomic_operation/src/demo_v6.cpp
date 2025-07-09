// std::atomic<std::shared_ptr>
/*
1: 多个线程能在不同的 shared_ptr 对象上调用所有成员函数[3]（包含复制构造函数与复制赋值）而不附加同步，即使这些实例是同一对象的副本且共享所有权也是如此。
(之所以多个线程通过 shared_ptr 的副本可以调用一切成员函数，甚至包括非 const 的成员函数 operator=、reset，是因为 shared_ptr 的"控制块是线程安全的")
2: 若多个执行线程访问同一 shared_ptr 对象而不同步，且任一线程使用 shared_ptr 的非 const 成员函数，则将出现数据竞争；std::atomic<shared_ptr> 能用于避免数据竞争。
*/

// 显然，std::shared_ptr 并不是完全线程安全的
// 在 C++20 中，原子模板 std::atomic 引入了一个偏特化版本 std::atomic<std::shared_ptr> 允许用户原子地操纵 shared_ptr 对象, 它是原子类型，这意味着它的所有操作都是原子操作
// 若多个执行线程不同步地同时访问同一 std::shared_ptr 对象，且任何这些访问使用了 shared_ptr 的非 const 成员函数，则将出现数据竞争，除非通过 std::atomic<std::shared_ptr> 的实例进行所有访问。

#define Safe
class Data {
   public:
    Data(int value = 0) : value_(value) {}
    int get_value() const { return value_; }
    void set_value(int new_value) { value_ = new_value; }

   private:
    int value_;
};

#ifdef UnSafe

auto data = std::make_shared<Data>();

void writer() {
    for (int i = 0; i < 10; ++i) {
        std::shared_ptr<Data> new_data = std::make_shared<Data>(i);
        data.swap(new_data);  // 调用非 const 成员函数
        std::this_thread::sleep_for(100ms);
    }
}

void reader() {
    for (int i = 0; i < 10; ++i) {
        if (data) {
            std::cout << "读取线程值: " << data->get_value() << std::endl;
        } else {
            std::cout << "没有读取到数据" << std::endl;
        }
        std::this_thread::sleep_for(100ms);
    }
}

int main() {
    std::thread writer_thread{writer};
    std::thread reader_thread{reader};

    // 内部实现是两个指针
    std::cout << sizeof(std::shared_ptr<Data>) << std::endl;

    writer_thread.join();
    reader_thread.join();
}
/*
以上这段代码是典型的线程不安全，它满足：
多个线程不同步地同时访问同一 std::shared_ptr 对象
任一线程使用 shared_ptr 的非 const 成员函数
*/

/*
shared_ptr 的通常实现只保有两个指针
1: 指向底层元素的指针（get()) 所返回的指针）
2: 指向“控制块 ”的指针

控制块是一个动态分配的对象，其中包含：
1: 指向被管理对象的指针或被管理对象本身
2: 删除器（类型擦除）
3: 分配器（类型擦除）
4: 持有被管理对象的 shared_ptr 的数量
5: 涉及被管理对象的 weak_ptr 的数量
*/

/*
控制块(指向控制块的指针指向的对象)是线程安全的:
    这意味着多个线程可以安全地操作引用计数和访问管理对象，即使这些 shared_ptr 实例是同一对象的副本且共享所有权也是如此
然而，shared_ptr 对象实例本身并不是线程安全的:
    shared_ptr 对象实例包含一个指向控制块的指针和一个指向底层元素的指针。这两个指针的操作在多个线程中并没有同步机制

因此，如果多个线程同时访问同一个 shared_ptr 对象实例并调用非 const 成员函数（如 reset 或 operator=），这些操作会导致对这些指针的并发修改，进而引发数据竞争。
如果不是同一 shared_ptr 对象，每个线程读写的指针也不是同一个，控制块又是线程安全的，那么自然不存在数据竞争，可以安全的调用所有成员函数。

控制块的“线程安全”仅指它对引用计数（use_count 和 weak_count）的增减、对象的销毁等生命周期管理操作使用了原子操作，从而保证在多线程环境下不会因为竞态而导致计数错误或重复释放。
    它并不对被管理对象 T 的任何成员访问或状态修改本身加锁或做同步保护。
    不管控制块内部是直接存储了 T（如 std::make_shared）还是只存了一个指针，控制块都只负责管理内存和计数，不负责管理 T 的并发访问。
被管理对象 T 的线程安全性完全取决于 T 本身：
当多个 shared_ptr 副本各自操作控制块时，控制块中的指针复制、交换、销毁过程中引用计数的增减是安全的；

自我理解：
简而言之: 对 shared_ptr 本身而言，对其内部实现的两个指针没有同步机制，无法保证线程安全，而对于其中指向控制块的指针而言，其指向的 动态分配的空间(堆):控制块 是线程安全的(也就是所指向的对象(控制块)是线程安全的,也无法保证被管理对象 T 的线程安全性（被管理对象T的线程安全完全取决于 T 自己本身）
结合 atomic 的理解 : 对于指针的特化，只能保证对指针本身的操作是原子的，而无法保证其所指向的对象。而对于 shared_ptr 只能保证，其中指针指向的对象(控制块)是线程安全，而无法保证自己本身，所以是线程不安全的，对同一 shared_ptr 对象除非通过 std::atomic<std::shared_ptr> 的实例进行所有访问，才能保证线程安全，但依然无法保证被管理对象 T 的线程安全性
*/

#elif defined(Safe)
// https://godbolt.org/z/fbKEhescv
std::atomic<std::shared_ptr<Data> > data = std::make_shared<Data>();

void writer() {
    for (int i = 0; i < 10; ++i) {
        std::shared_ptr<Data> new_data = std::make_shared<Data>(i);
        data.store(new_data);  // 原子地替换所保有的值
        // data.load().swap(new_data);
        /*
        没有意义，因为 load() 成员函数返回的是底层 std::shared_ptr 的副本，也就是一个临时对象。
        对这个临时对象调用 swap 并不会改变 data 本身的值，因此这种操作没有实际意义，尽管这不会引发数据竞争（因为是副本）
        */
        std::this_thread::sleep_for(10ms);
    }
}

void reader() {
    for (int i = 0; i < 10; ++i) {
        if (auto sp = data.load()) {
            std::cout << "读取线程值: " << sp->get_value() << std::endl;
        } else {
            std::cout << "没有读取到数据" << std::endl;
        }
        std::this_thread::sleep_for(10ms);
    }
}

int main() {
#if 1
    std::thread writer_thread{writer};
    std::thread reader_thread{reader};

    // 内部实现是两个指针
    std::cout << sizeof(std::shared_ptr<Data>) << std::endl;

    writer_thread.join();
    reader_thread.join();
#endif

    /*
    在使用 std::atomic<std::shared_ptr> 的时要注意:
    切勿将受保护数据的指针或引用传递到互斥量作用域之外，不然保护将形同虚设。
    */
#ifdef UnSafe
    std::atomic<std::shared_ptr<int> > ptr = std::make_shared<int>(10);
    *ptr.load() = 100;
    /*
    1: 调用 load() 成员函数，原子地返回底层共享指针的副本
    2: std::shared_ptr 解引用，等价 *get()，返回了 int&
    3: 直接修改这个引用所指向的资源。
    在第一步时，已经脱离了 std::atomic 的保护，第二步就获取了被保护的数据的引用，第三步进行了修改，这导致了数据竞争
    */
#elif defined(Safe)
    std::atomic<std::shared_ptr<int> > ptr = std::make_shared<int>(10);
    std::atomic_ref<int> ref{*ptr.load()};
    ref = 100;  // 原子地赋 100 给被引用的对象
    // 使用 std::atomic_ref 我们得以确保在修改共享资源时保持操作的原子性，从而避免了数据竞争
#endif
}
#endif