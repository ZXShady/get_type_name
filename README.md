# get_type_name<T>

get_type_name is a single header file which does not use any standard library headers and provides a function `zxshady::get_type_name<T>()` which 
gets the name of a type for GCC,Clang,MSVC.

# Examples:
```cpp
auto name = zxshady::get_type_name<int>(); 
// name is of type zxshady::get_type_name_result which has a conversion operator to any type
// that supports construction from const char* and Container::size_type of the type

for(auto c : name) { // supports forward-iterators not reverse iterators though
    std::printf("%c",c); 
}
std::string string = name; // converts to string implicitly
std::string string2 = name.template to<std::string>(); // using to method

struct MyString{
    ...
    using size_type = std::size_t; // this is a important the conversion functions uses this alias
    MyString(const char* s, std::size_t n);
};

MyString mystr = name; // works 

```



tested on `godbolt.org` on every compiler version there.


[NOTE] this won't work for types longer than 65536 characters if using a 16-bit-int that sucks I know.
NOTE.
