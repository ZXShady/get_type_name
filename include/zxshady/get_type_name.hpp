#ifndef ZXSHADY_GET_TYPE_NAME_HPP
#define ZXSHADY_GET_TYPE_NAME_HPP


#ifndef ZXSHADY_CONSTEXPR
#ifdef __cpp_constexpr
#define ZXSHADY_CONSTEXPR constexpr
#else
#define ZXSHADY_CONSTEXPR
#endif // __cpp_constexpr
#endif // !ZXSHADY_CONSTEXPR


#ifndef ZXSHADY_CPP_VER
#ifdef _MSVC_LANG
#define ZXSHADY_CPP_VER _MSVC_LANG
#else
#define ZXSHADY_CPP_VER __cplusplus
#endif
#endif

#ifndef ZXSHADY_NOEXCEPT
#if ZXSHADY_CPP_VER >= 201103L
#define ZXSHADY_NOEXCEPT noexcept
#else
#define ZXSHADY_NOEXCEPT throw()
#endif // __cpp_constexpr
#endif // !ZXSHADY_CONSTEXPR

#ifndef ZXSHADY_HAS_CPP_ATTRIBUTE
#ifdef __has_cpp_attribute
#define ZXSHADY_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define ZXSHADY_HAS_CPP_ATTRIBUTE(x) 0
#endif
#endif

#ifndef ZXSHADY_NODISCARD
#if ZXSHADY_HAS_CPP_ATTRIBUTE(nodiscard)
#define ZXSHADY_NODISCARD  [[nodiscard]]
#else
#define ZXSHADY_NODISCARD
#endif
#endif // !defined(ZXSHADY_NODISCARD)

namespace zxshady {

class get_type_name_result {
public:
    ZXSHADY_CONSTEXPR get_type_name_result(const char* begin, const char* end) ZXSHADY_NOEXCEPT
        : m_begin(begin)
        , m_end(end)
    {
    }
public:
    ZXSHADY_CONSTEXPR const char* begin() const ZXSHADY_NOEXCEPT { return m_begin; }
    ZXSHADY_CONSTEXPR const char* end() const ZXSHADY_NOEXCEPT { return m_end; }

    template<typename T>
    ZXSHADY_CONSTEXPR operator T() const { return T(m_begin, static_cast<typename T::size_type>(m_end - m_begin)); };

    template<typename T>
    ZXSHADY_CONSTEXPR T to() const { return T(m_begin, static_cast<typename T::size_type>(m_end - m_begin)); };
private:
    const char* m_begin;
    const char* m_end;
};

namespace details {
// this function is for guranteeing that s is pointing to same memory bassicly variables 
// for constexpr functions in C++11
// __FUNCSIG__ == __FUNCSIG__ <--- may be false
// why isn't len size_t? becuase it ain't worth including <cstddef> and no string will be longer than 2^16
// which is the miniumum integer that 'unsigned int' can hold
ZXSHADY_CONSTEXPR get_type_name_result get_type_name_variables(const char* s, unsigned int len) ZXSHADY_NOEXCEPT
{
#ifdef __clang__
    return get_type_name_result(s + 60, s + len - 2);
#elif defined(__GNUC__)

// constexpr is in __PRETTY_FUNCTION__
// but since __PRETTY_FUNCTION__ is not constexpr in GCC 4.x.x
// we won't include 'constexpr'
#if defined(__cpp_constexpr) && !(defined(__GNUC__) && __GNUC__ <= 4)
    return get_type_name_result(s + 74, s + len - 2);
#else
    return get_type_name_result(s + 74 - 9, s + len - 2);
#endif

#elif defined(_MSC_VER)
    return get_type_name_result(s + 68, s + len - 17);
#endif
}

}

// IMPORTANT!!!
// since clang 16
// not specifying the namespace in get_type_name_result means it won't put it in the __PRETTY_FUNCTION__ string
// leading to off errors
template<typename T>
#if !(defined(__GNUC__) && __GNUC__ <= 4)
ZXSHADY_CONSTEXPR
#endif
zxshady::get_type_name_result get_type_name(void) ZXSHADY_NOEXCEPT
{
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    return details::get_type_name_variables(__PRETTY_FUNCTION__,static_cast<unsigned int>(sizeof(__PRETTY_FUNCTION__)/sizeof(*__PRETTY_FUNCTION__)));
#elif defined(_MSC_VER)
    return details::get_type_name_variables(__FUNCSIG__,static_cast<unsigned int>(sizeof(__FUNCSIG__)/sizeof(*__FUNCSIG__)));
#else
    #error This Compiler does not support zxshady::get_type_name<T>()
#endif
}

}
#endif // !defined(ZXSHADY_GET_TYPE_NAME_HPP)
