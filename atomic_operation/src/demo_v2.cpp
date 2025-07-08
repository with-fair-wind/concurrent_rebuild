// 通常 std::atomic 对象不可进行复制、移动、赋值。不过可以隐式转换成对应的内置类型，因为它有转换函数
/*
atomic(const atomic&) = delete;
atomic& operator=(const atomic&) = delete;
operator T() const noexcept;
*/

/*
std::atomic 类模板不仅只能使用标准库定义的特化类型，我们也完全可以自定义类型创建对应的原子对象
不过因为是通用模板，操作仅限 load()、store()、exchange()、compare_exchange_weak() 、 compare_exchange_strong()，以及一个转换函数

模板 std::atomic 可用任何满足可复制构造 (CopyConstructible)及可复制赋值 (CopyAssignable)的可平凡复制 (TriviallyCopyable)类型 T 实例化
*/

struct trivial_type {
    int x{};
    float y{};

    trivial_type() {}
    trivial_type(int a, float b) : x{a}, y{b} {}
    trivial_type(const trivial_type &other) = default;
    trivial_type &operator=(const trivial_type &other) = default;
    ~trivial_type() = default;
};

template <typename T>
void isAtomic() {
    static_assert(std::is_trivially_copyable<T>::value, "");
    static_assert(std::is_copy_constructible<T>::value, "");
    static_assert(std::is_move_constructible<T>::value, "");
    static_assert(std::is_copy_assignable<T>::value, "");
    static_assert(std::is_move_assignable<T>::value, "");
}

int main() {
    isAtomic<trivial_type>();
    // 创建一个 std::atomic<trivial_type> 对象
    std::atomic<trivial_type> atomic_my_type{trivial_type{10, 20.5f}};

    // 使用 store 和 load 操作来设置和获取值
    trivial_type new_value{30, 40.5f};
    atomic_my_type.store(new_value);

    std::cout << "x: " << atomic_my_type.load().x << ", y: " << atomic_my_type.load().y << std::endl;

    // 使用 exchange 操作
    trivial_type exchanged_value = atomic_my_type.exchange(trivial_type{50, 60.5f});
    std::cout << "交换前的 x: " << exchanged_value.x << ", 交换前的 y: " << exchanged_value.y << std::endl;
    std::cout << "交换后的 x: " << atomic_my_type.load().x << ", 交换后的 y: " << atomic_my_type.load().y << std::endl;
}

// 原子类型的每个操作函数，都有一个内存序参数，这个参数可以用来指定执行顺序，操作分为三类：
/*
1: Store 操作（存储操作）：可选的内存序包括 memory_order_relaxed、memory_order_release、memory_order_seq_cst。

2: Load 操作（加载操作）：可选的内存序包括 memory_order_relaxed、memory_order_consume、memory_order_acquire、memory_order_seq_cst。

3: Read-modify-write（读-改-写）操作：可选的内存序包括 memory_order_relaxed、memory_order_consume、memory_order_acquire、memory_order_release、memory_order_acq_rel、memory_order_seq_cst。
*/
