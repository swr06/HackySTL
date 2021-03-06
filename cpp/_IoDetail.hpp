#pragma once

#include <math.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdexcept>

#include "SStream.hpp"
#include "StackArray.hpp"
#include "Pair.hpp"
#include "Tuple.hpp"

namespace hsd
{
    namespace io_detail
    {
        class bufferable
        {
        protected:
            static inline u8sstream _u8io_buf{4096};
            static inline wsstream _wio_buf{4096};
        };

        template <typename CharT, usize N>
        class string_literal
        {
        public:
            CharT data[N]{};
            using char_type = CharT;

            consteval string_literal() {}

            consteval string_literal(const CharT (&str)[N])
            {
                copy_n(str, N, data);
            }

            template <usize L1, usize L2>
            consteval string_literal(const string_literal<CharT, L1>& strr, const CharT (&strl)[L2])
            {
                copy_n(strr.data, L1, data);
                copy_n(strl, L2, (data + L1 - 1));
            }

            consteval string_literal(const CharT* str, usize len)
            {
                copy_n(str, len, data);
            }

            consteval usize size() const
            {
                return N;
            }
        };

        template <typename CharT, usize L1, usize L2>
        string_literal(const string_literal<CharT, L1>, const CharT (&str)[L2]) -> string_literal<CharT, L1 + L2>;

        template <usize N>
        using u8string_literal = string_literal<char, N>;
        template <usize N>
        using wstring_literal = string_literal<wchar, N>;

        template <string_literal fmt, usize N>
        static constexpr auto split()
        {
            using char_type = decltype(fmt)::char_type;
            usize _index = 0;
            stack_array< pair<const char_type*, usize>, N > _buf;
            const char_type* _iter_f = fmt.data;
            const char_type* _iter_s = cstring<char_type>::find(_iter_f, '{');

            if(_iter_s != nullptr && *(_iter_s + 1) != '}')
            {
                throw std::runtime_error("invalid character after \'{\'");
            }
            while (_iter_s != nullptr && *_iter_s != '\0')
            {
                _buf[_index++] = {_iter_f, static_cast<usize>(_iter_s - _iter_f)};
                _iter_f = _iter_s + 2;
                _iter_s = cstring<char_type>::find(_iter_f, '{');

                if(_iter_s != nullptr && *(_iter_s + 1) != '}')
                    throw std::runtime_error("invalid character after \'{\'");
            }

            _buf[_index] = {_iter_f, static_cast<usize>(fmt.data + fmt.size() - _iter_f)};
            return pair{_buf, _index};
        }

        template <u8string_literal str>
        static void _print(FILE* file_buf = stdout)
        {
            fprintf(file_buf, str.data);
        }

