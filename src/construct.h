#ifndef CONSTRUCT_H_
#define CONSTRUCT_H_

/*
    Description：定义 construct 构造对象，destroy 析构对象
        其中 construct 的对象除了分配的内存地址外，还有各种参数列表
        而 destroy 可以析构一个指针或是 [first, last) 间的迭代器
        且根据 value_type 判断是否有 non-trivial destructor (如果特别定义了析构函数，
        则说明需要在释放空间之前做一些事情，则这个析构函数称之为 non-trivial destructor。)
        若有则调用相应的析构函数
*/

namespace tinystl {

#include <new> // placement new
#include <type_traits> // 析构函数判定

template <typename T>
inline void construct(T* p) {
    // placement new 相关 https://www.cnblogs.com/luxiaoxun/archive/2012/08/10/2631812.html
    // 通过查询了解到这个操作叫做placement new，就是在指针p所指向的内存空间创建一个T1类型的对象，但是对象的内容是从T2类型的对象转换过来的（调用了T1的构造函数，T1::T1(value)）。
    // 就是在已有空间的基础上重新调整分配的空间，类似于realloc函数。这个操作就是把已有的空间当成一个缓冲区来使用，这样子就减少了分配空间所耗费的时间，因为直接用new操作符分配内存的话，在堆中查找足够大的剩余空间速度是比较慢的。
    ::new ((void*)p) T();
}

template <typename T1, typename T2>
inline void construct(T1* p, const T2& value) {
    ::new ((void*)p) T1(value);
}

// 可变参数模板
template <typename T1, typename... Args>
inline void construct(T1* p, Args&&... args) {
    ::new ((void*)p) T1(std::forward<Args>(args)...);
}

template <typename T>
inline void destroy_one(T*, std::true_type) {}

template <typename T>
inline void destroy_one(T* ptr, std::false_type) {
    if (ptr)
        ptr->~T();
}

template <typename ForwardIterator>
inline void destroy_cat(ForwardIterator, ForwardIterator, std::true_type) {}

template <typename ForwardIterator>
inline void destroy_cat(ForwardIterator first, ForwardIterator last, std::false_type) {
    for (; first != last; ++first)
        destroy(&*first);
}

template <typename T>
inline void destroy(T* ptr) {
    // 判断析构函数是否平凡
    destroy_one(ptr, std::is_trivially_destructible<T>{});
}

template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    // __type_traits
    destroy_cat(first, last, std::is_trivially_destructible<
            typename std::iterator_traits<ForwardIterator>::value_type>{});
}

} // !namespace tinystl 

#endif // !CONSTRUCT_H_