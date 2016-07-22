/*
UILib - A Open-Source UI-Library

Copyright(C) 2016 Infinideastudio-UITeam

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and / or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#include <string>
#include <vector>
#include <boost/spirit/home/support/detail/hold_any.hpp>

namespace UI
{
    namespace Data
    {
        std::vector<std::string> split(const std::string& src, std::string separate_character);
        void trim(std::string& s);
        std::string strtolower(const std::string& s);

        //Some Basic Conversations with String
        inline bool isDecimal(const std::string& str)
        {
            if (str.empty())
                return false;
            bool dot = false;
            for (char c : str)
            {
                if (c == '.' && !dot)
                    dot = true;
                else if (c<'0' || c>'9')
                    return false;
            }
            return true;
        }

        inline bool isInteger(const std::string& str)
        {
            if (str.empty())
                return false;
            for (char c : str)
                if (c<'0' || c>'9')
                    return false;
            return true;
        }

        inline bool isBoolean(const std::string& str)
        {
            if (str.empty())
                return false;
            return strtolower(str) == "true" || strtolower(str) == "false";
        }

        inline bool isString(const std::string& str)
        {
            char c1 = str[0], c2 = str[str.length() - 1];
            return (c1 == '"') && (c2 == '"');
        }

        inline double getDecimal(const std::string& str)
        {
            return std::stod(str);
        }

        inline int getInteger(const std::string& str)
        {
            return std::stoi(str);
        }

        inline bool getBoolean(const std::string& str)
        {
            return strtolower(str) == "true";
        }

        inline std::string getString(const std::string& str)
        {
            return str.substr(1, str.length() - 2);
        }

        namespace Convert
        {
            boost::spirit::hold_any StrToAny(const std::string& str);
            std::string ToString(boost::spirit::hold_any var);
        }

        enum class SimpleMapFileExceptions : size_t
        {
            IOFailureOnRead, IOFailureOnWrite
        };

    }
}
