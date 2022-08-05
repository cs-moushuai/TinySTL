#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

/*
    Description：用于给容器申请空间
*/

#include <cstddef> // UINT_MAX
#include <iostream> // cerr

#include "construct.h"

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

template <typename T>
class allocator {
public:
    using value_type        = T;
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = const T&;
    using size_type         = size_t;
    // difference_type是一种常用的迭代器型别，用来表示两个迭代器之间的距离
    using difference_type   = ptrdiff_t;

    // static 使得不需要实例化类就可以调用，内部只写调用是防止模板导致的代码膨胀
    static pointer allocate() {
        return _allocate(static_cast<difference_type>(1), static_cast<pointer>(nullptr));
    }

    static pointer allocate(size_type n) {
        return _allocate(static_cast<difference_type>(n), static_cast<pointer>(nullptr));
    }

    static void deallocate(pointer p) {
        _deallocate(p);
    }
    static void deallocate(pointer p, size_type) {
        _deallocate(p);
    }

    static void construct(pointer p) {
        tinystl::construct(p);
    }

    static void construct(pointer p, const_reference value) {
        tinystl::construct(p, value);
    }

    static void construct(pointer p, T&& value) {
        tinystl::construct(p, std::move(value));
    }

    template <class... Args>
    static void construct(T* p, Args&& ...args) {
        tinystl::construct(p, std::forward<Args>(args)...);
    }

    static void destroy(pointer p) {
        tinystl::destroy(p);
    }

    static void destroy(pointer first, pointer last) {
        tinystl::destroy(first, last);
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