# Smart pointers
Implementation of C++ STL smart pointers.
Includes:
* Unique pointer
* Shared pointer
* Weak pointer

## Unique pointer
Unique pointer supports all functions of C++ [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr). Smart pointer uses compresssed pair to store object pointer and its deleter effectively due to [Empty Base Optimization](https://en.cppreference.com/w/cpp/language/ebo). Also it has specialization for template type arrays.

## Shared pointer
Shared pointer is the implementation of [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr). Pointer has 2 control block realisations both for in-place initialization via MakeShared() function and initialization with existing pointer to minimize allocation count. Supports class EnableSharedFromThis equivalent to [std::enable_shared_from_this](https://en.cppreference.com/w/cpp/memory/enable_shared_from_this).

## Weak pointer
Weak pointer is the implementation of [std::weak_ptr](https://en.cppreference.com/w/cpp/memory/weak_ptr). It uses the same control blocks as Shared pointer for convertibility between Shared and Weak pointers and to resolve cycle reference problem with Shared pointer.