#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

/*
    Description：用于给容器申请空间
*/

#include <new>
#include <cstddef>
#include <iostream>

namespace tinystl {

template <typename T>
// 置空只是为了获取类型
inline T* _allocate(ptrdiff_t size, T*) {
    // 设置异常退出的函数为 nullptr
    // ::opeator new 内容查看 https://stackoverflow.com/questions/10513425/what-are-operator-new-and-operator-delete
    // 本质 new 做了三件事
    // 1. 分配内存 2. 调用构造函数
    std::set_new_handler(nullptr);
    size_t newSize = static_cast<size_t>(size * sizeof(T));
    T* tmp = reinterpret_cast<T*>(::operator new(newSize));
    if (!tmp) {
        std::cerr << "out of memory" << std::endl;
        exit(1);
    }
    return tmp;
}

template <typename T>
inline void _deallocate(T* buffer) {
    ::operator delete(buffer);
}

template <typename T1, typename T2>
inline void _construct(T1* p, const T2& value) {
    // placement new 相关 https://www.cnblogs.com/luxiaoxun/archive/2012/08/10/2631812.html
    // 通过查询了解到这个操作叫做placement new，就是在指针p所指向的内存空间创建一个T1类型的对象，但是对象的内容是从T2类型的对象转换过来的（调用了T1的构造函数，T1::T1(value)）。
    // 就是在已有空间的基础上重新调整分配的空间，类似于realloc函数。这个操作就是把已有的空间当成一个缓冲区来使用，这样子就减少了分配空间所耗费的时间，因为直接用new操作符分配内存的话，在堆中查找足够大的剩余空间速度是比较慢的。
    new(p) T1(value);
}

template <typename T>
inline void _destroy(T* ptr) {
    ptr->~T();
}

template <typename T>
class allocator {
public:
    using value_type        = T;
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = const T&;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;

    // static 使得不需要实例化类就可以调用，内部只写调用是防止模板导致的代码膨胀
    static pointer allocate(size_type n) {
        return _allocate(static_cast<difference_type>(n), static_cast<pointer>(nullptr));
    }

    static void deallocate(pointer p, size_type n) {
        _deallocate(p);
    }

    static void construct(pointer p, const_reference value) {
        _construct(p, value);
    }

    static void destroy(pointer p) {
        _destroy(p);
    }

    pointer address(reference x) const {
        return static_cast<pointer>(&x);
    }
    const_pointer const_address(reference x) const {
        return static_cast<const_pointer>(&x);
    }

    size_type max_size() const {
        return static_cast<size_type>(UINT_MAX/sizeof(T));
    }
};

} // !namespace tinystl 

#endif // !ALLOCATOR_H_