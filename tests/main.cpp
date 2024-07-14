
#include <string>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include "../include/zxshady/get_type_name.hpp"

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

struct MyString{
        typedef std::size_t size_type; // this is a important the conversion functions uses this alias
        MyString(const char*, size_type) {}
    };
void github_readme()
{
    zxshady::get_type_name_result name = zxshady::get_type_name<void(int)>(); 
    // name is of type zxshady::get_type_name_result which has a conversion operator to any type
    // that supports construction from const char* and Container::size_type of the type

    for(const char* iter = name.begin(),* const end=name.end();iter != end;++iter) { // supports forward-iterators not reverse iterators though
        std::printf("%c",*iter); 
    }
    std::printf("\n");
    std::string string = name; // converts to string implicitly
    std::string string2 = name.to<std::string>(); // using to method
    std::puts(string.c_str());
    
    MyString mstr = name;
    (void)mstr;
    (void)string2;
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
#if ZXSHADY_CPP_VER >= 201103L
        TEST(GCM("long long unsigned int", "unsigned long long", "unsigned __int64"), unsigned long long int);
#endif
        TEST(GCM("const void(int)","const void(int)","const void(const int)"), const void(const int));
        TEST(GCM("void* volatile* const", "void *volatile *const", "void*volatile *const "), void* volatile* const);
        
#if defined(__clang__) && __clang_major__ >= 12
        TEST("std::basic_string<char>",std::string);
#else
        TEST(GCM("std::__cxx11::basic_string<char>"
             , "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>"
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
        TEST(GCM("void(*)()", "void(*)()","void(__cdecl *)(void)"), decltype(&sf));

        TEST(GCM("unsigned int (main()::S::*)()", "unsigned int (S::*)()", "unsigned int(__thiscall main::S::* )(void)"), decltype(&S::f));
        TEST(GCM("int (main()::S::*)()", "int (S::*)()", "int(__thiscall main::S::* )(void)"), decltype(&S::g));

#if !(defined(_MSC_VER) && !defined(__clang__))
        auto lamdba = []() -> void {};
        using LadmbaT = decltype(lamdba);
        TEST(GCM("main()::<lambda()>", "(lambda at main.cpp:118:23)", "MSVC Generates random ids for lamdbas cant detect that"), LadmbaT);
#endif

#endif // nullptr 
    }
    catch (std::exception const& e) {
        std::puts(e.what());
    }
    std::cout << "[[[END OF TESTS]]]";
}
