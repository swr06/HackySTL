#pragma once

#include "_NetworkDetail.hpp"

#ifdef HSD_PLATFORM_LINUX

namespace hsd
{
    namespace udp
    {
        namespace client_detail
        {
            class socket
            {
            private:
                i32 _listening = 0;
                sockaddr_in6 _hintv6{0};
                sockaddr_in _hintv4{0};
                net::protocol_type _protocol;

            public:
                socket(net::protocol_type protocol = net::protocol_type::ipv4, 
                    u16 port = 54000, const char* ip_addr = "127.0.0.1")
                {
                    _protocol = protocol;

                    if(_protocol == net::protocol_type::ipv4)
                    {
                        _listening = ::socket(static_cast<i32>(_protocol), net::socket_type::dgram, 0);
                        _hintv4.sin_family = static_cast<i32>(_protocol);
                        _hintv4.sin_port = htons(port);
                        inet_pton(static_cast<i32>(_protocol), ip_addr, &_hintv4.sin_addr);
                        bind(_listening, reinterpret_cast<sockaddr*>(&_hintv4), sizeof(_hintv4));
                    }
                    else
                    {
                        _listening = ::socket(static_cast<i32>(_protocol), net::socket_type::dgram, 0);
                        _hintv6.sin6_family = static_cast<i32>(_protocol);
                        _hintv6.sin6_port = htons(port);
                        inet_pton(static_cast<i32>(_protocol), ip_addr, &_hintv6.sin6_addr);
                        bind(_listening, reinterpret_cast<sockaddr*>(&_hintv6), sizeof(_hintv6));
                    }
                }

                ~socket()
                {
                    close();
                }

                void close()
                {
                    ::close(_listening);
                }

                i32 get_listening()
                {
                    return _listening;
                }

                sockaddr* get_hint()
                {
                    if(_protocol == net::protocol_type::ipv4)
                        return reinterpret_cast<sockaddr*>(&_hintv4);
                    
                    return reinterpret_cast<sockaddr*>(&_hintv6);
                }

                void switch_to(net::protocol_type protocol, u16 port, const char* ip_addr)
                {
                    close();
                    _protocol = protocol;

                    if(_protocol == net::protocol_type::ipv4)
                    {
                        _listening = ::socket(static_cast<i32>(_protocol), net::socket_type::dgram, 0);
                        _hintv4.sin_family = static_cast<i32>(_protocol);
                        _hintv4.sin_port = htons(port);
                        inet_pton(static_cast<i32>(_protocol), ip_addr, &_hintv4.sin_addr);
                        bind(_listening, reinterpret_cast<sockaddr*>(&_hintv4), sizeof(_hintv4));
                    }
                    else
                    {
                        _listening = ::socket(static_cast<i32>(_protocol), net::socket_type::dgram, 0);
                        _hintv6.sin6_family = static_cast<i32>(_protocol);
                        _hintv6.sin6_port = htons(port);
                        inet_pton(static_cast<i32>(_protocol), ip_addr, &_hintv6.sin6_addr);
                        bind(_listening, reinterpret_cast<sockaddr*>(&_hintv6), sizeof(_hintv6));
                    }
                }
            };
        } // namespace client_detail

        class client
        {
        private:
            client_detail::socket _sock;
            sockaddr_in6 _hintv6{0};
            sockaddr_in _hintv4{0};
            socklen_t _len = sizeof(sockaddr_in);
            net::protocol_type _protocol = net::protocol_type::ipv4;
            hsd::u8sstream _net_buf{4095};

            void _clear_buf()
            {
                memset(_net_buf.data(), '\0', 4096);
            }

        public:
            client() = default;
            ~client() = default;

            client(net::protocol_type protocol, u16 port, const char* ip_addr)
                : _sock{protocol, port, ip_addr}, _protocol{protocol}
            {
                if(_protocol == net::protocol_type::ipv6)
                    _len = sizeof(sockaddr_in6);
            }