        template <u8string_literal str>
        static void _print(char val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "%c").data, val);
        }

        template <u8string_literal str>
        static void _print(uchar val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "%c").data, val);
        }

        template <u8string_literal str>
        static void _print(i16 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "%hd").data, val);
        }

        template <u8string_literal str>
        static void _print(u16 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "%hu").data, val);
        }

        template <u8string_literal str>
        static void _print(i32 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "%d").data, val);
        }

        template <u8string_literal str>
        static void _print(u32 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "%u").data, val);
        }

        template <u8string_literal str>
        static void _print(i64 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "%lld").data, val);
        }

        template <u8string_literal str>
        static void _print(u64 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "%llu").data, val);
        }

        template <u8string_literal str>
        static void _print(isize val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "%zd").data, val);
        }

        template <u8string_literal str>
        static void _print(usize val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "%zu").data, val);
        }

        template <u8string_literal str>
        static void _print(f32 val, FILE* file_buf = stdout)
        {
            auto _res = abs(floor(val) - val);

            if((_res < 0.0001 && _res != 0) || abs(val) > 1.e+10)
            {
                fprintf(file_buf, string_literal(str, "%e").data, val);
            }
            else
            {
                fprintf(file_buf, string_literal(str, "%f").data, val);
            }
        }

        template <u8string_literal str>
        static void _print(f64 val, FILE* file_buf = stdout)
        {
            auto _res = abs(floor(val) - val);

            if((_res < 0.0001 && _res != 0) || abs(val) > 1.e+10)
            {
                fprintf(file_buf, string_literal(str, "%e").data, val);
            }
            else
            {
                fprintf(file_buf, string_literal(str, "%lf").data, val);
            }
        }

        template <u8string_literal str>
        static void _print(f128 val, FILE* file_buf = stdout)
        {
            auto _res = abs(floor(val) - val);

            if((_res < 0.0001 && _res != 0) || abs(val) > 1.e+10)
            {
                fprintf(file_buf, string_literal(str, "%Le").data, val);
            }
            else
            {
                fprintf(file_buf, string_literal(str, "%Lf").data, val);
            }
        }

        template <u8string_literal str>
        static void _print(const char* val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "%s").data, val);
        }

        template <u8string_literal str>
        static void _print(const u8string& val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "%s").data, val.c_str());
        }
        
        template <u8string_literal str, typename... Args>
        static void _print(const tuple<Args...>& val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, string_literal(str, "(").data);

            if constexpr(val.size() == 1)
            {
                _print<"">(val.template get<0>(), file_buf);
            }
            else if constexpr(val.size() > 1)
            {
                [&]<usize... Ints>(index_sequence<Ints...>) {
                    ((
                        _print<"">(val.template get<Ints>(), file_buf), 
                        _print<", ">(file_buf)
                    ), ...);
                }(make_index_sequence<val.size() - 1>{});
                
                _print<"">(val.template get<val.size() - 1>());
            }

            fprintf(file_buf, ")");
        }

        template <wstring_literal str>
        static void _print(FILE* file_buf = stdout)
        {
            fwprintf(file_buf, str.data);
        }

        template <wstring_literal str>
        static void _print(wchar val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%lc").data, val);
        }

        template <wstring_literal str>
        static void _print(char val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%c").data, val);
        }

        template <wstring_literal str>
        static void _print(uchar val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%c").data, val);
        }

        template <wstring_literal str>
        static void _print(i16 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%hd").data, val);
        }

        template <wstring_literal str>
        static void _print(u16 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%hu").data, val);
        }

        template <wstring_literal str>
        static void _print(i32 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%d").data, val);
        }

        template <wstring_literal str>
        static void _print(u32 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%u").data, val);
        }

        template <wstring_literal str>
        static void _print(i64 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%lld").data, val);
        }

        template <wstring_literal str>
        static void _print(u64 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%llu").data, val);
        }

        template <wstring_literal str>
        static void _print(isize val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%zd").data, val);
        }

        template <wstring_literal str>
        static void _print(usize val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%zu").data, val);
        }

        template <wstring_literal str>
        static void _print(f32 val, FILE* file_buf = stdout)
        {
            auto _res = abs(floor(val) - val);

            if((_res < 0.0001 && _res != 0) || abs(val) > 1.e+10)
            {
                fwprintf(file_buf, string_literal(str, L"%e").data, val);
            }
            else
            {
                fwprintf(file_buf, string_literal(str, L"%f").data, val);
            }
        }

        template <wstring_literal str>
        static void _print(f64 val, FILE* file_buf = stdout)
        {
            auto _res = abs(floor(val) - val);

            if((_res < 0.0001 && _res != 0) || abs(val) > 1.e+10)
            {
                fwprintf(file_buf, string_literal(str, L"%e").data, val);
            }
            else
            {
                fwprintf(file_buf, string_literal(str, L"%lf").data, val);
            }
        }

        template <wstring_literal str>
        static void _print(f128 val, FILE* file_buf = stdout)
        {
            auto _res = abs(floor(val) - val);

            if((_res < 0.0001 && _res != 0) || abs(val) > 1.e+10)
            {
                fwprintf(file_buf, string_literal(str, L"%Le").data, val);
            }
            else
            {
                fwprintf(file_buf, string_literal(str, L"%Lf").data, val);
            }
        }

        template <wstring_literal str>
        static void _print(const char* val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%s").data, val);
        }

        template <wstring_literal str>
        static void _print(const wchar* val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%ls").data, val);
        }

        template <wstring_literal str>
        static void _print(u8string& val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%s").data, val.c_str());
        }

        template <wstring_literal str>
        static void _print(wstring& val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"%ls").data, val.c_str());
        }

        template <wstring_literal str, typename... Args>
        static void _print(const tuple<Args...>& val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, string_literal(str, L"(").data);

            if constexpr(val.size() == 1)
            {
                _print<L"">(val.template get<0>(), file_buf);
            }
            else if constexpr(val.size() > 1)
            {
                [&]<usize... Ints>(index_sequence<Ints...>) {
                    ((
                        _print<L"">(val.template get<Ints>(), file_buf), 
                        _print<L", ">(file_buf)
                    ), ...);
                }(make_index_sequence<val.size() - 1>{});
                
                _print<L"">(val.template get<val.size() - 1>());
            }

            fwprintf(file_buf, L")");
        }
    }
} // namespace hsd
