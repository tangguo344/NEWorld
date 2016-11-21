/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This file is part of NEWorld.
* NEWorld is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEWorld is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CORE_NWVECTOR_HPP
#define CORE_NWVECTOR_HPP

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include "../common/debug.h"

namespace nwstd
{
    // NEWorld Simple Vector Implementation (Loose Safety Checking)
    template<class T>
    class vector
    {
    public:
        using reference = T&;
        using const_reference = const T&;
        using iterator = T*;
        using const_iterator = const T*;
        using size_type = unsigned long long;
        using difference_type = long long;
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reverse_iterator = iterator;
        using const_reverse_iterator = const_iterator;
        vector() = default;
        explicit vector(size_type n) { resize(n); };
        vector(const vector& v)
        { resize(v.mSize); std::copy(v.begin(), v.end(), begin()); }
        vector(vector&& v)
        { mPtr = v.mPtr; mMaxSize = v.mMaxSize; mSize = v.mSize; v.mPtr = nullptr; }
        vector(std::initializer_list<T> l)
        { resize(l.size()); std::copy(l.begin(), l.end(), begin()); }
        ~vector() { if(mPtr) { clear(); free(mPtr); mPtr = nullptr; } }

        vector& operator=(const vector& v)
        { resize(v.mSize); std::copy(v.begin(), v.end(), begin()); return *this; }
        vector& operator=(vector&& v)
        { mPtr = v.mPtr; mMaxSize = v.mMaxSize; mSize = v.mSize; v.mPtr = nullptr; return *this; }
        vector& operator=(std::initializer_list<T> l)
        { resize(l.size()); std::copy(l.begin(), l.end(), begin());  return *this; }

        // iterators
        iterator begin() noexcept { return mPtr; }
        const_iterator begin() const noexcept { return mPtr; }
        iterator end() noexcept { return mPtr + mSize; }
        const_iterator end() const noexcept { return mPtr + mSize; }
        reverse_iterator rbegin() noexcept { return mPtr + size - 1; }
        const_reverse_iterator rbegin() const noexcept { return mPtr + size - 1; }
        reverse_iterator rend() noexcept { return mPtr - 1; }
        const_reverse_iterator rend() const noexcept { return mPtr - 1; }
        const_iterator cbegin() const noexcept { return mPtr; }
        const_iterator cend() const noexcept { return mPtr + mSize; }
        const_reverse_iterator crbegin() const noexcept { return mPtr + size - 1; }
        const_reverse_iterator crend() const noexcept { return mPtr - 1; }
        // capacity
        size_type size() const noexcept { return mSize; }
        size_type max_size() const noexcept { return mMaxSize; }
        void resize(size_type sz)
        {
            Assert(sz > mSize);
			if (sz <= mMaxSize)
			{
				mSize = sz;
				return;
			}
            auto size = 1 << static_cast<difference_type>(ceil(log2(sz + 1)));
            auto ptr = static_cast<pointer>(realloc(mPtr, size * sizeof(value_type)));
			if (ptr != nullptr)
			{
				mSize = sz;
				mPtr = ptr;
				mMaxSize = size;
			}
			else
			{
				fatalstream << "nwvector reallocation failure!";
			}
			Assert(ptr != nullptr);
        }
        size_type capacity() const noexcept { return mMaxSize; }
        bool empty() const noexcept { return mSize == 0; }
        void reserve(size_type n) { resize(mSize + n); }
        void shrink_to_fit() { resize(mSize); }

        // element access
        reference operator[](size_type offset) noexcept
        { Assert(offset < mSize); return *(mPtr + offset); }
        const_reference operator[](size_type offset) const noexcept
        { Assert(offset < mSize); return *(mPtr + offset); }
        reference at(size_type offset) noexcept
        { Assert(offset < mSize); return *(mPtr + offset); }
        const_reference at(size_type offset) const noexcept
        { Assert(offset < mSize); return *(mPtr + offset); }
        reference front() { Assert(!empty()); return *mPtr; }
        const_reference front() const { Assert(!empty()); return *mPtr; }
        reference back() { Assert(!empty()); return *(mPtr + mSize - 1); }
        const_reference back() const { Assert(!empty()); return *(mPtr + mSize - 1); }

        // data access
        pointer data() noexcept { return mPtr; }
        const_pointer data() const noexcept { return mPtr; }

        // modifiers
        template <class ...Args> iterator emplace_back(Args&& ...args)
        {
            resize(mSize + 1);
			back()->T(std::forward<Args>(args)...);
            return back();
        }
        iterator push_back(const T& x)
        {
            resize(mSize + 1);
            return &(*back() = x);
        }
        iterator push_back(T&& x)
        {
            resize(mSize + 1);
            return &(*back() = std::move(x));
        }
        void pop_back() { (mPtr + (--mSize))->~T(); }

        template <class ...Args> iterator emplace(const_iterator position, Args&& ...args)
        {
            size_type pos = position - mPtr;
            Assert(pos < size());
            resize(mSize + 1);
            memmove((mPtr + pos + 1), mPtr + pos, (mSize - pos) * sizeof(value_type));
			(mPtr + pos)->T(std::forward<Args>(args)...);
            return (mPtr + pos);
        }
        iterator insert(const_iterator position, const T& x)
        {
            size_type pos = position - mPtr;
            Assert(pos < size());
            resize(mSize + 1);
            memmove((mPtr + pos + 1), mPtr + pos, (mSize - pos) * sizeof(value_type));
            return &(*(mPtr + pos) = x);
        }
        iterator insert(const_iterator position, T&& x)
        {
            size_type pos = position - mPtr;
            Assert(pos < size());
            resize(mSize + 1);
            memmove((mPtr + pos + 1), mPtr + pos, (mSize - pos) * sizeof(value_type));
            return &(*(mPtr + pos) = std::move(x));
        }
        iterator insert(const_iterator position, size_type n, const T& x)
        {
            size_type pos = position - mPtr; 
            Assert(pos < size());
            resize(mSize + n);
            memmove(mPtr + pos + n, mPtr + pos, (mSize - pos) * sizeof(value_type));
            for (size_type i = 0; i < n; ++i) *(mPtr + pos + i) = x;
            return mPtr + pos;
        }
        template <class InputIterator>
        iterator insert(const_iterator position, InputIterator first, InputIterator last)
        {
            size_type pos = position - mPtr;
            size_type n = last - first;
            Assert(pos < size());
            resize(mSize + n);
            memmove(mPtr + pos + n, mPtr + pos, (mSize - pos) * sizeof(value_type));
            std::copy(first, last, mPtr + pos);
            return mPtr + pos;
        }
        iterator insert(const_iterator position, std::initializer_list<T> li)
        { return insert(position, li.begin(), li.end());}
        iterator erase(const_iterator position)
        {
            size_type pos = position - mPtr;
            Assert(pos < size());
            (mPtr + pos)->~T();
            memmove(mPtr + pos, mPtr + pos + 1, (--mSize - pos) * sizeof(value_type));
            return mPtr + pos;
        }
        void clear() { for (auto&& x : *this) x.~T(); mSize = 0; }
        void swap(vector& rhs)
        {std::swap(mPtr, rhs.mPtr); std::swap(mMaxSize, rhs.mMaxSize); std::swap(mSize, rhs.mSize);}
    private:
        pointer mPtr = nullptr;
        size_type mSize = 0, mMaxSize = 0;
    };
}
#endif //CORE_NWVECTOR_H
