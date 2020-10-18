#pragma once

#include "_IoDetail.hpp"

namespace hsd
{
    class io : private io_detail::bufferable
    {
    public:
        static u8sstream& read_line()
        {
            do
            {
                _u8io_buf.reset_data();
                char* _str = fgets(_u8io_buf.data(), 4096, stdin);

                if(_str == nullptr)
                    throw std::runtime_error("Input requirements not satisfied");
            }while(_u8io_buf.c_str()[0] == '\n');

            return _u8io_buf;
        }

        static u8sstream& read()
        {
            do
            {
                _u8io_buf.reset_data();
                scanf("%s", _u8io_buf.data());
            }while(_u8io_buf.c_str()[0] == '\n');

            return _u8io_buf;
        }

        static wsstream& wread_line()
        {
            do
            {
                _wio_buf.reset_data();
                wchar* _str = fgetws(_wio_buf.data(), 4096, stdin);

                if(_str == nullptr)
                    throw std::runtime_error("Input requirements not satisfied");
            }while(_wio_buf.c_str()[0] == '\n');

            return _wio_buf;
        }

        static wsstream& wread()
        {
            do
            {
                _wio_buf.reset_data();
                wscanf(L"%ls", _wio_buf.data());
            }while(_wio_buf.c_str()[0] == '\n');

            return _wio_buf;
        }

        template< io_detail::string_literal fmt, typename... Args >
        static void print(Args&&... args)
        {
            using char_type = decltype(fmt)::char_type;
            using io_detail::_print;
            constexpr auto _fmt_buf = io_detail::split<fmt, sizeof...(Args) + 1>();
            static_assert(_fmt_buf.second == sizeof...(Args), "Arguments don\'t match");

            constexpr auto _len = _fmt_buf.first[sizeof...(Args)].second;
            constexpr io_detail::string_literal<char_type, _len + 1> _last{
                _fmt_buf.first[sizeof...(Args)].first, _len
            };

            [&]<usize... Ints>(index_sequence<Ints...>)
            {
                (_print<io_detail::string_literal< char_type, _fmt_buf.first[Ints].second + 1 >{
                    _fmt_buf.first[Ints].first, _fmt_buf.first[Ints].second}>(args), ...);
            }(make_index_sequence<sizeof...(Args)>{});

            _print<_last>();
        }

        template< io_detail::string_literal fmt, typename... Args >
        static void err_print(Args&&... args)
        {
            using char_type = decltype(fmt)::char_type;
            using io_detail::_print;
            constexpr auto _fmt_buf = io_detail::split<fmt, sizeof...(Args) + 1>();
            static_assert(_fmt_buf.second == sizeof...(Args), "Arguments don\'t match");

            constexpr auto _len = _fmt_buf.first[sizeof...(Args)].second;
            constexpr io_detail::string_literal<char_type, _len + 1> _last{
                _fmt_buf.first[sizeof...(Args)].first, _len
            };

            [&]<usize... Ints>(index_sequence<Ints...>)
            {
                (_print<io_detail::string_literal< char_type, _fmt_buf.first[Ints].second + 1 >{
                    _fmt_buf.first[Ints].first, _fmt_buf.first[Ints].second}>(args, stderr), ...);
            }(make_index_sequence<sizeof...(Args)>{});

            _print<_last>(stderr);
        }
    };

    class file : private io_detail::bufferable
    {
    private:
        FILE* _file_buf = nullptr;
        const char* _file_mode;

        bool only_write()
        {
            return (u8cstring::compare(_file_mode, "w") == 0
            || u8cstring::compare(_file_mode, "wb") == 0);
        }

        bool only_read()
        {
            return (u8cstring::compare(_file_mode, "r") == 0
            || u8cstring::compare(_file_mode, "rb") == 0);
        }

    public:
        struct options
        {
            struct text
            {
                static inline constexpr char read[] = "r";
                static inline constexpr char write[] = "w";
                static inline constexpr char append[] = "a";
                static inline constexpr char read_write[] = "r+";
                static inline constexpr char rw_create[] = "w+";
                static inline constexpr char rw_append[] = "a+";
            };

            struct binary
            {
                static inline constexpr char read[] = "rb";
                static inline constexpr char write[] = "wb";
                static inline constexpr char append[] = "ab";
                static inline constexpr char read_write[] = "r+b";
                static inline constexpr char rw_create[] = "w+b";
                static inline constexpr char rw_append[] = "a+b";
            };
        };

        file(const char* file_path, const char* open_option = hsd::file::options::text::read)
        {
            _file_buf = fopen(file_path, open_option);
            _file_mode = open_option;
            
            if(_file_buf == nullptr)
                throw std::runtime_error("File not found");
        }

        ~file()
        {
            fclose(_file_buf);
        }

        u8sstream& read_line()
        {
            _u8io_buf.reset_data();
            
            if(only_write())
            {
                throw std::runtime_error("Cannot read file. It is in write mode");
            }
            if(fgets(_u8io_buf.data(), 4096, _file_buf) == nullptr)
            { 
                _u8io_buf.reset_data();
            }
            
            return _u8io_buf;
        }

        wsstream& wread_line()
        {
            _wio_buf.reset_data();
            
            if(only_write())
            {
                throw std::runtime_error("Cannot read file. It is in write mode");
            }
            if(fgetws(_wio_buf.data(), 4096, _file_buf) == nullptr)
            { 
                _wio_buf.reset_data();
            }
            
            return _wio_buf;
        }

        u8sstream& read()
        {
            _u8io_buf.reset_data();

            if(only_write())
            {
                throw std::runtime_error("Cannot read file. It is in write mode");
            }
            if(fscanf(_file_buf, "%s", _u8io_buf.data()) == EOF)
            {
                _u8io_buf.reset_data();
            }
            
            return _u8io_buf;
        }

        wsstream& wread()
        {
            _wio_buf.reset_data();

            if(only_write())
            {
                throw std::runtime_error("Cannot read file. It is in write mode");
            }
            if(fwscanf(_file_buf, L"%ls", _wio_buf.data()) == EOF)
            {
                _wio_buf.reset_data();
            }

            return _wio_buf;
        }

        template< io_detail::string_literal fmt, typename... Args >
        void print(Args&&... args)
        {
            if(only_read())
                throw std::runtime_error("Cannot write file. It is in read mode");

            using char_type = decltype(fmt)::char_type;
            using io_detail::_print;
            constexpr auto _fmt_buf = io_detail::split<fmt, sizeof...(Args) + 1>();
            static_assert(_fmt_buf.second == sizeof...(Args), "Arguments don\'t match");

            constexpr auto _len = _fmt_buf.first[sizeof...(Args)].second;
            constexpr io_detail::string_literal<char_type, _len + 1> _last{
                _fmt_buf.first[sizeof...(Args)].first, _len
            };

            [&]<usize... Ints>(index_sequence<Ints...>)
            {
                (_print<io_detail::string_literal< char_type, _fmt_buf.first[Ints].second + 1 >{
                    _fmt_buf.first[Ints].first, _fmt_buf.first[Ints].second}>(args, _file_buf), ...);
            }(make_index_sequence<sizeof...(Args)>{});

            _print<_last>(_file_buf);
        }
    };
} // namespace hsd
