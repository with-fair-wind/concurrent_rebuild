// std::atomic<T*>
// 对指针本身的操作是原子，无法保证指针所指向的对象
/*
1: fetch_add：以原子方式增加指针的值, 并返回操作前的指针值）
2: fetch_sub：以原子方式减少指针的值, 并返回操作前的指针值。
3: operator+= 和 operator-=：以原子方式增加或减少指针的值
*/
struct Foo {};

int main() {
    Foo array[5]{};
    std::atomic<Foo *> p{array};

    // p 加 2，并返回原始值
    Foo *x = p.fetch_add(2);
    assert(x == array);
    assert(p.load() == &array[2]);

    // p 减 1
    x = (p -= 1);
    assert(x == &array[1]);
    assert(p.load() == &array[1]);

    // 函数也允许内存序作为给定函数的参数
    p.fetch_add(3, std::memory_order_release);
}
