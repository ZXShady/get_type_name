#include <string>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <zxshady/get_type_name.hpp>

#if defined(__clang__)
#define GCM(gnu,clang,msvc) clang
#elif defined(__GNUC__) || defined(__GNUG__)
#define GCM(gnu,clang,msvc) gnu
#elif defined(_MSC_VER)
#define GCM(gnu,clang,msvc) msvc
#endif

#define RESOLVE1(x) x
#define RESOLVE2(x) RESOLVE1(RESOLVE1(x)) 
#define RESOLVE(x) RESOLVE2(x)


struct functor_comp_char {
    functor_comp_char(char _c) : c(_c) {}
    char c;
    bool operator()(char _c)
    {
        return _c == c;
    }
};
std::string ltrim(std::string s,char c)
{
    
    s.erase(std::remove_if(s.begin(), s.end(), functor_comp_char( c )), s.end());
    return s;
}

void test(std::string str,std::string expected,unsigned int line)
{

    if (ltrim(str,' ') != ltrim(expected, ' ')) {
        std::printf("[%s] [TEST FAILED] [LINE:%u] [to_str] \"%s\" does not equal[expected] \"%s\"\n",GCM("GCC","CLANG","MSVC"), line, str.c_str(), expected.c_str());
        std::printf("SIDE BY SIDE COMPARISON\n[to_str]   '%s' \n[expected] '%s'\n", str.c_str(), expected.c_str());
    } else {
        std::printf("[succeeded] %s\n",str.c_str());
    }
}



#define TEST(type_str,type) test(zxshady::get_type_name<RESOLVE(type)>(),type_str,static_cast<unsigned int>(__LINE__))


// Static test not used although the  strings are constant?
// gcc has a bug that __PRETTY_FUNCTION__ is not constexpr in 8.0 and 9.2 but it is in 8.2 and versions <= 7.0
#define STATIC_TEST(type_str,type) static_assert(strcompare_no_space(zxshady::get_type_name<RESOLVE(type)>().template to<std::string_view>(),type_str),"failed")


void github_readme()
{
    auto name = zxshady::get_type_name<long long int>(); 
    // name is of type zxshady::get_type_name_result which has a conversion operator to any type
    // that supports construction from const char* and Container::size_type of the type

    for(auto c : name) { // supports forward-iterators not reverse iterators though
        std::printf("%c",c); 
    }
    std::printf("\n");
    std::string string = name; // converts to string implicitly
    std::string string2 = name.template to<std::string>(); // using to method
    std::puts(string.c_str());
    struct MyString{
        using size_type = std::size_t; // this is a important the conversion functions uses this alias
        MyString(const char* s, std::size_t n) {}
    };
    MyString mstr = name;
}

int main()
{
    github_readme();
        std::puts(__TIME__);
    try {
        TEST("float", float);
        TEST("double", double);
        TEST("int", int);
        TEST("unsigned int", unsigned int);
        TEST(GCM("long int", "long", "long"), long);
        TEST(GCM("long long unsigned int", "unsigned long long", "unsigned __int64"), unsigned long long int);
        TEST(GCM("const void(int)","const void(int)","const void(const int)"), const void(const int));
        TEST(GCM("void* volatile* const", "void *volatile *const", "void*volatile *const "), void* volatile* const);
        
#if defined(__clang__) && __clang_major__ >= 12
        TEST("std::basic_string<char>",std::string);
#elif defined(__clang__) && __clang_major__ <= 11
        TEST("std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>",std::string);
#else
        TEST(GCM("std::__cxx11::basic_string<char>"
             , "Clang is not handled here there ^^^^ "
             , "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >"), std::string);

#endif
#if ZXSHADY_CPP_VER >= 201103L

#if defined(__clang__) &&  __clang_major__ <= 13
        TEST(GCM("std::nullptr_t", "nullptr_t", "nullptr"), decltype(nullptr));
#else
        TEST(GCM("std::nullptr_t", "std::nullptr_t", "nullptr"), decltype(nullptr));
#endif

        struct S {
            unsigned int f();
            auto g(void) -> int;
        };
        void sf();
        // should be __thiscall
        // but msvc considers it cdecl??
        TEST(GCM("void(*)()", "void(*)()","void(__cdecl *)(void)"), decltype(&sf));

        TEST(GCM("unsigned int (main()::S::*)()", "unsigned int (S::*)()", "unsigned int(__thiscall main::S::* )(void)"), decltype(&S::f));
        TEST(GCM("int (main()::S::*)()", "int (S::*)()", "int(__thiscall main::S::* )(void)"), decltype(&S::g));

#if !(defined(_MSC_VER) && !defined(__clang__))
        auto lamdba = []() -> void {};
        using LadmbaT = decltype(lamdba);
        TEST(GCM("main()::<lambda()>", "(lambda at main.cpp:80:23)", "MSVC Generates random ids for lamdbas cant detect that"), LadmbaT);
#endif

#endif // nullptr 
    }
    catch (std::exception const& e) {
        std::puts(e.what());
    }
    std::cout << "[[[END OF TESTS]]]";
    std::cin.get();
}
