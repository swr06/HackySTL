#pragma once

#include <stdexcept>

#include "Utility.hpp"
#include "Types.hpp"

namespace hsd
{
    template <typename T> class forward_list;

    namespace forward_list_detail
    {
        template <typename T>
        class iterator
        {
        private:
            struct forward_list_impl
            {
                T _value;
                forward_list_impl* _next = nullptr;

                HSD_CONSTEXPR forward_list_impl() {}

                constexpr forward_list_impl(const T& val)
                    : _value{val}
                {}

                constexpr forward_list_impl(T&& val)
                {
                    _value = hsd::move(val);
                }
            };
            
            forward_list_impl *_iterator = nullptr;
            friend class forward_list<T>;

            HSD_CONSTEXPR iterator() {}
            constexpr iterator(hsd::NullType) {}

            constexpr iterator(const iterator& other)
            {
                _iterator = other._iterator;
            }

            constexpr iterator& operator=(const iterator& rhs)
            {
                _iterator = rhs._iterator;
                return *this;
            }

            constexpr bool operator==(const iterator& rhs)
            {
                return _iterator == rhs._iterator;
            }

            constexpr T* get()
            {
                return &_iterator->_value;
            }

            constexpr T* get() const
            {
                return &_iterator->_value;
            }

            HSD_CONSTEXPR void push_back(const T& value)
            {
                _iterator->_next = new forward_list_impl(value);
                _iterator = _iterator->_next;
            }

            HSD_CONSTEXPR void push_back(T&& value)
            {
                _iterator->_next = new forward_list_impl(hsd::move(value));
                _iterator = _iterator->_next;
            }

            template <typename... Args>
            HSD_CONSTEXPR void emplace_back(Args&&... args)
            {
                _iterator->_next = new forward_list_impl();
                _iterator->_next->_value.~T();
                new (&_iterator->_next->_value) T(hsd::forward<Args>(args)...);
                _iterator = _iterator->_next;
            }

            HSD_CONSTEXPR void push_front(const T& value)
            {
                forward_list_impl* _element = new forward_list_impl(value);
                _element->_next = _iterator;
                _iterator = _element;
            }

            HSD_CONSTEXPR void push_front(T&& value)
            {
                forward_list_impl* _element = new forward_list_impl(hsd::move(value));
                _element->_next = _iterator;
                _iterator = _element;
            }

            template <typename... Args>
            HSD_CONSTEXPR void emplace_front(Args&&... args)
            {
                forward_list_impl* _element = new forward_list_impl();
                _element->_next = _iterator;
                _element->_value.~T();
                new (&_element->_value) T(hsd::forward<Args>(args)...);
                _iterator = _element;
            }

            HSD_CONSTEXPR void pop_front()
            {
                forward_list_impl* _element = _iterator;
                _iterator = _iterator->_next;
                delete _element;
            }
        public:
        
            constexpr bool operator!=(const iterator& rhs)
            {
                return _iterator != rhs._iterator;
            }

            constexpr iterator& operator++()
            {
                if(_iterator == nullptr)
                    throw std::runtime_error("Null pointer access denied");

                _iterator = _iterator->_next;
                return *this;
            }

            constexpr iterator operator++(i32)
            {
                iterator tmp = *this;
                operator++();
                return tmp;
            }

            constexpr T& operator*()
            {
                if(_iterator == nullptr)
                    throw std::runtime_error("Null pointer access denied");

                return _iterator->_value;
            }

            constexpr T& operator*() const
            {
                if(_iterator == nullptr)
                    throw std::runtime_error("Null pointer access denied");

                return _iterator->_value;
            }

            constexpr T* operator->()
            {
                if(_iterator == nullptr)
                    throw std::runtime_error("Null pointer access denied");

                return get();
            }

            constexpr T* operator->() const
            {
                if(_iterator == nullptr)
                    throw std::runtime_error("Null pointer access denied");

                return get();
            }
        };
    } // namespace forward_list_detail

    template <typename T>
    class forward_list
    {
    private:
        forward_list_detail::iterator<T> _head;
        forward_list_detail::iterator<T> _tail;
        
    public:
        using iterator = forward_list_detail::iterator<T>;
        using const_iterator = const iterator;
        HSD_CONSTEXPR forward_list() {}

        HSD_CONSTEXPR forward_list(const forward_list& other)
        {
            for(const auto& _element : other)
                push_back(_element);
        }

