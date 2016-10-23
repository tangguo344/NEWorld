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

#ifndef TAKEDATAHELPER_H_
#define TAKEDATAHELPER_H_
#include <memory>

// A helper class for taking data out of a byte array
class TakeDataHelper
{
public:
    TakeDataHelper(std::shared_ptr<char> buffer, size_t length)
        : m_buffer(buffer.get()), m_length(length), m_offset(0), m_shared_ptr(buffer)
    {
    }

    template <typename T>
    std::enable_if_t<std::is_pod<T>::value, T> take()
    {
        if (m_offset + sizeof(T) > m_length) throw std::runtime_error("Failed to take! Length is too big.");
        T ret = *reinterpret_cast<T*>(m_buffer + m_offset);
        m_offset += sizeof(T);
        return ret;
    }

    std::string getString()
    {
        std::string ret(m_buffer+ m_offset);
        m_offset += ret.length();
        if (m_offset >= m_length) throw std::runtime_error("Failed to take! Length is too big.");
        return ret;
    }

private:
    char* m_buffer;
    size_t m_length;
    size_t m_offset;
    std::shared_ptr<char> m_shared_ptr;
};

#endif // !TAKEDATAHELPER_H_
