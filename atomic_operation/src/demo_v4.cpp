// 原子类型共性知识: 以 atomic_bool 为例
#define Type 3
#if Type == 1
std::atomic_bool b{true};
int main() {
    // 一般operator=返回引用，可以连等
    std::string s{};
    s = "kk";
    // atomic 的 operator= 不同于通常情况, 赋值操作 b = false 返回一个普通的 bool 值
    b = false;
#if 1
    // 避免多余的加载（load）过程
    bool new_value = (b = true);
#else
    auto &ref = (b = true);  // 假如返回 atomic 引用
    bool flag = ref.load();
#endif
}

/*
1: 使用 store 原子的替换当前对象的值，远好于 std::atomic_flag 的 clear()
2: test_and_set() 也可以换为更加通用常见的 exchange，它可以原子的使用新的值替换已经存储的值，并返回旧值
*/

/*
比较/交换: compare_exchange_weak() 和 compare_exchang_strong()
1: compare_exchange_weak: 尝试将原子对象的当前值与预期值进行比较，如果相等则将其更新为新值并返回 true；否则，将原子对象的值加载进 expected（进行加载操作）并返回 false
此操作可能会由于某些硬件的特性而出现假失败[2]，需要在循环中重试
2: compare_exchang_strong: 类似于 compare_exchange_weak，但不会出现假失败，因此不需要重试
*/
#elif Type == 2
int main() {
    std::atomic<bool> flag{false};
    bool expected = false;
    while (!flag.compare_exchange_weak(expected, true));
    std::cout << std::boolalpha << flag << std::endl;
}
#else
std::atomic<bool> flag{false};
bool expected = false;

void try_set_flag() {
    // 尝试将 flag 设置为 true，如果当前值为 false
    if (flag.compare_exchange_strong(expected, true)) {
        std::osyncstream{std::cout} << "flag 为 false, 设为 true。\n";
    } else {
        std::osyncstream{std::cout} << "flag 为 true, expected 设为 true。\n";
    }
}

int main() {
    std::jthread t1{try_set_flag};
    std::jthread t2{try_set_flag};
}
#endif
