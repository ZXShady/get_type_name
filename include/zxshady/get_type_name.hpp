#ifndef ZXSHADY_GET_TYPE_NAME_HPP
#define ZXSHADY_GET_TYPE_NAME_HPP


#ifndef ZXSHADY_CONSTEXPR
  #ifdef __cpp_constexpr
    #define ZXSHADY_CONSTEXPR constexpr
  #else
    #define ZXSHADY_CONSTEXPR
  #endif // __cpp_constexpr
#endif   // !ZXSHADY_CONSTEXPR


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
#endif   // !ZXSHADY_CONSTEXPR

#ifndef ZXSHADY_HAS_CPP_ATTRIBUTE
  #ifdef __has_cpp_attribute
    #define ZXSHADY_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
  #else
    #define ZXSHADY_HAS_CPP_ATTRIBUTE(x) 0
  #endif
#endif

#ifndef ZXSHADY_NODISCARD
  #if ZXSHADY_HAS_CPP_ATTRIBUTE(nodiscard)
    #define ZXSHADY_NODISCARD [[nodiscard]]
  #else
    #define ZXSHADY_NODISCARD
  #endif
#endif // !defined(ZXSHADY_NODISCARD)

namespace zxshady {
namespace details {
  namespace get_type_name {


#if !defined(__cpp_constexpr)

    unsigned int constexpr_string_find(const char* s, unsigned int lens, const char* f, unsigned lenf, unsigned int = 0) ZXSHADY_NOEXCEPT
    {
      if (lens < lenf)
        return static_cast<unsigned int>(-1);

      for (unsigned int i = 0; i < (lens - lenf + 1); ++i) {
        for (unsigned int j = 0; j < lenf; ++j) {
          if (s[i + j] != f[j]) {
            goto refind;
          }
        }
        return i;
      refind:;
      }
      return static_cast<unsigned int>(-1);
    }

    unsigned int constexpr_string_find_reverse(const char* input, unsigned int inputlen, const char* substr, unsigned int sublen) ZXSHADY_NOEXCEPT
    {
      if (inputlen < sublen)
        return static_cast<unsigned int>(-1); // not found

      for (int i = static_cast<int>(inputlen - sublen - 1); i >= 0; --i) {
        for (unsigned int j = 0; j < sublen; ++j) {
          if (input[i + j] != substr[j]) {
            goto refind;
          }
        }
        return i;
      refind:;
      }
      return static_cast<unsigned int>(-1);
    }

#else
    constexpr bool constexpr_string_equal(const char* s1, unsigned int len1, const char* s2, unsigned len2) ZXSHADY_NOEXCEPT
    {
      return (!len1 || !len2) ||
        (len1 == len2 && *s1 == *s2 && constexpr_string_equal(s1 + 1, len1 - 1, s2 + 1, len2 - 1));
    }

    constexpr unsigned int constexpr_string_find(
      const char*  s,
      unsigned int lens,
      const char*  f,
      unsigned     lenf,
      unsigned int pos = 0) ZXSHADY_NOEXCEPT
    {
      return (lens != 0) && constexpr_string_equal(s + pos, lenf, f, lenf)
        ? pos
        : constexpr_string_find(s, lens - 1, f, lenf, pos + 1);
    }
    constexpr unsigned int constexpr_string_find_reverse_impl(
      const char*  input,
      unsigned int inputlen,
      const char*  substr,
      unsigned int sublen,
      unsigned int pos = 0) ZXSHADY_NOEXCEPT
    {
      return (inputlen != 0) && constexpr_string_equal(input + pos, sublen, substr, sublen)
        ? pos
        : constexpr_string_find_reverse_impl(input, inputlen - 1, substr, sublen, pos - 1);
    }

    constexpr unsigned int constexpr_string_find_reverse(const char*  input,
                                                         unsigned int inputlen,
                                                         const char*  substr,
                                                         unsigned int sublen) ZXSHADY_NOEXCEPT
    {
      return constexpr_string_find_reverse_impl(input, inputlen, substr, sublen, inputlen - sublen);
    }

#endif
  } // namespace get_type_name
} // namespace details

class get_type_name_result {
public:
  ZXSHADY_CONSTEXPR get_type_name_result(const char* begin, const char* end) ZXSHADY_NOEXCEPT : m_begin(begin), m_end(end)
  {
  }
public:
  ZXSHADY_CONSTEXPR const char* begin() const ZXSHADY_NOEXCEPT { return m_begin; }
  ZXSHADY_CONSTEXPR const char* end() const ZXSHADY_NOEXCEPT { return m_end; }
   
  template<typename T>
  ZXSHADY_CONSTEXPR operator T() const
  {
    return T(m_begin, static_cast<typename T::size_type>(m_end - m_begin));
  }

  template<typename T>
  ZXSHADY_CONSTEXPR T to() const
  {
    return T(m_begin, static_cast<typename T::size_type>(m_end - m_begin));
  }
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
  inline ZXSHADY_CONSTEXPR get_type_name_result get_type_name_variables(const char* s, unsigned int len) ZXSHADY_NOEXCEPT
  {
#if defined(__clang__)
    return get_type_name_result(s + 5 + details::get_type_name::constexpr_string_find(s, len, "[T = ", 5),
                                s + details::get_type_name::constexpr_string_find_reverse(s, len, "]", 1));
#elif defined(__GNUC__) || defined(__GNUG__)
    return get_type_name_result(s + 10 + details::get_type_name::constexpr_string_find(s, len, "[with T = ", 10),
                                s + details::get_type_name::constexpr_string_find_reverse(s, len, "]", 1));
#elif defined(_MSC_VER)
    return get_type_name_result(s + 1 + details::get_type_name::constexpr_string_find(s, len, "<", 1),
                                s + details::get_type_name::constexpr_string_find_reverse(s, len, ">(void)", 7));
#endif
  }

} // namespace details

template<typename T>
  inline
#if !(defined(__GNUC__) && __GNUC__ <= 4)
    ZXSHADY_CONSTEXPR
  // ^^^^ not constexpr until Gcc 5.x.x
#endif
  
  zxshady::get_type_name_result
  get_type_name() ZXSHADY_NOEXCEPT
{
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
  return details::get_type_name_variables(__PRETTY_FUNCTION__,
                                          static_cast<unsigned int>(
                                            sizeof(__PRETTY_FUNCTION__) / sizeof(*__PRETTY_FUNCTION__)));
#elif defined(_MSC_VER)
  return details::get_type_name_variables(__FUNCSIG__,
                                          static_cast<unsigned int>(sizeof(__FUNCSIG__) / sizeof(*__FUNCSIG__)));
#else
  #error This Compiler does not support zxshady::get_type_name<T>()
#endif
}

} // namespace zxshady
#endif // !defined(ZXSHADY_GET_TYPE_NAME_HPP)