            hsd::pair< hsd::u8sstream&, net::received_state > receive()
            {
                _clear_buf();
                isize _response = 0;
                
                if(_protocol == net::protocol_type::ipv4)
                {
                    _response = recvfrom(_sock.get_listening(), _net_buf.data(), 
                        4096, 0, reinterpret_cast<sockaddr*>(&_hintv4), &_len);
                }
                else
                {
                    _response = recvfrom(_sock.get_listening(), _net_buf.data(), 
                        4096, 0, reinterpret_cast<sockaddr*>(&_hintv6), &_len);
                }
                if (_response == static_cast<isize>(net::received_state::err))
                {
                    hsd::io::err_print<"Error in receiving\n">();
                    _clear_buf();
                    return {_net_buf, net::received_state::err};
                }
                if (_response == static_cast<isize>(net::received_state::disconnected))
                {
                    hsd::io::err_print<"Server down\n">();
                    _clear_buf();
                    return {_net_buf, net::received_state::disconnected};
                }

                return {_net_buf, net::received_state::ok};
            }

            template < usize N, typename... Args >
            net::received_state respond(const char (&fmt)[N], Args&&... args)
            {
                hsd::vector<hsd::u8string> _fmt_buf = io_detail::split(fmt, N - 1);
                hsd::vector<hsd::u8string> _args_buf = {
                    hsd::forward<hsd::u8string>(hsd::u8string::to_string(args))...
                };
                hsd::u8string _send_buf;

                if(_args_buf.size() != _fmt_buf.size() && _args_buf.size() + 1 != _fmt_buf.size())
                {
                    throw std::runtime_error("Arguments don\'t match");
                }
                else
                {
                    usize index = 0;

                    for(; index < _args_buf.size(); index++)
                    {
                        _send_buf += _fmt_buf[index] + _args_buf[index];
                    }
                    if(_fmt_buf.size() != _args_buf.size())
                    {
                        _send_buf += _fmt_buf[index];
                    }

                    isize _response = 0;
                    
                    if(_protocol == net::protocol_type::ipv4)
                    {
                        _response = sendto(_sock.get_listening(), _send_buf.data(), 
                            _send_buf.size(), 0, _sock.get_hint(), _len);
                    }
                    else
                    {
                        _response = sendto(_sock.get_listening(), _send_buf.data(), 
                            _send_buf.size(), 0, _sock.get_hint(), _len);
                    }
                    if(_response == static_cast<isize>(net::received_state::err))
                    {
                        hsd::io::err_print<"Error in sending\n">();
                        return net::received_state::err;
                    }
                }

                return net::received_state::ok;
            }
        };
    } // namespace udp

    namespace tcp
    {
        namespace client_detail
        {
            class socket
            {
            private:
                i32 _sock = 0;
                sockaddr_in6 _hintv6;
                sockaddr_in _hintv4;
                net::protocol_type _protocol;

            public:
                socket(net::protocol_type protocol = net::protocol_type::ipv4, 
                    u16 port = 54000, const char* ip_addr = "127.0.0.1")
                {
                    _protocol = protocol;

                    if(_protocol == net::protocol_type::ipv4)
                    {
                        _sock = ::socket(static_cast<i32>(_protocol), net::socket_type::dgram, 0);
                        _hintv4.sin_family = static_cast<i32>(_protocol);
                        _hintv4.sin_port = htons(port);
                        inet_pton(static_cast<i32>(_protocol), ip_addr, &_hintv4.sin_addr);
                        connect(_sock, reinterpret_cast<sockaddr*>(&_hintv4), sizeof(_hintv4));
                    }
                    else
                    {
                        _sock = ::socket(static_cast<i32>(_protocol), net::socket_type::dgram, 0);
                        _hintv6.sin6_family = static_cast<i32>(_protocol);
                        _hintv6.sin6_port = htons(port);
                        inet_pton(static_cast<i32>(_protocol), ip_addr, &_hintv6.sin6_addr);
                        connect(_sock, reinterpret_cast<sockaddr*>(&_hintv6), sizeof(_hintv6));
                    }
                }

