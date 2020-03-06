#include "WcharHelper.h"

namespace fw
{
ToWchar::ToWchar(const std::string& s)
{
    const size_t size = s.length() + 1;
    wideChar = new wchar_t[size];
    mbstowcs(wideChar, s.c_str(), size);
}

ToWchar::~ToWchar()
{
    delete wideChar;
}

wchar_t* ToWchar::getWchar() const
{
    return wideChar;
}

ToChar::ToChar(const wchar_t* s)
{
    const size_t len = wcslen(s) + 1;
    mbChar = new char[len];
    wcstombs(mbChar, s, len);
}

ToChar::~ToChar()
{
    delete mbChar;
}

char* ToChar::getChar() const
{
    return mbChar;
}
} // namespace fw
