#include "../../cpp/Io.hpp"

// custom struct for printing
struct test 
{
    hsd::i32 a = 1, b = 0;
    const char* c = "sample text";
    hsd::f64 d = 4.2;

    test() = default;

    test(hsd::i32 first, hsd::i32 second, const char* third, hsd::f64 forth)
        : a{first}, b{second}, c{third}, d{forth}
    {}
};

template<hsd::io_detail::u8string_literal str>
void _print(test& t, FILE* ptr = stdout)
{
    if(ptr == stdout)
    {
        hsd::io_detail::_print<str>();
        hsd::io::print<"Struct Test contains:\n[\n    a = {}\n    b = {}\n    c = {}\n    d = {}\n]">(t.a, t.b, t.c, t.d);
    }
    else if(ptr == stderr)
    {
        hsd::io_detail::_print<str>(stderr);
        hsd::io::err_print<"Struct Test contains:\n[\n    a = {}\n    b = {}\n    c = {}\n    d = {}\n]">(t.a, t.b, t.c, t.d);
    }
}

template<hsd::io_detail::wstring_literal str>
void _print(test& t, FILE* ptr = stdout)
{
    if(ptr == stdout)
    {
        // Optional: You can use hsd::io::print<L"{}">(str) as well
        hsd::io_detail::_print<str>();
        hsd::io::print<L"Struct Test contains:\n[\n    a = {}\n    b = {}\n    c = {}\n    d = {}\n]">(t.a, t.b, t.c, t.d);
    }
    else if(ptr == stderr)
    {
        hsd::io_detail::_print<str>(stderr);
        hsd::io::err_print<L"Struct Test contains:\n[\n    a = {}\n    b = {}\n    c = {}\n    d = {}\n]">(t.a, t.b, t.c, t.d);
    }
}

void _parse(hsd::u8string& str, test& t)
{
    sscanf(str.c_str(), "%d%d%f", &t.a, &t.b, &t.d);
}

int main()
{
    hsd::i32 x, z;
    hsd::f32 y;
    hsd::io::print<L"hello, {} and other words\n">(123.2);
    
    hsd::io::err_print<L"{}\n">(test{});
    hsd::io::print<L"{}\n">(test{21, 1, "how is this possible", 69.42342});
    auto t = hsd::io::read_line().parse<test>();
    
    hsd::io::read_line().set_data(x, y, z);
    hsd::io::print<L"{}, {}, {}\n">(x, y, z);
    
    auto file = hsd::file(
        "/home/catalin/Desktop/Programming"
        "/HackySTL/Tests/Io/test.txt",
        hsd::file::options::text::read
    );
    
    auto c = file.read_line().parse<hsd::i32>();
}