        HSD_CONSTEXPR forward_list(forward_list&& other)
        {
            _head = other._head;
            _tail = other._tail;
            other._head = nullptr;
            other._tail = nullptr;
        }

        HSD_CONSTEXPR forward_list(const std::initializer_list<T>& other)
        {
            for(const auto& _element : other)
                push_back(_element);
        }

        HSD_CONSTEXPR forward_list(std::initializer_list<T>&& other)
        {
            for(auto&& _element : other)
                push_back(move(_element));
        }

        HSD_CONSTEXPR ~forward_list()
        {
            clear();
        }

        HSD_CONSTEXPR forward_list& operator=(const forward_list& rhs)
        {
            clear();
            
            for(const auto& _element : rhs)
                push_back(_element);
            
            return *this;
        }

        HSD_CONSTEXPR forward_list& operator=(forward_list&& rhs)
        {
            _head = rhs._head;
            _tail = rhs._tail;
            rhs._head = nullptr;
            rhs._tail = nullptr;
            
            return *this;
        }

        HSD_CONSTEXPR forward_list& operator=(const std::initializer_list<T>& other)
        {
            clear();

            for(const auto& _element : other)
                push_back(_element);

            return *this;
        }

        HSD_CONSTEXPR forward_list& operator=(std::initializer_list<T>&& other)
        {
            clear;

            for(auto&& _element : other)
                push_back(move(_element));

            return *this;
        }

        HSD_CONSTEXPR forward_list& operator+(const forward_list& rhs)
        {
            for(auto _element : rhs)
                push_back(_element);

            return *this;
        }

        HSD_CONSTEXPR void push_back(const T& value)
        {
            if(_head == end())
            {
                _head._iterator = new typename iterator::forward_list_impl(value);
                _tail = _head;
            }
            else
                _tail.push_back(value);
        }

        HSD_CONSTEXPR void push_back(T&& value)
        {
            if(_head == end())
            {
                _head._iterator = new typename iterator::forward_list_impl(hsd::move(value));
                _tail = _head;
            }
            else
                _tail.push_back(hsd::move(value));
        }

        template <typename... Args>
        HSD_CONSTEXPR void emplace_back(Args&&... args)
        {
            if(_head == end())
            {
                _head._iterator = new typename iterator::forward_list_impl();
                _head._iterator->_value.~T();
                new (&_head._iterator->_value) T(hsd::forward<Args>(args)...);
                _tail = _head;
            }
            else
                _tail.emplace_back(hsd::forward<Args>(args)...);
        }

        HSD_CONSTEXPR void push_front(const T& value)
        {
            if(_head == end())
            {
                _head._iterator = new typename iterator::forward_list_impl(value);
                _tail = _head;
            }
            else
                _head.push_front(value);
        }

        HSD_CONSTEXPR void push_front(T&& value)
        {
            if(_head == end())
            {
                _head._iterator = new typename iterator::forward_list_impl(hsd::move(value));
                _tail = _head;
            }
            else
                _head.push_front(hsd::move(value));
        }

        template <typename... Args>
        HSD_CONSTEXPR void emplace_front(Args&&... args)
        {
            if(_head == end())
            {
                _head._iterator = new typename iterator::forward_list_impl();
                _head._iterator->_value.~T();
                new (&_head._iterator->_value) T(hsd::forward<Args>(args)...);
                _tail = _head;
            }
            else
                _head.emplace_front(hsd::forward<Args>(args)...);
        }

        HSD_CONSTEXPR void pop_front()
        {
            if(_head != end())
                _head.pop_front();
            else
                _tail = _head;
        }

        HSD_CONSTEXPR void clear()
        {
            for(; _head != end(); pop_front());
                pop_front();
        }

        constexpr bool empty()
        {
            return _head == end();
        }

        constexpr T& front()
        {
            return *_head.get();
        }

        constexpr T& back()
        {
            return *_tail.get();
        }

        constexpr iterator begin()
        {
            return _head;
        }

        constexpr iterator begin() const
        {
            return _head;
        }

        constexpr iterator end()
        {
            return iterator(nullptr);
        }

        constexpr iterator end() const
        {
            return iterator(nullptr);
        }

        constexpr const_iterator cbegin()
        {
            return begin();
        }

        constexpr const_iterator cbegin() const
        {
            return begin();
        }

        constexpr const_iterator cend()
        {
            return end();
        }

        constexpr const_iterator cend() const
        {
            return end();
        }
    };
} // namespace hsd