                ~socket()
                {
                    close();
                }

                void close()
                {
                    ::close(_sock);
                }

                i32 get_sock()
                {
                    return _sock;
                }

                void switch_to(net::protocol_type protocol, u16 port, const char* ip_addr)
                {
                    close();
                    _protocol = protocol;

                    if(_protocol == net::protocol_type::ipv4)
                    {
                        _sock = ::socket(static_cast<i32>(_protocol), net::socket_type::stream, 0);
                        _hintv4.sin_family = static_cast<i32>(_protocol);
                        _hintv4.sin_port = htons(port);
                        inet_pton(static_cast<i32>(_protocol), ip_addr, &_hintv4.sin_addr);
                        connect(_sock, reinterpret_cast<sockaddr*>(&_hintv4), sizeof(_hintv4));
                    }
                    else
                    {
                        _sock = ::socket(static_cast<i32>(_protocol), net::socket_type::stream, 0);
                        _hintv6.sin6_family = static_cast<i32>(_protocol);
                        _hintv6.sin6_port = htons(port);
                        inet_pton(static_cast<i32>(_protocol), ip_addr, &_hintv6.sin6_addr);
                        connect(_sock, reinterpret_cast<sockaddr*>(&_hintv6), sizeof(_hintv6));
                    }
                }
            };
        } // namespace client_detail

        class client
        {
        private:
            client_detail::socket _sock;
            hsd::u8sstream _net_buf{4095};

            void _clear_buf()
            {
                memset(_net_buf.data(), '\0', 4096);
            }

        public:
            client() = default;
            ~client() = default;

            client(net::protocol_type protocol, u16 port, const char* ip_addr)
                : _sock{protocol, port, ip_addr}
            {}

            hsd::pair< hsd::u8sstream&, net::received_state > receive()
            {
                _clear_buf();
                isize _response = recv(_sock.get_sock(), 
                    _net_buf.data(), 4096, 0);

                if (_response == static_cast<isize>(net::received_state::err))
                {
                    hsd::io::err_print<"Error in receiving\n">();
                    _clear_buf();
                    return {_net_buf, net::received_state::err};
                }
                if (_response == static_cast<isize>(net::received_state::disconnected))
                {
                    hsd::io::err_print<"Server down\n">();
                    _clear_buf();
                    return {_net_buf, net::received_state::disconnected};
                }

                return {_net_buf, net::received_state::ok};
            }

            template< usize N, typename... Args >
            net::received_state respond(const char (&fmt)[N], Args&&... args)
            {
                hsd::vector<hsd::u8string> _fmt_buf = io_detail::split(fmt, N - 1);
                hsd::vector<hsd::u8string> _args_buf = {
                    hsd::forward<hsd::u8string>(hsd::u8string::to_string(args))...
                };
                hsd::u8string _send_buf;

                if(_args_buf.size() != _fmt_buf.size() && _args_buf.size() + 1 != _fmt_buf.size())
                {
                    throw std::runtime_error("Arguments don\'t match");
                }
                else
                {
                    hsd::usize index = 0;

                    for(; index < _args_buf.size(); index++)
                    {
                        _send_buf += _fmt_buf[index] + _args_buf[index];
                    }
                    if(_fmt_buf.size() != _args_buf.size())
                    {
                        _send_buf += _fmt_buf[index];
                    }

                    isize _response = send(_sock.get_sock(), 
                        _send_buf.data(), _send_buf.size(), 0);

                    if(_response == static_cast<isize>(net::received_state::err))
                    {
                        hsd::io::err_print<"Error in sending\n">();
                        return net::received_state::err;
                    }
                }

                return net::received_state::ok;
            }
        };
    } // namespace tcp
} // namespace hsd

#endif